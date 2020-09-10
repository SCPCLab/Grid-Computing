#include "macro.h"
#include "slave.h"
// #include "slaveUtils.h"
#include "kernel_slave.h"
// #define TIME

static inline unsigned long rpcc(){
    unsigned long time;
    asm("rcsr %0,4":  "=r" (time) : );
    return time; 
}

extern scalar ***buffer;
extern label **table;
extern label cnts[64];

__thread_local label *row;                  // 行号
__thread_local label *col;                  // 列号
__thread_local label *map;                  // 映射关系   map[i] -> i
__thread_local scalar **edgeArry;           // 边信息
__thread_local scalar **vertexArry;         // 点信息
__thread_local label fArrayNum_edge;        
__thread_local label fArrayNum_vertex;
__thread_local label id;


inline void wait_reply(volatile unsigned int *reply, int m){
	while(*reply != m) {};
}
void computation_kernel(SlavePara *para)
{
    int i, j;
    int eleCnt = para->counts[id];              // 从核当前边数
    int displ = para->displs[id];               // 偏移
    DataSet edge = *(para->dataSet_edge);       
    DataSet vertex = *(para->dataSet_vertex);

    if(eleCnt == 0) {
        cnts[id] = 0;
        return;
    }
    edge.floatArrays = edgeArry;
    vertex.floatArrays = vertexArry;

    volatile unsigned int get_reply, put_reply; 

    get_reply = 0;    
    athread_get(PE_MODE, para->row + displ, row, sizeof(label) * eleCnt, &get_reply, 0, 0, 0);      // 获取行号
    wait_reply(&get_reply, 1);
    get_reply = 0;    
    athread_get(PE_MODE, para->col + displ, col, sizeof(label) * eleCnt, &get_reply, 0, 0, 0);      // 获取列号
    wait_reply(&get_reply, 1);

    for(j = 0; j < fArrayNum_edge; j++){                                                            // 获取边状态
        int dim = edge.fArrayDims[j];
        get_reply = 0;
        athread_get(    PE_MODE, 
                        para->dataSet_edge->floatArrays[j] + displ * dim,
                        edge.floatArrays[j],
                        sizeof(scalar) * eleCnt * dim,
                        &get_reply,
                        0, 0, 0
                    );
        wait_reply(&get_reply, 1);
    }

    int pre = -1, cnt = 0;          // pre: 上一个行号  cnt: 重排的点数
    for(i = 0; i < eleCnt; i++){    // 行号重排 
        int r = row[i];
        if(r == pre){               // 旧顶点
            row[i] = cnt - 1;       // 重映射为最后一个顶点号    
        }else{                      // 新顶点
            row[i] = cnt;           // 重映射为最后一个顶点号 + 1
            map[cnt] = r;           // 修改映射表
            cnt++;                  // 点数增加
        }           
        pre = r;                    // 更新上一个行号
    }
    for(i = 0; i < eleCnt; i++){
        int c = col[i];
        for(j = 0; j < cnt; j++){
            if(map[j] == c){        // 该列号已经重排
                col[i] = j;
                break;
            }
        }
        if(j == cnt){               // 新顶点
            col[i] = cnt;
            map[cnt] = c;
            cnt++;
        }
    }
    cnts[id] = cnt;                 // put从核点的数量

    for(j = 0; j < fArrayNum_vertex; j++){
        int dim = vertex.fArrayDims[j];
        int pos = 0, dataLen = 1;   // pos: 起始位置  dataLen: 连续的点的数量
        pre = map[0];               // 上一个点初始化为第一个点
        for(i = 1; i < cnt; i++){   
            if(map[i] != pre + 1){  // 当不连续时，读取数据
                get_reply = 0;
                athread_get(    PE_MODE,
                                para->dataSet_vertex->floatArrays[j] + map[pos] * dim,  // map[pos] 为映射之前的位置
                                vertex.floatArrays[j] + pos * dim,                      // 读取从pos开始dataLen个点状态
                                sizeof(scalar) * dataLen * dim,
                                &get_reply,
                                0, 0, 0
                            );
                wait_reply(&get_reply, 1);
                pos = i;            // 更新起始位置
                dataLen = 0;        // 重置数据长度
            }
            pre = map[i];           
            dataLen++;              // 累加数据长度
        }
        get_reply = 0;              
        athread_get(    PE_MODE,    // 读取剩余的数据
                        para->dataSet_vertex->floatArrays[j] + map[pos] * dim,
                        vertex.floatArrays[j] + pos * dim,
                        sizeof(scalar) * dataLen * dim,
                        &get_reply,
                        0, 0, 0
                    );
        wait_reply(&get_reply, 1);
    }

    edge.fArraySize = eleCnt;       // 设置边数
    vertex.fArraySize = cnt;        // 设置点数
    
    para->funPtr(&edge, &vertex, row, col);     // 调用算子计算

    get_reply = 0;
    athread_put(PE_MODE, map, table[id], sizeof(label) * cnt, &get_reply, 0, 0);    // put映射表 
    wait_reply(&get_reply, 1);

    for(j = 0; j < fArrayNum_edge; j++){                                            // put边状态
        if(edge.fArrayInOut[j] == COPYIN) continue;
        int dim = edge.fArrayDims[j];
        put_reply = 0;
        athread_put(    PE_MODE, 
                        edge.floatArrays[j],
                        para->dataSet_edge->floatArrays[j] + displ * dim,
                        sizeof(scalar) * eleCnt * dim,                              // eleCnt为边数量
                        &put_reply,
                        0, 0
                    );
        wait_reply(&put_reply, 1);
    }

    for(j = 0; j < fArrayNum_vertex; j++){                                          // put点状态 (连续)
        if(vertex.fArrayInOut[j] == COPYIN) continue;
        int dim = vertex.fArrayDims[j];
        put_reply = 0;
        athread_put(    PE_MODE, 
                        vertex.floatArrays[j],
                        buffer[id][j],
                        sizeof(scalar) * cnt * dim,                                 // cnt为点数量
                        &put_reply,
                        0, 0
                    );
        wait_reply(&put_reply, 1);
    }
}
void init(SlavePara *para){     // 分配内存
    id = athread_get_id(-1);
    int i, j, eleCnt = para->counts[0];
    DataSet *edge = para->dataSet_edge;
    DataSet *vertex = para->dataSet_vertex;

    row = (label*)ldm_malloc(sizeof(label) * eleCnt); 
    col = (label*)ldm_malloc(sizeof(label) * eleCnt);
    map = (label*)ldm_malloc(sizeof(label) * 2 * eleCnt);

    fArrayNum_edge = edge->fArrayNum; 
    fArrayNum_vertex = vertex->fArrayNum;

    edgeArry = (scalar**)ldm_malloc(sizeof(scalar*) * fArrayNum_edge);
    vertexArry = (scalar**)ldm_malloc(sizeof(scalar*) * fArrayNum_vertex);

    for(i = 0; i < fArrayNum_edge; i++) edgeArry[i] = (scalar*)ldm_malloc(sizeof(scalar) * eleCnt * edge->fArrayDims[i]);
    for(i = 0; i < fArrayNum_vertex; i++) vertexArry[i] = (scalar*)ldm_malloc(sizeof(scalar) * 2 * eleCnt * vertex->fArrayDims[i]);
}
void destory(SlavePara *para){  // 释放内存
    int i, j, eleCnt = para->counts[0];
    DataSet *edge = para->dataSet_edge;
    DataSet *vertex = para->dataSet_vertex;

    ldm_free(row, sizeof(label) * eleCnt);
    ldm_free(col, sizeof(label) * eleCnt);
    ldm_free(map, sizeof(label) * 2 * eleCnt);

    for(i = 0; i < fArrayNum_edge; i++) ldm_free(edgeArry[i], sizeof(scalar) * eleCnt * edge->fArrayDims[i]);
    ldm_free(edgeArry, sizeof(scalar*) * fArrayNum_edge);

    for(i = 0; i < fArrayNum_vertex; i++) ldm_free(vertexArry[i], sizeof(scalar) * 2 * eleCnt * vertex->fArrayDims[i]);
    ldm_free(vertexArry, sizeof(scalar*) * fArrayNum_vertex);
}