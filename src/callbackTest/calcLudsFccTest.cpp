#include "calcLudsFcc.h"

extern "C"
{
void slave_calcLudsFcc(DataSet *dataSet_edge, DataSet *dataSet_vertex,
    label *row, label *col);
}

static inline unsigned long rpcc()
{
	unsigned long time;
	__asm__("rtc %0": "=r" (time) : );
	return time;
}

void calcLudsFcc_test(label* row, label* col, label n_edge, label n_vertex)
{
    DataSet dataSet_edge_s, dataSet_vertex_s;
    DataSet dataSet_edge_m, dataSet_vertex_m;

    ArrayArray massFlux, cellx, facex, fcc, rface0, rface1, S;
    ArrayArray fcc_test, rface0_test, rface1_test, S_test;

    // init edge data
    srand((int)time(0));
    massFlux.initRandom(1, n_edge);
    facex.initRandom   (1, n_edge);
    fcc.initRandom     (1, n_edge);
    rface0.initRandom  (1, n_edge);
    rface1.initRandom  (1, n_edge);

    fcc_test.clone(fcc);
    rface0_test.clone(rface0);
    rface1_test.clone(rface1);

    // init vertex data
    cellx.initRandom(1, n_vertex);
    S.initRandom    (1, n_vertex);

    S_test.clone(S);

    initDataSet(&dataSet_edge_s);
    initDataSet(&dataSet_vertex_s);

    initDataSet(&dataSet_edge_m);
    initDataSet(&dataSet_vertex_m);

    addSingleArray(dataSet_edge_s, massFlux, COPYIN);
    addSingleArray(dataSet_edge_s, facex,    COPYIN);
    addSingleArray(dataSet_edge_s, fcc,      COPYOUT);
    addSingleArray(dataSet_edge_s, rface0,   COPYOUT);
    addSingleArray(dataSet_edge_s, rface1,   COPYOUT);

    addSingleArray(dataSet_vertex_s, cellx, COPYIN);
    addSingleArray(dataSet_vertex_s, S,     COPYINOUT);

    addSingleArray(dataSet_edge_m, massFlux,    COPYIN);
    addSingleArray(dataSet_edge_m, facex,       COPYIN);
    addSingleArray(dataSet_edge_m, fcc_test,    COPYOUT);
    addSingleArray(dataSet_edge_m, rface0_test, COPYOUT);
    addSingleArray(dataSet_edge_m, rface1_test, COPYOUT);

    addSingleArray(dataSet_vertex_m, cellx,  COPYIN);
    addSingleArray(dataSet_vertex_m, S_test, COPYINOUT);
printf("\t\t calcLudsFcc\n");
unsigned long start, end;
double t1 = 0.0, t2 = 0.0;
start = rpcc();
    // 从核计算部分
    slave_computation(&dataSet_edge_s, &dataSet_vertex_s, row, col, slave_calcLudsFcc);
end = rpcc();
t1 = (end - start) / 1.45e9;
start = rpcc();
    // 主核计算部分
    calcLudsFcc(&dataSet_edge_m, &dataSet_vertex_m, row, col);
end = rpcc();
t2 = (end - start) / 1.45e9;
printf("The slave computation time is %lf S\n", t1);
printf("The host  computation time is %lf S\n", t2);
    // 校验结果
    checkResult(fcc.data,    fcc_test.data,    n_edge);
    checkResult(rface0.data, rface0_test.data, n_edge);
    checkResult(rface1.data, rface1_test.data, n_edge);
    checkResult(S.data,      S_test.data,      n_vertex);
}
