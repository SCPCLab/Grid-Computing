#include "integrationTest.h"

extern "C"
{
void slave_integration(DataSet *dataSet_edge, DataSet *dataSet_vertex,
    label *row, label *col);
}
static inline unsigned long rpcc()
{
	unsigned long time;
	__asm__("rtc %0": "=r" (time) : );
	return time;
}
void mycheckResult(scalar *array1, scalar *array2, label count) 
{ 
    std::cout<<"check result..."<<std::endl; 
    int cnt = 0;
    for(int i=0;i<(count);i++) { 
        if(std::fabs(array1[i]-array2[i])>EPS) { 
            double eps = 1e-14;
            int k = 14;
            while(std::fabs(array1[i]-array2[i])>eps){
                eps *= 10;
                k--;
            }
            cnt++;
            printf("%d ", k);
        } 
    } 
    if(cnt > 0) {
        printf("cnt = %d\n", cnt);
        exit(-1);
    }else{
        std::cout<<"The result is correct!"<<std::endl;
    }
}

void integration_test(label* row, label* col, label n_edge, label n_vertex)
{
    // 声明数据结构
    DataSet dataSet_edge_s, dataSet_vertex_s;
    DataSet dataSet_edge_m, dataSet_vertex_m;
    ArrayArray U, flux, U_test;

    srand((int)time(0));
    // 初始化边状态和点状态
    U.initRandom   (3, n_vertex);
    flux.initRandom(3, n_edge);
    U_test.clone(U);

    // 初始化状态集
    initDataSet(&dataSet_edge_s);
    initDataSet(&dataSet_edge_m);
    initDataSet(&dataSet_vertex_s);
    initDataSet(&dataSet_vertex_m);

    // 将边状态、点状态封装进状态集
    addSingleArray(dataSet_edge_s, flux, COPYIN);
    addSingleArray(dataSet_vertex_s, U, COPYINOUT);

    addSingleArray(dataSet_edge_m, flux, COPYIN);
    addSingleArray(dataSet_vertex_m, U_test, COPYINOUT);
printf("\t\t integration\n");
unsigned long start, end;
double t1 = 0.0, t2 = 0.0;
start = rpcc();
    // 从核计算部分
    slave_computation(&dataSet_edge_s, &dataSet_vertex_s, row, col, slave_integration);
end = rpcc();
t1 = (end - start) / 1.45e9;
start = rpcc();
    // 主核计算部分
    integration(&dataSet_edge_m, &dataSet_vertex_m, row, col);
end = rpcc();
t2 = (end - start) / 1.45e9;
printf("The slave computation time is %lf S\n", t1);
printf("The host  computation time is %lf S\n", t2);

    // 校验结果
    mycheckResult(U.data, U_test.data, n_vertex*3);
    // checkResult(U.data, U_test.data, n_vertex*3);
}
