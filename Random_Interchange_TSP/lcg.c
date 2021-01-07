#include "lcg.h"

unsigned long long randULL(LCG *lcg){
	lcg->current=((lcg->a)*(lcg->current)+(lcg->c))%(lcg->m);
	return lcg->current;
}

double randDBLBetween(LCG *lcg,double min,double max){
	/* The left term of the product returns a random number between 0 and 1. 
	Obviously, the random numbers we get this way do not cover the full range of double values. */
	return min+(((double) randULL(lcg))/((lcg->m)-1))*(max-min);
}

int randINTBetween(LCG *lcg,int min,int max){
	return randULL(lcg)%(max-min+1)+min;
}

void DurstenfeldShuffle(LCG *lcg, int arrayEls, int array[]){
	int a;
	int b;
	int temp;
	
	for(a=0; a<=arrayEls-2; a++){
		b=randINTBetween(lcg,a,arrayEls-1);
		temp=array[a];
		array[a]=array[b];
		array[b]=temp;
	}
}

//array[] must have N+1 elements
void randomPermutationFrom0toN(LCG* lcg, int N,int array[]){
	int a;
	
	for(a=0; a<=N; a++){
		array[a]=a;
	}
	
	DurstenfeldShuffle(lcg,N+1,array);
}