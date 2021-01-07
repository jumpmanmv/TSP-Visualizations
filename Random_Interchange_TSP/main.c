#include <stdio.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <string.h>
#include "lcg.h"
#include "utilities.h"
#include "render.h"

#define NUM_OF_THREADS 8
#define NUM_OF_ITERATIONS 5000

void citiesInitialization(LCG *lcg,city *cities);
void constructGraph(city *cities,double **adjMatrix);
void pathInitiliazation(LCG *lcg,int *path);
double calcPathDistance(double **adjMatrix,int *path);
double calcDistanceSquared(city c1,city c2);
double calcDistance(city c1,city c2);
void shareData(int path[NUM_OF_THREADS][NUM_OF_CITIES],double pathDistance[NUM_OF_THREADS],int sharedPath[NUM_OF_CITIES],double sharedPathDistance);

int main(int argc, char *argv[]){
	city cities[NUM_OF_CITIES];
	double **adjMatrix=doubleArray2DInHeap(NUM_OF_CITIES,NUM_OF_CITIES);
	int path[NUM_OF_THREADS][NUM_OF_CITIES];
	double oldPathDistance[NUM_OF_THREADS];
	double newPathDistance[NUM_OF_THREADS];
	int batch=0;
	int it;
	int a,b;
	int temp;
	int threadID;
	char message[100];
	
	LCG lcg=(LCG) {44485709377909ULL,11863279ULL,281474976710656ULL,time(NULL)};
	
	//Core logic=======================================================================================
	citiesInitialization(&lcg,cities);
	constructGraph(cities,adjMatrix);
	pathInitiliazation(&lcg,path[0]);
	shareData(path,oldPathDistance,path[0],calcPathDistance(adjMatrix,path[0]));
	
	init();
	// In the first iteration, build the city grid without any connections
	buildCityGrid(cities, NUM_OF_CITIES);
	int quit = 0;
	SDL_Event e;

	#pragma omp parallel num_threads(NUM_OF_THREADS) private(it,a,b,temp,threadID,lcg)
	{
		threadID=omp_get_thread_num();
		lcg=(LCG) {44485709377909ULL,11863279ULL,281474976710656ULL,&threadID};	//Each thread gets a different seed
		
		while(!quit){
			#pragma omp single
			{
				sprintf(message,"Batch %d, Path Distance = %.2f \n",batch,oldPathDistance[0]);	//Each entry in oldPathDistance contains the minimum
				printf("%s",message);

				clearWindow();
				updateIterationText(message);
				buildCityGrid(cities, NUM_OF_CITIES);
				connectCities(cities, path);
				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_QUIT)
					{
						quit = 1;
						break;
					}
					else if (e.key.keysym.sym == SDLK_ESCAPE)
					{
						quit = 1;
						break;
					}
				}
			}
			
			for(it=0; it<NUM_OF_ITERATIONS; it++){
				//Exchange two cities
				a=randINTBetween(&lcg,1,NUM_OF_CITIES-1);
				b=randINTBetween(&lcg,1,NUM_OF_CITIES-1);
			
				temp=path[threadID][a];
				path[threadID][a]=path[threadID][b];
				path[threadID][b]=temp;
				//Compare path distances
				newPathDistance[threadID]=calcPathDistance(adjMatrix,path[threadID]);
				if(oldPathDistance[threadID]<newPathDistance[threadID]){
					//Exchange back the two cities
					path[threadID][b]=path[threadID][a];
					path[threadID][a]=temp;
				}
				else{
					oldPathDistance[threadID]=newPathDistance[threadID];
				}
			}
			#pragma omp barrier
			
			//Find the best thread
			#pragma omp single
			{
				int ind=0;
				double min=oldPathDistance[0];
				for(int thread=1; thread<NUM_OF_THREADS; thread++){
					if(oldPathDistance[thread]<min){
						ind=thread;
						min=oldPathDistance[thread];
					}
				}
				shareData(path,oldPathDistance,path[ind],min);
			}
			
			#pragma omp single
			batch++;
		}
	}
	//=================================================================================================
	
	shutDown();
	return 0;
}

void citiesInitialization(LCG *lcg,city *cities){
	int a;
	city *c;
	
	for(a=0; a<NUM_OF_CITIES; a++){
		c=&cities[a];
		c->x=randDBLBetween(lcg,0.0, (double) X_MAX);
		c->y=randDBLBetween(lcg,0.0, (double) Y_MAX);
	}
}

void constructGraph(city *cities, double **adjMatrix){
	int a;
	int b;
	
	for(a=0; a<NUM_OF_CITIES; a++){
		for(b=0; b<NUM_OF_CITIES; b++){
			adjMatrix[a][b]=calcDistance(cities[a],cities[b]);
		}
	}
	
}

void pathInitiliazation(LCG *lcg,int *path){
	int a;
	int temp;
	
	randomPermutationFrom0toN(lcg,NUM_OF_CITIES-1,path);
	
	for(a=0; a<NUM_OF_CITIES; a++){
		if( path[a]==0 ){
			break;
		}
	}
	
	temp=path[0];
	path[0]=path[a];
	path[a]=temp;
}

double calcPathDistance(double **adjMatrix,int *path){
	double d=0.0;
	int a;
	
	for(a=0; a<=NUM_OF_CITIES-2; a++){
		d+=adjMatrix[path[a]][path[a+1]];
	}
	d+=adjMatrix[path[NUM_OF_CITIES-1]][path[0]];
	
	return d;
}

double calcDistanceSquared(city c1,city c2){
	double x_dif=c1.x-c2.x;
	double y_dif=c1.y-c2.y;
	return x_dif*x_dif+y_dif*y_dif;
}

double calcDistance(city c1,city c2){
	return sqrt(calcDistanceSquared(c1,c2));
}

void shareData(int path[NUM_OF_THREADS][NUM_OF_CITIES],double pathDistance[NUM_OF_THREADS],int sharedPath[NUM_OF_CITIES],double sharedPathDistance){
	int thread;
	
	for(thread=0; thread<NUM_OF_THREADS; thread++){
		memcpy(path[thread],sharedPath,NUM_OF_CITIES*sizeof(int));
		pathDistance[thread]=sharedPathDistance;
	}
	
}



