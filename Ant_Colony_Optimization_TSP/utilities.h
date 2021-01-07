#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdlib.h>

double * doubleArray1DInHeap(int dim1);
double ** doubleArray2DInHeap(int dim1, int dim2);
int * intArray1DInHeap(int dim1);
int ** intArray2DInHeap(int dim1, int dim2);
void setArrayI(int * array,int size,int value);
void setTableD(double **table,int rows,int cols,double value);

#endif