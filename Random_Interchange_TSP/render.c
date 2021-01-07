#include "render.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

// The window we will be using
SDL_Window *gWin = NULL;
// The renderer we will be using
SDL_Renderer *gRend = NULL;

// Font for the main body
TTF_Font *gFont = NULL;
// Font for title
TTF_Font *gTitleFont = NULL;

void init()
{
    SDL_Init(SDL_INIT_VIDEO);
    gWin = SDL_CreateWindow("Random Interchange TSP", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    gRend = SDL_CreateRenderer(gWin, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(gRend, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(gRend);
    TTF_Init();
    gFont = TTF_OpenFont("../dependencies/fonts/FiraSans-Regular.ttf", 18);
    gTitleFont = TTF_OpenFont("../dependencies/fonts/FiraSans-Regular.ttf", 23);
    loadNewText(&gTitle, "Random Interchange TSP Visualization", 1);
    loadNewText(&gCredits, "Created by: George Vlassis & Manos Vlassis", 0);
    loadNewText(&gIterationText, "Iteration 0: ", 0);
}

void buildCity(int x, int y)
{
    SDL_SetRenderDrawColor(gRend, 0xFF, 0xA0, 0x00, 0xFF);
    SDL_Rect rec = {x, y+TOP_PADDING, CITY_SIZE, CITY_SIZE};
    SDL_RenderFillRect(gRend, &rec);
}

void buildCityGrid(city cities[], int num)
{
    for (int i = 0; i < num; i++)
    {
        buildCity(CITY_SIZE*cities[i].x, CITY_SIZE*cities[i].y);
    }
    renderText(gCredits, SCREEN_WIDTH-gCredits.width, SCREEN_HEIGHT-gCredits.height);
    renderText(gTitle, (SCREEN_WIDTH-gTitle.width), 0);
    renderText(gIterationText, 2, 2);
    SDL_RenderPresent(gRend);
}

void connectCities(city cities[], int path[][NUM_OF_CITIES])
{
    SDL_SetRenderDrawColor(gRend, 0x90, 0xCA, 0xF9, 0xFF);
    for (int i = 0; i < NUM_OF_CITIES-1; i++)
    {
        SDL_RenderDrawLine(gRend, (CITY_SIZE*cities[path[0][i]].x)+(CITY_SIZE/2), (CITY_SIZE*cities[path[0][i]].y)+(CITY_SIZE/2)+TOP_PADDING,
        (CITY_SIZE*cities[path[0][i+1]].x)+(CITY_SIZE/2), (CITY_SIZE*cities[path[0][i+1]].y)+(CITY_SIZE/2)+TOP_PADDING);
        SDL_RenderPresent(gRend);
    }
}

void clearWindow()
{
    SDL_SetRenderDrawColor(gRend, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(gRend);
}

void shutDown()
{
    SDL_DestroyRenderer(gRend);
    gRend = NULL;
    SDL_DestroyWindow(gWin);
    gWin = NULL;
    TTF_Quit();
    SDL_Quit();
}

void loadNewText(text *t, const char* str, int isTitleFont)
{
    if (t->pTexture == NULL) 
    {
        SDL_DestroyTexture(t->pTexture);
    }
    SDL_Color textColor = {0xFF, 0xFF, 0xFF};
    SDL_Surface *loadedSurface = NULL;
    if (!isTitleFont)
    {
        loadedSurface = TTF_RenderText_Blended_Wrapped(gFont, str, textColor, SCREEN_WIDTH);  
    }
    else
    {
        loadedSurface = TTF_RenderText_Blended_Wrapped(gTitleFont, str, textColor, SCREEN_WIDTH/2);  
    }
    if (loadedSurface == NULL)
    {
        printf("Could not load surface!");
    }
    else
    {        
        t->pTexture = SDL_CreateTextureFromSurface(gRend, loadedSurface);
        t->width = loadedSurface->w;
        t->height = loadedSurface->h;
    }
    SDL_FreeSurface(loadedSurface);
}

void renderText(text t, int x, int y)
{
    if (t.pTexture == NULL)
    {
        printf("Could not render text!");
    }
    else
    {      
        SDL_Rect destinationRect = {x, y, t.width, t.height};
        SDL_RenderCopy(gRend, t.pTexture, NULL, &destinationRect);
    }
}

void updateIterationText(char *message)
{
    loadNewText(&gIterationText, message, 0);
    return;
}