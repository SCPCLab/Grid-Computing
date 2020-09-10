#ifndef READFILE_HPP
#define READFILE_HPP

#include <cstdio>
#include <iostream>

using namespace std;

// #define NONZERONUM 40200

int* readFile(char* name, int *n_edge){
    FILE *fp = fopen(name,"r");
    if(fp==NULL){
        cout<<"File not exist!!!"<<endl;
        exit(-1);
    }
    char c1[100],c2[100];
    int num;
    while(1)
    {
        fgets(c1,100,fp);
        if(c1[0]=='('){
            num = atoi(c2);
            break;
        }
        fgets(c2,100,fp);
        if(c2[0]=='('){
            num = atoi(c1);
            break;
        }else{
        }
    }
    int *buff = (int*)malloc(sizeof(int)*num);
    int i=0;
    while(i<num)
    {
        fgets(c1,100,fp);
        buff[i] = atoi(c1);
        i++;
    }
    *n_edge = num;
    // printf("InternalFaceNum = %d\n",cellNum);
    return buff;
}

label getVertexNum(label *owner, label *neighbor, label n_edge)
{
    label n_vertex=-1;
    for (int i = 0; i < n_edge; ++i)
    {
        n_vertex = max(n_vertex, owner[i]);
        n_vertex = max(n_vertex, neighbor[i]);
    }
    return n_vertex+1;
}


#endif
