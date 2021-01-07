#include "utilities.h"

//Returns a pointer which points to a double 1D Array in the Heap.
//The caller is responsible for freeing the memory when he is done with it.
double * doubleArray1DInHeap(int dim1){
	double * pointer;
	
	pointer=(double *) malloc(dim1*sizeof(double));
	
	return pointer;
}

//Returns a pointer which points to a double 2D Array in the Heap.
//The caller is responsible for freeing the memory when he is done with it.
double ** doubleArray2DInHeap(int dim1, int dim2){
	int a;
	double ** pointer;
	
	pointer=(double**) malloc(dim1*sizeof(double *));	//Allocate space in the heap for dim1 pointers to double
	for(a=0; a<dim1; a++){
		pointer[a]=doubleArray1DInHeap(dim2);	//Allocate space in the heap for dim2 double values for each of the dim1 pointers
	}
	
	return pointer;
}

//Returns a pointer which points to an int 1D Array in the Heap.
//The caller is responsible for freeing the memory when he is done with it.
int * intArray1DInHeap(int dim1){
	int * pointer;
	
	pointer=(int *) malloc(dim1*sizeof(int));
	
	return pointer;
}

//Returns a pointer which points to a double 2D Array in the Heap.
//The caller is responsible for freeing the memory when he is done with it.
int ** intArray2DInHeap(int dim1, int dim2){
	int a;
	int ** pointer;
	
	pointer=(int**) malloc(dim1*sizeof(int *));	//Allocate space in the heap for dim1 pointers to int
	for(a=0; a<dim1; a++){
		pointer[a]=intArray1DInHeap(dim2);	//Allocate space in the heap for dim2 int values for each of the dim1 pointers
	}
	
	return pointer;
}

//Set all array components to the same value
void setArrayI(int * array,int size,int value){
	int a;
	
	for(a=0; a<size; a++){
		array[a]=value;
	}
}

void setTableD(double **table,int rows,int cols,double value){
	int a,b;
	double *line;
	
	for(a=0; a<rows; a++){
		line=table[a];
		for(b=0; b<cols; b++){
			line[b]=value;
		}
	}
	
}