#ifndef __MEASURE__
#define __MEASURE__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>

typedef struct
{
	int value;
	SDL_Rect rect;
}
Measure;

typedef struct MeasureNode
{
	Measure value;
	struct MeasureNode *next;
}
MeasureNode;

typedef enum 
{
	TEMPERATURE,
	MOISTURE
}
MeasureType;

MeasureNode* addMeasure(MeasureNode* node, Measure value);
void displayList(MeasureNode* list);
void freeMeasures(MeasureNode* list);
void getMinMax(MeasureNode *list, int* min, int* max);
float getAverage(MeasureNode *list);
float getStandardDeviation(MeasureNode *list);
void generateMeasures(int measuresCount, MeasureNode **temperatures, MeasureNode **moistures);
MeasureNode* clearMeasures(MeasureNode* list);

#endif