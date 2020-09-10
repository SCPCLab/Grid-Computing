#ifndef WRAPPER_HPP
#define WRAPPER_HPP

#include "macro.h"
#include "dataSet.h"


void slave_computation(DataSet *dataSet_edge, DataSet *dataSet_vertex,
    label *row, label *col, FunPtr funPtr);



#endif
