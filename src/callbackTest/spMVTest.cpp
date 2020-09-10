#include "spMVTest.h"

extern "C"
{
void slave_spMV(DataSet *dataSet_edge, DataSet *dataSet_vertex,
    label *row, label *col);
}
static inline unsigned long rpcc()
{
	unsigned long time;
	__asm__("rtc %0": "=r" (time) : );
	return time;
}
void spMV_test(label* row, label* col, label n_edge, label n_vertex)
{
    DataSet dataSet_edge_s, dataSet_vertex_s;
    DataSet dataSet_edge_m, dataSet_vertex_m;
    ArrayArray A, x, b, b_test;

    srand((int)time(0));
    A.initRandom(1, n_edge);
    x.initRandom(1, n_vertex);
    b.initRandom(1, n_vertex);
    b_test.clone(b);

    initDataSet(&dataSet_edge_s);
    initDataSet(&dataSet_vertex_s);

    initDataSet(&dataSet_edge_m);
    initDataSet(&dataSet_vertex_m);

    addSingleArray(dataSet_edge_s,   A, COPYIN);
    addSingleArray(dataSet_vertex_s, x, COPYIN);
    addSingleArray(dataSet_vertex_s, b, COPYINOUT);

    addSingleArray(dataSet_edge_m,   A, COPYIN);
    addSingleArray(dataSet_vertex_m, x, COPYIN);
    addSingleArray(dataSet_vertex_m, b_test, COPYINOUT);
printf("\t\t spMV\n");
unsigned long start, end;
double t1 = 0.0, t2 = 0.0;
start = rpcc();
    // 从核计算部分
    slave_computation(&dataSet_edge_s, &dataSet_vertex_s, row, col, slave_spMV);
end = rpcc();
t1 = (end - start) / 1.45e9;
start = rpcc();
    // 主核计算部分
    spMV(&dataSet_edge_m, &dataSet_vertex_m, row, col);
end = rpcc();
t2 = (end - start) / 1.45e9;
printf("The slave computation time is %lf S\n", t1);
printf("The host  computation time is %lf S\n", t2);
    // 校验结果
    checkResult(b.data, b_test.data, n_vertex);
}
