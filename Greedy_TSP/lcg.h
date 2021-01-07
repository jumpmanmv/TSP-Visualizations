#ifndef LCG_H
#define LCH_H

/* Represents a Linear Congruential Generation (LCG).
It returns a uniformly distributed random number 
from 0 to m-1.
To use it first declare a LCG variable and then
keep passing its address to the appropriate function
Example values for its initialization:
{44485709377909ULL,11863279ULL,281474976710656ULL,time(NULL)}
(taken from: https://nuclear.llnl.gov/CNP/rng/rngman/node5.html) */
typedef struct LCG{
	unsigned long long a;	//Multiplier
	unsigned long long c;	//Increment
	unsigned long long m;	//Modulus
	unsigned long long current;
} LCG;

unsigned long long randULL(LCG *lcg);
double randDBLBetween(LCG *lcg,double min,double max);
int randINTBetween(LCG *lcg,int min,int max);
void DurstenfeldShuffle(LCG *lcg, int arrayEls, int array[]);
void randomPermutationFrom0toN(LCG *lcg, int N,int array[]);

#endif

	

