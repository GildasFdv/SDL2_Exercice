#ifndef __RENDERER___
#define __RENDERER___

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "measure.h"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800
#define EDIT_WINDOW_WIDTH 500
#define EDIT_WINDOW_HEIGHT 280

typedef enum 
{
	NONE,
	COUNT_NUMBER,
	EDITING_NODE
}
InputMode;

typedef struct
{
	SDL_Window *window;
	int windowId;
	SDL_Renderer *renderer;
	SDL_Window *editWindow;
	int editWindowId;
	SDL_bool editWindowOpened;
	SDL_Renderer *editRenderer;
	TTF_Font *font[2];
	char countNumber[3];
	MeasureNode *temperatures;
	MeasureNode *moistures;
	int measuresCount;
	MeasureNode* editingNode;
	MeasureType editingType;
	char *editNumber;
	InputMode inputMode;
}
AppState;

SDL_bool loop(AppState *state);
void SDL_ExitWithError(const char* message);
void Init(AppState *state);
void SetBackground(SDL_Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
void DrawTitle(AppState *state);
void DrawGeneratorButton(AppState *state);
void DrawGeneratorCountInput(AppState *state);
SDL_Point GetMousePosition();
SDL_bool IsInRectangle(SDL_Point position, SDL_Rect rect);
void HandleInputText(AppState *state, char* input);
void HandleSuppressChar(AppState *state);
void DrawTemperature(AppState * state);
void DrawMoistures(AppState *state);
void DrawText(AppState *state, char* text, SDL_Color color, SDL_Point position, int fontIndex, SDL_bool centered);
void HandleClickOnValue(AppState *state, SDL_Point position);
void DrawTemperatureStats(AppState *state);
void DrawMoistureStats(AppState *state);
void HandleClickOnValue(AppState *state, SDL_Point position);
SDL_bool HandleMainWindowEvent(AppState *state, SDL_Event event);
void HandleEditWindowEvent(AppState *state, SDL_Event event);
void DrawEditWindowTitle(AppState *state);
void DrawEditValueInput(AppState *state);
void StopTextInput(AppState *state);
void StartTextInput(AppState *state, InputMode inputMode);
void DrawValidateButton(AppState *state);
SDL_bool IsValidMeasureValue(MeasureType type, int value);
void Destroy(AppState *state);

#endif