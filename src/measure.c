#include "measure.h"

void generateMeasures(int measuresCount, MeasureNode **temperatures, MeasureNode **moistures)
{
	*temperatures = clearMeasures(*temperatures);
	*moistures = clearMeasures(*moistures);
	for (int i = 0; i < measuresCount; ++i)
	{
		Measure temperature;
		temperature.value = rand() % 40;
		SDL_Rect rect = { 0,0,0,0};
		temperature.rect = rect;
		*temperatures = addMeasure(*temperatures, temperature);

		Measure moisture;
		moisture.value = rand() % 100;
		moisture.rect = rect;
		*moistures = addMeasure(*moistures, moisture);
	}
}

MeasureNode* addMeasure(MeasureNode* node, Measure value)
{
	MeasureNode* newNode = malloc(sizeof(MeasureNode));
	newNode->next = node;
	newNode->value = value;
	return newNode;
}

void displayList(MeasureNode *list)
{
	if (list == NULL)
	{
		return;
	}

	printf("value: %d\n", list->value.value, list->value.rect);
	displayList(list->next);
}

void freeMeasures(MeasureNode *list)
{
	if (list == NULL)
	{
		return;
	}

	freeMeasures(list->next);
	free(list);
}

MeasureNode* clearMeasures(MeasureNode* list)
{
	freeMeasures(list);
	return NULL;
}

void getMinMax(MeasureNode *list, int* min, int* max)
{
	MeasureNode *iterator = list;
	*min = list->value.value;
	*max = list->value.value;
	while (iterator != NULL)
	{
		if (iterator->value.value > *max)
		{
			*max = iterator->value.value;
		}
		if (iterator->value.value < *min)
		{
			*min = iterator->value.value;
		}
		iterator = iterator->next;
	}
}

float getAverage(MeasureNode *list)
{
	MeasureNode *iterator = list;
	int sum = 0;
	int count = 0;
	while (iterator != NULL)
	{
		sum += iterator->value.value;
		count++;
		iterator = iterator->next;
	}

	return (float) sum / count;
}

float getStandardDeviation(MeasureNode *list)
{
	MeasureNode *iterator = list;
	float average = getAverage(list);
	int sum = 0;
	int count = 0;

	while (iterator != NULL)
	{
		sum = sum + pow((iterator->value.value - average), 2);
		count++;
		iterator = iterator->next;
	}

	if (count == 0)
		return 0;
	
	return sqrt(sum / count);
}