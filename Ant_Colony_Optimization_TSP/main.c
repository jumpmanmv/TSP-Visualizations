#include <stdio.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include "lcg.h"
#include "utilities.h"
#include "render.h"

#define NUM_OF_THREADS 4
#define NUM_OF_ANTS_PER_THREAD (NUM_OF_CITIES/20)
#define NUM_OF_ANTS NUM_OF_THREADS*NUM_OF_ANTS_PER_THREAD
#define ALPHA 1
#define BETA 6
#define RHO 0.7
#define T0 1.0/(2*X_MAX+2*Y_MAX) 

void citiesInitialization(LCG *lcg,city *cities);
void constructGraph(city *cities,double **adjMatrix);
void heuristicValuesInitialization(double **adjMatrix, double **heuristicValues);
void pherTrailsInitialization(double **pherTrails);
int calcNextCity(LCG *lcg,int *memory, int noOfCitiesVisited, double **routTable);
void depositPheromone(int *memory,double dt,double **pherTrails);
void pheromoneEvaporation(double** pherTrails);
void updateRoutTable(double **pherTrails,double **heuristicValues,double **routTable);
int findBestAnt(double *pathDistance);
double calcDistanceSquared(city c1,city c2);
double calcDistance(city c1,city c2);
void printPath(int *path);

int main(int argc, char* argv[]){
	city cities[NUM_OF_CITIES];
	double **adjMatrix=doubleArray2DInHeap(NUM_OF_CITIES,NUM_OF_CITIES);
	double **pherTrails=doubleArray2DInHeap(NUM_OF_CITIES,NUM_OF_CITIES);
	double **heuristicValues=doubleArray2DInHeap(NUM_OF_CITIES,NUM_OF_CITIES);
	double **routTable=doubleArray2DInHeap(NUM_OF_CITIES,NUM_OF_CITIES);
	int **memory=intArray2DInHeap(NUM_OF_ANTS,NUM_OF_CITIES);
	double pathDistance[NUM_OF_ANTS];
	double dt;
	int ant;
	int noOfCitiesVisited;
	int nextCity;
	int iteration=0;
	int bestAnt;
	int threadID;
	char message[100];
	
	LCG lcg=(LCG) {44485709377909ULL,11863279ULL,281474976710656ULL,time(NULL)};
	
	//Core logic=======================================================================================
	citiesInitialization(&lcg,cities);
	constructGraph(cities,adjMatrix);
	heuristicValuesInitialization(adjMatrix,heuristicValues);
	pherTrailsInitialization(pherTrails);
	updateRoutTable(pherTrails,heuristicValues,routTable);

	init();
	// In the first iteration, build the city grid without any connections
	buildCityGrid(cities, NUM_OF_CITIES);
	int quit = 0;
	SDL_Event e;
	
	#pragma omp parallel num_threads(NUM_OF_THREADS) private(threadID,dt,ant,noOfCitiesVisited,nextCity,lcg)
	{
		threadID=omp_get_thread_num();
		LCG lcg=(LCG) {44485709377909ULL,11863279ULL,281474976710656ULL,&threadID};	//Each thread gets a different seed
	
		while(!quit){
			//Each thread spawns NUM_OF_ANTS_PER_THREAD ants
			for(ant=0; ant<NUM_OF_ANTS_PER_THREAD; ant++){
				memory[threadID*NUM_OF_ANTS_PER_THREAD+ant][0]=0;		//All ants start from cities[0]
				pathDistance[threadID*NUM_OF_ANTS_PER_THREAD+ant]=0;	//Reset pathDistance of this ant
				
				//Ant finds a hamiltonian cycle
				for(noOfCitiesVisited=1; noOfCitiesVisited<NUM_OF_CITIES; noOfCitiesVisited++){
					nextCity=calcNextCity(&lcg,memory[threadID*NUM_OF_ANTS_PER_THREAD+ant],noOfCitiesVisited,routTable);
					memory[threadID*NUM_OF_ANTS_PER_THREAD+ant][noOfCitiesVisited]=nextCity;
					pathDistance[threadID*NUM_OF_ANTS_PER_THREAD+ant]+=adjMatrix[memory[threadID*NUM_OF_ANTS_PER_THREAD+ant][noOfCitiesVisited-1]][nextCity];
				}
				
				//Add the distance from the last city back to the starting one
				pathDistance[threadID*NUM_OF_ANTS_PER_THREAD+ant]+=adjMatrix[nextCity][0];
				dt=1.0/pathDistance[threadID*NUM_OF_ANTS_PER_THREAD+ant];						//Although each ant calls depositPheromone as soon as it finds a hamiltonian cycle, the changes in the pherTrails
				depositPheromone(memory[threadID*NUM_OF_ANTS_PER_THREAD+ant],dt,pherTrails);	//it causes are invisible to the ants of this iteration since ants only utilize routTable for routing decisions (which
			}																					//is updated with updateRoutTable())
			#pragma omp barrier
			
			#pragma omp single
			{
				pheromoneEvaporation(pherTrails);
				updateRoutTable(pherTrails,heuristicValues,routTable);
				
				bestAnt=findBestAnt(pathDistance);
				sprintf(message,"Iteration %d: Best Ant=%d, Minimum Path Distance = %.2f\n",iteration,bestAnt,pathDistance[bestAnt]);
				printf("%s",message);

				clearWindow();
				updateIterationText(message);
				buildCityGrid(cities, NUM_OF_CITIES);
				connectCities(cities, memory, bestAnt);
				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_QUIT)
					{
						quit = 1;
					}
					else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
					{
						quit = 1;
					}
				}
				iteration++;
			}
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

void heuristicValuesInitialization(double **adjMatrix, double **heuristicValues){
	int a,b;
	
	for(a=0; a<NUM_OF_CITIES; a++){
		for(b=0; b<NUM_OF_CITIES; b++){
			if(a==b){
				heuristicValues[a][a]=0;
			}else{
				heuristicValues[a][b]=1/adjMatrix[a][b];	
			}
		}
	}
	
}

void pherTrailsInitialization(double **pherTrails){
	setTableD(pherTrails,NUM_OF_CITIES,NUM_OF_CITIES,T0);
	
	for(int a=0; a<NUM_OF_CITIES; a++){
		pherTrails[a][a]=0;
	}
}

int calcNextCity(LCG *lcg,int *memory, int noOfCitiesVisited, double **routTable){
	int curCity=memory[noOfCitiesVisited-1];
	int possibleToTravelTo[NUM_OF_CITIES];
	double denominator;
	double probabilities[NUM_OF_CITIES];
	double limits[NUM_OF_CITIES];
	int a;
	double randomNum;
	
	//Find the possible next cities
	setArrayI(possibleToTravelTo,NUM_OF_CITIES,1);		//You can go to every city...
	for(a=0; a<noOfCitiesVisited; a++){		//No need to check the whole memory
		possibleToTravelTo[memory[a]]=0;	//...except those already visited
	}
	
	//Calculate denominator
	denominator=0;
	for(a=0; a<NUM_OF_CITIES; a++){
		if(possibleToTravelTo[a]){
			denominator+=routTable[curCity][a];
		}
	}
	
	//Calculate probabilities
	for(a=0; a<NUM_OF_CITIES; a++){
		if(possibleToTravelTo[a]){
			probabilities[a]=routTable[curCity][a]/denominator;
		}else{
			probabilities[a]=0.0;
		}
	}
	
	//Calculate limits
	limits[0]=0;
	for(a=1; a<NUM_OF_CITIES; a++){
		limits[a]=limits[a-1]+probabilities[a-1];
	}
	
	//Calculate nextCity
	randomNum=randDBLBetween(lcg,0.0,1.0);
	for(a=1; a<NUM_OF_CITIES; a++){
		//Covers every case except when limits[a-1]=0,randomNum=0,limits[a]=X
		if( limits[a-1]<randomNum && randomNum<=limits[a] ){	//(,]
			break;
		////Covers every case except when limits[a-1]=X,randomNum=1,limits[a]=1
		}else if( limits[a-1]<=randomNum && randomNum<limits[a] ){	//[,)
			break;
		}
	}
	
	return --a;
}

void depositPheromone(int *memory,double dt,double **pherTrails){
	int a;
	
	for(a=0; a<=NUM_OF_CITIES-2; a++){
		pherTrails[memory[a]][memory[a+1]]+=dt;
	}
	pherTrails[memory[a]][0]+=dt;
}

void pheromoneEvaporation(double** pherTrails){
	int a,b;
	
	for(a=0; a<NUM_OF_CITIES; a++){
		for(b=0; b<NUM_OF_CITIES; b++){
			pherTrails[a][b]*=(1-RHO);
		}
	}
}

void updateRoutTable(double **pherTrails,double **heuristicValues,double **routTable){
	int a,b;
	double numerator;
	double denominator;
	
	for(a=0; a<NUM_OF_CITIES; a++){
		//Calculate denominator
		denominator=0.0;
		for(b=0; b<NUM_OF_CITIES; b++){
			denominator+=pow(pherTrails[a][b],ALPHA)*pow(heuristicValues[a][b],BETA);
		}
		//Calculate elements
		for(b=0; b<NUM_OF_CITIES; b++){
			numerator=pow(pherTrails[a][b],ALPHA)*pow(heuristicValues[a][b],BETA);
			routTable[a][b]=numerator/denominator;
		}
	}
	
}

int findBestAnt(double *pathDistance){
	int ant;
	int bestAnt;
	double bestPath;
	double candPath;
	
	bestAnt=0;
	bestPath=pathDistance[0];
	for(ant=1; ant<NUM_OF_ANTS; ant++){
		if( (candPath=pathDistance[ant])<bestPath){
			bestAnt=ant;
			bestPath=candPath;
		}
	}
	
	return bestAnt;
}

double calcDistanceSquared(city c1,city c2){
	double x_dif=c1.x-c2.x;
	double y_dif=c1.y-c2.y;
	return x_dif*x_dif+y_dif*y_dif;
}

double calcDistance(city c1,city c2){
	return sqrt(calcDistanceSquared(c1,c2));
}

void printPath(int *path){
	int a=0;
	
	printf("%d",path[0]);
	for(a=1; a<NUM_OF_CITIES; a++){
		printf("->%d",path[a]);
	}
}