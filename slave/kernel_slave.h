#ifndef KERNEL_SLAVE_H
#define KERNEL_SLAVE_H

#include "macro.h"
#include "dataSet.h"
#include "callback.h"

#ifdef __cplusplus
extern "C" {
#endif 

typedef struct
{
    label* row;
    label* col;

    DataSet* dataSet_edge;
    DataSet* dataSet_vertex;

    FunPtr funPtr;
    int *counts;    // 边数
    int *displs;    // 偏移
} SlavePara;

#ifdef __cplusplus
}
#endif 

#endif
