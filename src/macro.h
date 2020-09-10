#ifndef MACRO_H
#define MACRO_H

#ifdef __cplusplus
#include <cstdlib>
#include <cmath>
#else
#include <stdlib.h>
#include <math.h>
#endif 



#define EPS 1e-14

typedef int label;
typedef double scalar;

#define Terminate(location, content) \
{ \
    printf("Location: \033[31m%s\033[0m, error message: \033[31m%s\033[0m, file: \033[31m%s\033[0m, line: \033[31m%d\033[0m\n", \
        location, content, __FILE__, __LINE__); \
    exit(-1); \
}

#define DELETE_POINTER(ptr) if(ptr) \
{ \
    delete [] (ptr); \
    ptr = NULL; \
}

#define checkResult(array1, array2, count) \
{ \
    std::cout<<"check result..."<<std::endl; \
    for(int i=0;i<(count);i++) \
    { \
        if(std::fabs(array1[i]-array2[i])>EPS) \
        { \
            if(array1[i]==0) \
            { \
                if(std::fabs(array2[i])>EPS) \
                { \
                    printf("Error on index[%d], %.8f, %.8f\n", \
                                i, array1[i], array2[i]); \
                    std::exit(-1); \
                } \
            } \
            else if(std::fabs((array1[i]-array2[i])/array1[i])>EPS) \
            { \
                printf("Error on index[%d], %.8f, %.8f\n", \
                            i, array1[i], array2[i]); \                         
                std::exit(-1); \
            } \
        } \
    } \
    std::cout<<"The result is correct!"<<std::endl; \
}

#define MAX(x, y) ( ((x) > (y)) ? (x) : (y) )
#define MIN(x, y) ( ((x) < (y)) ? (x) : (y) )

#define SMALL 1e-40


#endif
