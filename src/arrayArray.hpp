#ifndef ARRAYARRAY_HPP
#define ARRAYARRAY_HPP

#include <cstdlib>
#include "macro.h"
#include "dataSet.h"

class BasicElement
{
public:
    scalar *data;
    BasicElement()
    {
        data = NULL;
    };
    scalar& operator[](const int i){return data[i];}
};

class ArrayArray
{
public:
    label num; ///< size of structs
    label dim; ///< start index of structs
    scalar*    data; ///< structs
    BasicElement *basicEle; /// basic elements
    /**
    * @brief constructor
    */
    ArrayArray()
    {
        basicEle = NULL;
        data = NULL;
        num = 0;
        // par_std_out_("constructor: %d\n", refCount->GetRefCount());
    };
    /**
    * @brief deconstructor
    */
    ~ArrayArray()
    {
        DELETE_POINTER(data);
        DELETE_POINTER(basicEle);
    };
    /**
    * @brief overload function of []
    */
    BasicElement& operator[](const int i)
    {
        if(basicEle)
        {
            return basicEle[i];
        } else
        {
            basicEle = new BasicElement[num];
            for (int i = 0; i < num; ++i)
            {
                basicEle[i].data = &data[i*dim];
            }
        }
    }
    /**
    * @brief initialize a ArrayArray with constant value
    */
    void initConstant(scalar value, label dim, label size)
    {
        this->num = size;
        this->dim = dim;
        this->data = new scalar[this->num*this->dim];
        for (int i = 0; i < this->num*this->dim; ++i)
        {
            this->data[i] = value;
        }
        basicEle = new BasicElement[this->num];
        for (int i = 0; i < this->num; ++i)
        {
            basicEle[i].data = &data[i*this->dim];
        }
    }

    /**
    * @brief initialize a ArrayArray with random value
    */
    void initRandom(label dim, label size)
    {
        this->num = size;
        this->dim = dim;
        this->data = new scalar[this->num*this->dim];
        for (int i = 0; i < this->num*this->dim; ++i)
        {
            this->data[i] = (scalar)(rand()%100-50)/100;
            if(fabs(this->data[i] - 0.0) < 1e-6) this->data[i] = 0.1;
        }
        basicEle = new BasicElement[this->num];
        for (int i = 0; i < this->num; ++i)
        {
            basicEle[i].data = &data[i*this->dim];
        }
    }
    /**
    * @brief clone from another ArrayArray
    */
    void clone(ArrayArray& arr)
    {
        this->num = arr.num;
        this->dim = arr.dim;
        this->data = new scalar[this->num*this->dim];
        for (int i = 0; i < this->num*this->dim; ++i)
        {
            this->data[i] = arr.data[i];
        }
        basicEle = new BasicElement[this->num];
        for (int i = 0; i < this->num; ++i)
        {
            basicEle[i].data = &data[i*this->dim];
        }
    }

    /**
    * @brief get the size of structs
    * @return the size of structs
    */
    label size() const {return num;};
};

void addSingleArray(DataSet &arrays, ArrayArray &array,
    InOut inout)
{
    label oldNum = getArrayNum(&arrays);
    scalar** floatArrays = new scalar*[oldNum+1];
    label* fArrayDims = new label[oldNum+1];
    InOut* fArrayInOut = new InOut[oldNum+1];
    for (int i = 0; i < oldNum; ++i)
    {
        floatArrays[i] = accessArray(&arrays, i);
        arrays.floatArrays[i] = NULL;
        fArrayDims[i] = getArrayDim(&arrays, i);
        fArrayInOut[i] = getArrayInOut(&arrays, i);
    }
    floatArrays[oldNum] = array.data;
    fArrayDims[oldNum] = array.dim;
    fArrayInOut[oldNum] = inout;

    DELETE_POINTER(arrays.fArrayDims);
    DELETE_POINTER(arrays.fArrayInOut);

    arrays.floatArrays = floatArrays;
    arrays.fArrayDims = fArrayDims;
    arrays.fArrayInOut = fArrayInOut;
    arrays.fArrayNum++;
    arrays.fArraySize = array.num;

    fArrayDims = NULL;
    fArrayInOut = NULL;
    floatArrays = NULL;
}

#endif
