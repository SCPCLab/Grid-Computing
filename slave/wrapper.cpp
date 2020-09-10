
#include "kernel_slave.h"
#include "callback.h"
// #include "wrapper.hpp"

#define MAX_SIZE 49152 // 48KB
#ifdef __cplusplus
extern "C" {
#endif 

#include <athread.h>

void slave_computation_kernel(SlavePara *para);
void slave_init(SlavePara *para);
void slave_destory(SlavePara *para);

#ifdef __cplusplus
}
#endif

// #define TIME
static inline unsigned long rpcc()
{
	unsigned long time;
	__asm__("rtc %0": "=r" (time) : );
	return time;
}

scalar ***buffer;       // 从核put的点信息
label **table;          // 从核重排关系
label cnts[64];         // 从核点数量

void slave_computation(DataSet *dataSet_edge, DataSet *dataSet_vertex,
    label *row, label *col, FunPtr funPtr)
{
    int counts[64], displs[64];
    SlavePara para = {
        row,
        col,

        dataSet_edge,
        dataSet_vertex,
        
        funPtr,
        counts, displs
    };

    int n_edge = dataSet_edge->fArraySize;
    int n_vertex = dataSet_vertex->fArraySize;

    int fArrayNum_edge = dataSet_edge->fArrayNum; 
    int fArrayNum_vertex = dataSet_vertex->fArrayNum;
    
    int typeSize = 0;                     
    for(int i = 0; i < fArrayNum_edge; i++){
        typeSize += sizeof(scalar) * dataSet_edge->fArrayDims[i];       // 计算每个边节点的状态信息所需内存大小
    }
    for(int i = 0; i < fArrayNum_vertex; i++){
        typeSize += 2 * sizeof(scalar) * dataSet_vertex->fArrayDims[i]; // 计算每个点节点的状态信息所需内存大小
    }
    typeSize += 4 * sizeof(label);                 
    int eleCnt = MAX_SIZE / typeSize;                                   // ldm可以容纳的边数

    table = (label**)malloc(sizeof(label*) * 64); 
    buffer = (scalar***)malloc(sizeof(scalar**) * 64);                  // 64个从核

    for(int i = 0; i < 64; i++){
        buffer[i] = (scalar**)malloc(sizeof(scalar*) * fArrayNum_vertex);
        table[i] = (label*)malloc(sizeof(label) * 2 * eleCnt);
        for(int j = 0; j < fArrayNum_vertex; j++){                      // 每一个fArray
            buffer[i][j] = (scalar*)malloc(sizeof(scalar) * 2 * eleCnt * dataSet_vertex->fArrayDims[j]);
        }
    }
    
    scalar **vertex_temp;                                               // 点状态副本
    vertex_temp = (scalar**)malloc(sizeof(scalar*) * fArrayNum_vertex); 

    for(int j = 0; j < fArrayNum_vertex; j++){
        if(dataSet_vertex->fArrayInOut[j] == COPYIN) continue;
        vertex_temp[j] = (scalar*)malloc(sizeof(scalar) * n_vertex * dataSet_vertex->fArrayDims[j]);
        memcpy(vertex_temp[j], dataSet_vertex->floatArrays[j], sizeof(scalar) * n_vertex * dataSet_vertex->fArrayDims[j]);  // 复制点状态作为副本初值
    }

    int times = (n_edge - 1) / (eleCnt * 64) + 1;                       // 主核迭代次数
    int len = eleCnt * 64, offset = 0;
    counts[0] = eleCnt;
    
    __real_athread_spawn((void*)slave_init, &para);                     // 分配内存
    athread_join();

    for(int iter = 0; iter < times; iter++){
        if(iter == times - 1 && n_edge % len != 0) len = n_edge % len;  // 特殊情况
        int q = len / 64;    
        int r = len % 64; 
        for(int id = 0; id < 64; id++){                                 // 划分数据
            counts[id] = q + ((id < r) ? 1 : 0);                  
            displs[id] = offset + q * id + ((id < r) ? id : r);
        }        
        // 从核计算
        __real_athread_spawn((void*)slave_computation_kernel, &para);   // 从核计算
        athread_join();

        // 规约
        #pragma unroll [8] 
        for(int j = 0; j < fArrayNum_vertex; j++){
            if(dataSet_vertex->fArrayInOut[j] == COPYIN) continue;      // 跳过输入
            int dim = dataSet_vertex->fArrayDims[j];
            for(int id = 0; id < 64; id++){                             // 遍历64个从核
                for(int i = 0; i < cnts[id]; i++){                      // cnts[id] = 每个从核重排后点的数量
                    for(int k = 0; k < dim; k++){
                                                                        // 更新副本（累加增量），table[id][i]为映射之前的结点号
                        vertex_temp[j][table[id][i] * dim + k] += (buffer[id][j][i * dim + k] - dataSet_vertex->floatArrays[j][table[id][i] * dim + k]);
                    }
                }
            }
        }
        offset += len;                                                  // 偏移量增加
    }
    counts[0] = eleCnt;
    __real_athread_spawn((void*)slave_destory, &para);                  // 释放从核内存
    athread_join();

    for(int j = 0; j < fArrayNum_vertex; j++){                          // 释放主核内存
        if(dataSet_vertex->fArrayInOut[j] == COPYIN) continue;
        int dim = dataSet_vertex->fArrayDims[j];
        memcpy(dataSet_vertex->floatArrays[j], vertex_temp[j], sizeof(scalar) * n_vertex * dim);    // 复制副本
        free(vertex_temp[j]);
    }
    free(vertex_temp);
    for(int i = 0; i < 64; i++){
        for(int j = 0; j < fArrayNum_vertex; j++){
            free(buffer[i][j]);
        }
        free(buffer[i]);
        free(table[i]);
    }
    free(buffer);
    free(table);
}
    