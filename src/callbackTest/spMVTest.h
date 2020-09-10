#ifndef SPMVTEST_H
#define SPMVTEST_H

#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "macro.h"
#include "arrayArray.hpp"
#include "dataSet.h"
#include "readFile.hpp"
#include "callback.h"
#include "wrapper.hpp"

void spMV_test(label* row, label* col, label n_edge, label n_vertex);


#endif
