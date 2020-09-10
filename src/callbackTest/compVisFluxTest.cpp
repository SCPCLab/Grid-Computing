#include "compVisFluxTest.h"

extern "C"
{
void slave_compVisflux(DataSet *dataSet_edge, DataSet *dataSet_vertex,
    label *row, label *col);
}
static inline unsigned long rpcc()
{
	unsigned long time;
	__asm__("rtc %0": "=r" (time) : );
	return time;
}
void compVisFlux_test(label* row, label* col, label n_edge, label n_vertex)
{
    DataSet dataSet_edge_s, dataSet_vertex_s;
    DataSet dataSet_edge_m, dataSet_vertex_m;

    ArrayArray xfc, yfc, zfc, xfn, yfn, zfn, primitiveVariableFace;
    ArrayArray tm, deltaL, deltaR, viscousLaminarFace, viscousTurbulentFace;
    ArrayArray kCp, flux, area, flux_test;

    ArrayArray xcc, ycc, zcc, primitiveVariable, t;
    ArrayArray dqdx, dqdy, dqdz, dtdx, dtdy, dtdz;

    srand((int)time(0));
    // init edge data
    xfc.initRandom                  (1, n_edge);
    yfc.initRandom                  (1, n_edge);
    zfc.initRandom                  (1, n_edge);
    xfn.initRandom                  (1, n_edge);
    yfn.initRandom                  (1, n_edge);
    zfn.initRandom                  (1, n_edge);
    primitiveVariableFace.initRandom(5, n_edge);
    tm.initRandom                   (1, n_edge);
    deltaL.initRandom               (1, n_edge);
    deltaR.initRandom               (1, n_edge);
    viscousLaminarFace.initRandom   (1, n_edge);
    viscousTurbulentFace.initRandom (1, n_edge);
    kCp.initRandom                  (1, n_edge);
    flux.initRandom                 (5, n_edge);
    area.initRandom                 (1, n_edge);
    flux_test.clone(flux);

    // init vertex data
    xcc.initRandom              (1, n_vertex);
    ycc.initRandom              (1, n_vertex);
    zcc.initRandom              (1, n_vertex);
    primitiveVariable.initRandom(5, n_vertex);
    t.initRandom                (1, n_vertex);
    dqdx.initRandom             (5, n_vertex);
    dqdy.initRandom             (5, n_vertex);
    dqdz.initRandom             (5, n_vertex);
    dtdx.initRandom             (1, n_vertex);
    dtdy.initRandom             (1, n_vertex);
    dtdz.initRandom             (1, n_vertex);


    initDataSet(&dataSet_edge_s);
    initDataSet(&dataSet_vertex_s);

    initDataSet(&dataSet_edge_m);
    initDataSet(&dataSet_vertex_m);

    addSingleArray(dataSet_edge_s, xfc, COPYIN);
    addSingleArray(dataSet_edge_s, yfc, COPYIN);
    addSingleArray(dataSet_edge_s, zfc, COPYIN);
    addSingleArray(dataSet_edge_s, xfn, COPYIN);
    addSingleArray(dataSet_edge_s, yfn, COPYIN);
    addSingleArray(dataSet_edge_s, zfn, COPYIN);
    addSingleArray(dataSet_edge_s, primitiveVariableFace, COPYIN);
    addSingleArray(dataSet_edge_s, tm, COPYIN);
    addSingleArray(dataSet_edge_s, deltaL, COPYIN);
    addSingleArray(dataSet_edge_s, deltaR, COPYIN);
    addSingleArray(dataSet_edge_s, viscousLaminarFace, COPYIN);
    addSingleArray(dataSet_edge_s, viscousTurbulentFace, COPYIN);
    addSingleArray(dataSet_edge_s, kCp, COPYIN);
    addSingleArray(dataSet_edge_s, flux, COPYINOUT);
    addSingleArray(dataSet_edge_s, area, COPYIN);

    addSingleArray(dataSet_vertex_s, xcc, COPYIN);
    addSingleArray(dataSet_vertex_s, ycc, COPYIN);
    addSingleArray(dataSet_vertex_s, zcc, COPYIN);
    addSingleArray(dataSet_vertex_s, primitiveVariable, COPYIN);
    addSingleArray(dataSet_vertex_s, t, COPYIN);
    addSingleArray(dataSet_vertex_s, dqdx, COPYIN);
    addSingleArray(dataSet_vertex_s, dqdy, COPYIN);
    addSingleArray(dataSet_vertex_s, dqdz, COPYIN);
    addSingleArray(dataSet_vertex_s, dtdx, COPYIN);
    addSingleArray(dataSet_vertex_s, dtdy, COPYIN);
    addSingleArray(dataSet_vertex_s, dtdz, COPYIN);

    addSingleArray(dataSet_edge_m, xfc, COPYIN);
    addSingleArray(dataSet_edge_m, yfc, COPYIN);
    addSingleArray(dataSet_edge_m, zfc, COPYIN);
    addSingleArray(dataSet_edge_m, xfn, COPYIN);
    addSingleArray(dataSet_edge_m, yfn, COPYIN);
    addSingleArray(dataSet_edge_m, zfn, COPYIN);
    addSingleArray(dataSet_edge_m, primitiveVariableFace, COPYIN);
    addSingleArray(dataSet_edge_m, tm, COPYIN);
    addSingleArray(dataSet_edge_m, deltaL, COPYIN);
    addSingleArray(dataSet_edge_m, deltaR, COPYIN);
    addSingleArray(dataSet_edge_m, viscousLaminarFace, COPYIN);
    addSingleArray(dataSet_edge_m, viscousTurbulentFace, COPYIN);
    addSingleArray(dataSet_edge_m, kCp, COPYIN);
    addSingleArray(dataSet_edge_m, flux_test, COPYINOUT);
    addSingleArray(dataSet_edge_m, area, COPYIN);

    addSingleArray(dataSet_vertex_m, xcc, COPYIN);
    addSingleArray(dataSet_vertex_m, ycc, COPYIN);
    addSingleArray(dataSet_vertex_m, zcc, COPYIN);
    addSingleArray(dataSet_vertex_m, primitiveVariable, COPYIN);
    addSingleArray(dataSet_vertex_m, t, COPYIN);
    addSingleArray(dataSet_vertex_m, dqdx, COPYIN);
    addSingleArray(dataSet_vertex_m, dqdy, COPYIN);
    addSingleArray(dataSet_vertex_m, dqdz, COPYIN);
    addSingleArray(dataSet_vertex_m, dtdx, COPYIN);
    addSingleArray(dataSet_vertex_m, dtdy, COPYIN);
    addSingleArray(dataSet_vertex_m, dtdz, COPYIN);
printf("\t\t compVisflux\n");
unsigned long start, end;
double t1 = 0.0, t2 = 0.0;
start = rpcc();
    // 从核计算部分
    slave_computation(&dataSet_edge_s, &dataSet_vertex_s, row, col, slave_compVisflux);
end = rpcc();
t1 = (end - start) / 1.45e9;
start = rpcc();
    // 主核计算部分
    compVisflux(&dataSet_edge_m, &dataSet_vertex_m, row, col);
end = rpcc();
t2 = (end - start) / 1.45e9;
printf("The slave computation time is %lf S\n", t1);
printf("The host  computation time is %lf S\n", t2);

    // 校验结果
    checkResult(flux.data, flux_test.data, n_edge*5);
}
