#include <stdio.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include "lcg.h"
#include "utilities.h"
#include "render.h"

void citiesInitialization(LCG *lcg,city *cities);
void constructGraph(city *cities,double **adjMatrix);
double calcDistanceSquared(city c1,city c2);
double calcDistance(city c1,city c2);

int main(int argc, char *argv[]){
	city cities[NUM_OF_CITIES];
	double **adjMatrix=doubleArray2DInHeap(NUM_OF_CITIES,NUM_OF_CITIES);
	int a;
	int next, b;
	double nextDistance, bDistance;
	int cur;
	int visited[NUM_OF_CITIES];
	setArrayI(visited,NUM_OF_CITIES,0);
	int path[NUM_OF_CITIES];
	double pathDistance;
	char message[100];
	
	LCG lcg=(LCG) {44485709377909ULL,11863279ULL,281474976710656ULL,time(NULL)};
	
	//Core logic=======================================================================================
	citiesInitialization(&lcg,cities);
	constructGraph(cities,adjMatrix);
	
	path[0]=0;
	cur=0;
	next=0;
	pathDistance=0;
	visited[0]=1;

	init();
	// In the first iteration, build the city grid without any connections
	buildCityGrid(cities, NUM_OF_CITIES);
	SDL_Event e;
	int quit = 0;

	//Loop NUM_OF_CITIES-1 times.
	for(a=0; a<=NUM_OF_CITIES-2 && !quit; a++){
		sprintf(message,"Determining element: %d, Path distance: %.2f\n", a+1, pathDistance);
		printf("%s",message);

		clearWindow();
		updateIterationText(message);
		buildCityGrid(cities, NUM_OF_CITIES);
		
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = 1;
			}
			else if (e.key.keysym.sym == SDLK_ESCAPE)
			{
				quit = 1;
			}
		}
		
		nextDistance=DBL_MAX;
		//For each of the possible destinations from cur
		for(b=0; b<NUM_OF_CITIES; b++){
			//Find the one that has the minimum distance AND has NOT been visited
			if( (bDistance=adjMatrix[cur][b])<nextDistance && visited[b]==0 ){
				next=b;
				nextDistance=bDistance;
			}
		}
		
		

		path[a+1]=next;
		cur=next;
		pathDistance+=nextDistance;
		visited[next]=1;
	}
	
	//Add the distance from the last city back to the starting one
	pathDistance+=adjMatrix[cur][0];
	sprintf(message,"Final Path Distance: %.2f. Press ESC or click the X to exit...\n",pathDistance);
	printf("%s",message);

	clearWindow();
	updateIterationText(message);
	buildCityGrid(cities, NUM_OF_CITIES);
	connectCities(cities, path);

	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = 1;
			}
			else if (e.key.keysym.sym == SDLK_ESCAPE)
			{
				quit = 1;
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

double calcDistanceSquared(city c1,city c2){
	double x_dif=c1.x-c2.x;
	double y_dif=c1.y-c2.y;
	return x_dif*x_dif+y_dif*y_dif;
}

double calcDistance(city c1,city c2){
	return sqrt(calcDistanceSquared(c1,c2));
}