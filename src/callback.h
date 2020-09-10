#ifndef CALLBACK_H
#define CALLBACK_H

#include "dataSet.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (* FunPtr)(DataSet *dataSet_edge, DataSet *dataSet_vertex, label *row, label *col);

void spMV(DataSet *dataSet_edge, DataSet *dataSet_vertex,
    label *row, label *col);

void integration(DataSet *dataSet_edge, DataSet *dataSet_vertex,
    label *row, label *col);

void compVisflux(DataSet *dataSet_edge, DataSet *dataSet_vertex,
    label *row, label *col);

void calcLudsFcc(DataSet *dataSet_edge, DataSet *dataSet_vertex,
    label *row, label *col);

#ifdef __cplusplus
}
#endif

#endif
