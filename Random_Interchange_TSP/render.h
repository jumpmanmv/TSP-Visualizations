#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


// These parameters can be changed, but keep in mind that the window's dimensions are X_MAX*CITY_SIZE and Y_MAX*CITY_SIZE(+padding),
// so if you make them too big the window won't fit in the screen and you'll have to press ESC to exit.
#define NUM_OF_CITIES 1000
#define X_MAX 300
#define Y_MAX 300
#define CITY_SIZE 3

// To fit the title/iteration text/credits in the window
#define TOP_PADDING 24
#define BOTTOM_PADDING 22

#define SCREEN_WIDTH CITY_SIZE*X_MAX
#define SCREEN_HEIGHT CITY_SIZE*Y_MAX+TOP_PADDING+BOTTOM_PADDING

typedef struct city
{
	double x;
	double y;
} city;

// Basic struct to hold necessary data for rendering text
typedef struct text
{
    SDL_Texture *pTexture;
    int positionX;
    int positionY;
    int width;
    int height;
} text;

text gTitle;
text gCredits;
text gIterationText;

// Initializes SDL and creates renderer
void init();

// Renders a city of CITY_SIZE at these coordinates
void buildCity(int x, int y);

// Clears window and renders a new city grid
void buildCityGrid(city cities[], int num);

// Connects cities using the path found by the best thread, in this case we use thread 0
void connectCities(city cities[], int path[][NUM_OF_CITIES]);

// Clears window
void clearWindow();

// Frees memory and shuts down SDL
void shutDown();

// Renders text struct in these coordinates
void renderText(text t, int x, int y);

// Loads new text, isTitleFont = 1 for bigger font, 0 otherwise
void loadNewText(text *t, const char* str, int isTitleFont);

// Updates text for current iteration
void updateIterationText(char* message);