#ifndef DataSet_H
#define DataSet_H

#include <stddef.h>
#include "macro.h"

#ifdef __cplusplus
extern "C" {
#endif 

typedef enum
{
    COPYIN,
    COPYOUT,
    COPYINOUT
} InOut;

typedef struct
{
    scalar** floatArrays;
    label* fArrayDims;
    InOut* fArrayInOut;
    label fArrayNum;
    label fArraySize;
} DataSet;

void initDataSet(DataSet *arrays);

#define accessArray(arrays, i) \
( \
    (arrays)->floatArrays[i] \
)

#define getArrayDim(arrays, i) \
( \
    (arrays)->fArrayDims[i] \
)

#define getArrayNum(arrays) \
( \
    (arrays)->fArrayNum \
)

#define getArraySize(arrays) \
( \
    (arrays)->fArraySize \
)

#define getArrayInOut(arrays, i) \
( \
    (arrays)->fArrayInOut[i] \
) 


#ifdef __cplusplus
}
#endif 


#endif
