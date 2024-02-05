#include "renderer.h"

SDL_bool loop(AppState *state)
{
	SDL_Event event;
	SDL_bool running;

	while (SDL_PollEvent(&event))
	{
		running = HandleMainWindowEvent(state, event);
		HandleEditWindowEvent(state, event);
	}

	SetBackground(state->renderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
    DrawTitle(state);
    DrawGeneratorButton(state);
    DrawGeneratorCountInput(state);
    DrawTemperature(state);
    DrawMoistures(state);
    DrawTemperatureStats(state);
    DrawMoistureStats(state);
	SDL_RenderPresent(state->renderer);

	if (state->editWindowOpened)
	{
		SetBackground(state->editRenderer, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
	    DrawEditWindowTitle(state);
	    DrawEditValueInput(state);
	    DrawValidateButton(state);
		SDL_RenderPresent(state->editRenderer);
	}

	SDL_Delay(50);

	return running;
}

SDL_bool HandleMainWindowEvent(AppState *state, SDL_Event event)
{
	if (event.window.windowID != state->windowId)
		return SDL_TRUE;

	SDL_Point position;
	SDL_Rect generateButton = {50, 40, 140, 40};
	SDL_Rect countInputButton = {200, 40, 60, 40};

	switch (event.type)
	{	
		case SDL_QUIT:
			return SDL_FALSE;
		case SDL_MOUSEBUTTONDOWN:
			position = GetMousePosition();

			if (IsInRectangle(position, generateButton))
			{
				StopTextInput(state);
				state->measuresCount = atoi(state->countNumber);
				generateMeasures(state->measuresCount, &(state->temperatures), &(state->moistures));
			}
			else if (IsInRectangle(position, countInputButton))
			{
				StartTextInput(state, COUNT_NUMBER);
			}
			else
			{
				HandleClickOnValue(state, position);
			}
			break;
		case SDL_TEXTINPUT:
			HandleInputText(state, event.text.text);
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_BACKSPACE) {
				HandleSuppressChar(state);
			}
			break;
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_CLOSE)
			{
				return SDL_FALSE;
			}
		default:
			break;
	}

	return SDL_TRUE;
}

void HandleEditWindowEvent(AppState *state, SDL_Event event)
{
	if (event.window.windowID != state->editWindowId)
		return;

	SDL_Point position;
	SDL_Rect validateButton = {(EDIT_WINDOW_WIDTH - 140) / 2, 170, 140, 40};
	SDL_Rect editButton = {(EDIT_WINDOW_WIDTH - 60)/ 2, 100, 60, 40};

	switch (event.type)
	{
		case SDL_MOUSEBUTTONDOWN:
			position = GetMousePosition();

			if (IsInRectangle(position, validateButton))
			{
				int value = atoi(state->editNumber);
				if (IsValidMeasureValue(state->editingType, value))
				{
					SDL_HideWindow(state->editWindow);
					state->editWindowOpened = SDL_FALSE;
					free(state->editNumber);
					state->editNumber = NULL;
					state->editingNode->value.value = value;
					state->editingNode = NULL;
					StopTextInput(state);
				}
			}
			else if (IsInRectangle(position, editButton))
			{
				StartTextInput(state, EDITING_NODE);
			}
			break;
		case SDL_TEXTINPUT:
			HandleInputText(state, event.text.text);
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_BACKSPACE) {
				HandleSuppressChar(state);
			}
			break;
		case SDL_WINDOWEVENT:
			switch(event.window.event)
			{
				case SDL_WINDOWEVENT_CLOSE:
					SDL_HideWindow(state->editWindow);
					state->editWindowOpened = SDL_FALSE;
					free(state->editNumber);
					state->editNumber = NULL;
					state->editingNode = NULL;
					StopTextInput(state);
					break;
			}
			break;
		default:
			break;
	}
}

void Init(AppState *state)
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    	SDL_ExitWithError("Initialisation SDL");

    if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI, &(state->window), &(state->renderer)) != 0)
    	SDL_ExitWithError("Creation fenetre et rendu echouee");

    state->windowId = SDL_GetWindowID(state->window);

    if (SDL_CreateWindowAndRenderer(EDIT_WINDOW_WIDTH, EDIT_WINDOW_HEIGHT, SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI, &(state->editWindow), &(state->editRenderer)) != 0)
    	SDL_ExitWithError("Creation fenetre et rendu echouee");

    state->editWindowId = SDL_GetWindowID(state->editWindow);

    if (TTF_Init() == -1)
    {
        SDL_Log("TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    if ((state->font[0] = TTF_OpenFont("./Roboto-Regular.ttf", 24)) == NULL)
    {
    	SDL_Log("TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    if ((state->font[1] = TTF_OpenFont("./Roboto-Regular.ttf", 16)) == NULL)
    {
    	SDL_Log("TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    strcpy(state->countNumber, "10");

    state->measuresCount = 0;
    state->temperatures = NULL;
    state->moistures = NULL;
    state->editingNode = NULL;
    state->editNumber = NULL;
    state->editWindowOpened = SDL_FALSE;
    state->inputMode = NONE;
}

void SDL_ExitWithError(const char* message)
{
	SDL_Log("Erreur: %s > %s\n", message, SDL_GetError());
	SDL_Quit();
	exit(EXIT_FAILURE);
}

void SetBackground(SDL_Renderer *renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderClear(renderer);
}

void DrawTitle(AppState *state)
{
	SDL_Color Black = {0, 0, 0, SDL_ALPHA_OPAQUE};
    SDL_Surface* surfaceTitle = TTF_RenderText_Solid(state->font[0], "Gestion des champs", Black);
    SDL_Texture* title = SDL_CreateTextureFromSurface(state->renderer, surfaceTitle);

	SDL_Rect Message_rect;
	Message_rect.x = (WINDOW_WIDTH - surfaceTitle->w) / 2;
	Message_rect.y = 40;
	Message_rect.w = surfaceTitle->w;
	Message_rect.h = surfaceTitle->h;

	SDL_RenderCopy(state->renderer, title, NULL, &Message_rect);
}

void DrawEditWindowTitle(AppState *state)
{
	SDL_Color Black = {0, 0, 0, SDL_ALPHA_OPAQUE};
    SDL_Surface* surfaceTitle = NULL;

    if (state->editingType == TEMPERATURE)
    {
    	surfaceTitle = TTF_RenderText_Solid(state->font[0], "Temperature", Black);
    }
    else
    {
    	surfaceTitle = TTF_RenderText_Solid(state->font[0], "Humidite", Black);
    }

    SDL_Texture* title = SDL_CreateTextureFromSurface(state->editRenderer, surfaceTitle);

	SDL_Rect Message_rect;
	Message_rect.x = (EDIT_WINDOW_WIDTH - surfaceTitle->w) / 2;
	Message_rect.y = 40;
	Message_rect.w = surfaceTitle->w;
	Message_rect.h = surfaceTitle->h;

	SDL_RenderCopy(state->editRenderer, title, NULL, &Message_rect);
}

void DrawGeneratorButton(AppState *state)
{
	SDL_Rect button = {50, 40, 140, 40};
    SDL_SetRenderDrawColor(state->renderer, 0xcf, 0xcf, 0xcf, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(state->renderer, &button);

	SDL_Color Black = {0, 0, 0, SDL_ALPHA_OPAQUE};
    SDL_Surface* surfaceTitle = TTF_RenderText_Solid(state->font[0], "Generer", Black);
    SDL_Texture* title = SDL_CreateTextureFromSurface(state->renderer, surfaceTitle);

	SDL_Rect Message_rect;
	Message_rect.x = button.x + (button.w - surfaceTitle->w) / 2;
	Message_rect.y = button.y + (button.h - surfaceTitle->h) / 2;
	Message_rect.w = surfaceTitle->w;
	Message_rect.h = surfaceTitle->h;

	SDL_RenderCopy(state->renderer, title, NULL, &Message_rect);
}

void DrawGeneratorCountInput(AppState *state)
{
	SDL_Rect button = {200, 40, 60, 40};
	if (state->inputMode == COUNT_NUMBER)
	{
		SDL_SetRenderDrawColor(state->renderer, 0xaf, 0xaf, 0xaf, SDL_ALPHA_OPAQUE);
	}
	else
	{
		SDL_SetRenderDrawColor(state->renderer, 0xcf, 0xcf, 0xcf, SDL_ALPHA_OPAQUE);
	}
    
    SDL_RenderFillRect(state->renderer, &button);

	SDL_Color Black = {0, 0, 0, SDL_ALPHA_OPAQUE};
    SDL_Surface* surfaceTitle = TTF_RenderText_Solid(state->font[0], state->countNumber, Black);
    SDL_Texture* title = SDL_CreateTextureFromSurface(state->renderer, surfaceTitle);

	SDL_Rect Message_rect;
	Message_rect.x = button.x + button.w - surfaceTitle->w - 15;
	Message_rect.y = button.y + (button.h - surfaceTitle->h) / 2;
	Message_rect.w = surfaceTitle->w;
	Message_rect.h = surfaceTitle->h;

	SDL_RenderCopy(state->renderer, title, NULL, &Message_rect);
}

void DrawValidateButton(AppState *state)
{
	SDL_Rect button = {(EDIT_WINDOW_WIDTH - 140) / 2, 170, 140, 40};
    SDL_SetRenderDrawColor(state->editRenderer, 0xcf, 0xcf, 0xcf, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(state->editRenderer, &button);

	SDL_Color Black = {0, 0, 0, SDL_ALPHA_OPAQUE};
    SDL_Surface* surfaceTitle = TTF_RenderText_Solid(state->font[0], "Valider", Black);
    SDL_Texture* title = SDL_CreateTextureFromSurface(state->editRenderer, surfaceTitle);

	SDL_Rect Message_rect;
	Message_rect.x = button.x + (button.w - surfaceTitle->w) / 2;
	Message_rect.y = button.y + (button.h - surfaceTitle->h) / 2;
	Message_rect.w = surfaceTitle->w;
	Message_rect.h = surfaceTitle->h;

	SDL_RenderCopy(state->editRenderer, title, NULL, &Message_rect);
}

void DrawEditValueInput(AppState *state)
{
	if (state->editingNode == NULL)
		return;

	SDL_Rect button = {(EDIT_WINDOW_WIDTH - 60)/ 2, 100, 60, 40};
	if (state->inputMode == EDITING_NODE)
	{
		int value = atoi(state->editNumber);
		if (IsValidMeasureValue(state->editingType, value))
		{
			SDL_SetRenderDrawColor(state->editRenderer, 0xaf, 0xaf, 0xaf, SDL_ALPHA_OPAQUE);
		}
		else
		{
			SDL_SetRenderDrawColor(state->editRenderer, 0xff, 0xaf, 0xaf, SDL_ALPHA_OPAQUE);
		}
	}
	else
	{
		SDL_SetRenderDrawColor(state->editRenderer, 0xcf, 0xcf, 0xcf, SDL_ALPHA_OPAQUE);
	}
    
    SDL_RenderFillRect(state->editRenderer, &button);

    if (state->editNumber == NULL)
    {
    	state->editNumber = malloc(sizeof(char) * 3);
    	sprintf(state->editNumber, "%d", state->editingNode->value.value);
    }

	SDL_Color Black = {0, 0, 0, SDL_ALPHA_OPAQUE};
    SDL_Surface* surfaceTitle = TTF_RenderText_Solid(state->font[0], state->editNumber, Black);
    SDL_Texture* title = SDL_CreateTextureFromSurface(state->editRenderer, surfaceTitle);

	SDL_Rect Message_rect;
	Message_rect.x = button.x + button.w - surfaceTitle->w - 15;
	Message_rect.y = button.y + (button.h - surfaceTitle->h) / 2;
	Message_rect.w = surfaceTitle->w;
	Message_rect.h = surfaceTitle->h;

	SDL_RenderCopy(state->editRenderer, title, NULL, &Message_rect);
}

SDL_Point GetMousePosition()
{
	SDL_Point position;
	SDL_GetMouseState(&(position.x), &(position.y));
	return position;
}

SDL_bool IsInRectangle(SDL_Point position, SDL_Rect rect)
{
	return position.x >= rect.x && position.x <= rect.x + rect.w && position.y >= rect.y && position.y <= rect.y + rect.h;
}

void HandleInputText(AppState *state, char* input)
{
	char *text = NULL;

	if (state->inputMode == COUNT_NUMBER)
	{
		text = state->countNumber;
	}
	else
	{
		text = state->editNumber;
	}

	if (text == NULL)
		return;

	int len = strlen(text);
	int inputLen = strlen(input);
	
	if (len + inputLen > 2)
		return;

	if (text[0] == ' ')
		len = 0;

	for (int i = 0; i < inputLen; ++i)
	{
		if (isdigit(input[i]))
		{
			text[len] = input[i];
		}
	}
}

void HandleSuppressChar(AppState *state)
{
	char *text = NULL;

	if (state->inputMode == COUNT_NUMBER)
	{
		text = state->countNumber;
	}
	else
	{
		text = state->editNumber;
	}

	if (text == NULL)
		return;

	int len = strlen(text);
	
	if (len == 0)
		return;

	if (len == 1)
	{
		text[len - 1] = ' '; 
		return;
	}	

	text[len - 1] = '\0'; 
}

void DrawTemperature(AppState *state)
{
	// draw section
	SDL_Rect section = {50, 100, WINDOW_WIDTH - 100, 250};
    SDL_SetRenderDrawColor(state->renderer, 0xcf, 0xcf, 0xcf, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(state->renderer, &section);

    // draw section title
	SDL_Color Black = {0, 0, 0, SDL_ALPHA_OPAQUE};
    SDL_Surface* surfaceTitle = TTF_RenderText_Solid(state->font[0], "Temperature (°C)", Black);
    SDL_Texture* title = SDL_CreateTextureFromSurface(state->renderer, surfaceTitle);

	SDL_Rect Message_rect;
	Message_rect.x = section.x + (section.w - surfaceTitle->w) / 2;
	Message_rect.y = section.y + 5;
	Message_rect.w = surfaceTitle->w;
	Message_rect.h = surfaceTitle->h;

	SDL_RenderCopy(state->renderer, title, NULL, &Message_rect);

	int spacing = 0;

	if (state->measuresCount != 0)
	{
		spacing = (float) (section.w - 50) / (state->measuresCount + 1);
	}

	SDL_Point mouse = GetMousePosition();

	// draw measures
	MeasureNode *iterator = state->temperatures;
	int i = 0;
	while (iterator != NULL)
	{
		int height = 200 * ((float) iterator->value.value / 40);
		SDL_Rect bar = {section.x + 50 + (i + 1) * spacing, section.y + section.h - 10 - height, 10, height};
		if (IsInRectangle(mouse, bar))
		{
			SDL_SetRenderDrawColor(state->renderer, 0x00, 0xff, 0xcc, SDL_ALPHA_OPAQUE);
		}
		else
		{
			SDL_SetRenderDrawColor(state->renderer, 0x00, 0x22, 0xcc, SDL_ALPHA_OPAQUE);
		}
		iterator->value.rect = bar;
	    SDL_RenderFillRect(state->renderer, &bar);
	    iterator = iterator->next;
	    i++;
	}

	SDL_Point position = { section.x + 10, section.y + section.h - 28};
	DrawText(state, "0 -", Black, position, 1, SDL_FALSE);

	position.y = section.y + section.h - 128;
	DrawText(state, "20 -", Black, position, 1, SDL_FALSE);

	position.y = section.y + section.h - 228;
	DrawText(state, "40 -", Black, position, 1, SDL_FALSE);
}

void DrawMoistures(AppState *state)
{
	// draw section
	SDL_Rect section = {50, 430, WINDOW_WIDTH - 100, 250};
    SDL_SetRenderDrawColor(state->renderer, 0xcf, 0xcf, 0xcf, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(state->renderer, &section);

    // draw section title
	SDL_Color Black = {0, 0, 0, SDL_ALPHA_OPAQUE};
    SDL_Surface* surfaceTitle = TTF_RenderText_Solid(state->font[0], "Humidite (%)", Black);
    SDL_Texture* title = SDL_CreateTextureFromSurface(state->renderer, surfaceTitle);

	SDL_Rect Message_rect;
	Message_rect.x = section.x + (section.w - surfaceTitle->w) / 2;
	Message_rect.y = section.y + 5;
	Message_rect.w = surfaceTitle->w;
	Message_rect.h = surfaceTitle->h;

	SDL_RenderCopy(state->renderer, title, NULL, &Message_rect);

	int spacing = 0;

	if (state->measuresCount != 0)
	{
		spacing =  ((float)section.w - 50) / (state->measuresCount + 1);
	}
	
	SDL_SetRenderDrawColor(state->renderer, 0x00, 0x22, 0xcc, SDL_ALPHA_OPAQUE);

	MeasureNode *iterator = state->moistures;
	int i = 0;
	// draw measures
	while (iterator != NULL && iterator->next != NULL)
	{
		int height1 = 200 * ((float) iterator->value.value / 100);
		int height2 = 200 * ((float) iterator->next->value.value / 100);
		SDL_RenderDrawLine(state->renderer, section.x + 50 + (i + 1) * spacing, 
											section.y + section.h - 10 - height1, 
											section.x + 50 + (i + 2) * spacing, 
											section.y + section.h - 10 - height2);
		iterator = iterator->next;
		i++;
	}

	iterator = state->moistures;
	i = 0;
	SDL_Point mouse = GetMousePosition();
	while (iterator != NULL)
	{
		int height = 200 * ((float) iterator->value.value / 100);
		SDL_Rect bar = {section.x + 50 + (i + 1) * spacing - 5, section.y + section.h - 10 - height - 5, 10, 10};
		if (IsInRectangle(mouse, bar))
		{
			SDL_SetRenderDrawColor(state->renderer, 0x00, 0xff, 0xcc, SDL_ALPHA_OPAQUE);
		}
		else
		{
			SDL_SetRenderDrawColor(state->renderer, 0x00, 0x22, 0xcc, SDL_ALPHA_OPAQUE);
		}
		iterator->value.rect = bar;
	    SDL_RenderFillRect(state->renderer, &bar);
		iterator = iterator->next;
		i++;
	}

	SDL_Point position = { section.x + 10, section.y + section.h - 28};
	DrawText(state, "0 -", Black, position, 1, SDL_FALSE);

	position.y = section.y + section.h - 128;
	DrawText(state, "50 -", Black, position, 1, SDL_FALSE);

	position.y = section.y + section.h - 228;
	DrawText(state, "100 -", Black, position, 1, SDL_FALSE);

}

void DrawText(AppState *state, char* text, SDL_Color color, SDL_Point position, int fontIndex, SDL_bool centered)
{
    SDL_Surface* surfaceText = TTF_RenderText_Solid(state->font[fontIndex], text, color);
    SDL_Texture* textureText = SDL_CreateTextureFromSurface(state->renderer, surfaceText);

	SDL_Rect text_rect;
	text_rect.x = centered ? position.x - surfaceText->w : position.x;
	text_rect.y = centered ? position.y - surfaceText->h : position.y;
	text_rect.w = surfaceText->w;
	text_rect.h = surfaceText->h;

	SDL_RenderCopy(state->renderer, textureText, NULL, &text_rect);
}

void HandleClickOnValue(AppState *state, SDL_Point position)
{
	MeasureNode* iterator = state->temperatures;
	while (iterator != NULL)
	{
		if(IsInRectangle(position, iterator->value.rect))
		{
			state->editingNode = iterator;
			state->editingType = TEMPERATURE;
			state->editWindowOpened = SDL_TRUE;
			SDL_ShowWindow(state->editWindow);
			SDL_RaiseWindow(state->editWindow);
    		return;
		}
		iterator = iterator->next;
	}

	iterator = state->moistures;
	while (iterator != NULL)
	{
		if(IsInRectangle(position, iterator->value.rect))
		{
			state->editingNode = iterator;
			state->editingType = MOISTURE;
			state->editWindowOpened = SDL_TRUE;
			SDL_ShowWindow(state->editWindow);
			SDL_RaiseWindow(state->editWindow);
    		return;
		}
		iterator = iterator->next;
	}
}

void DrawTemperatureStats(AppState *state)
{
	if (state->measuresCount == 0)
		return;
	
	int spacing = WINDOW_WIDTH / 5;
	int min, max;
	char buffer[256];
	getMinMax(state->temperatures, &min, &max);
	float average = getAverage(state->temperatures);
	float standardDeviation = getStandardDeviation(state->temperatures);

	sprintf(buffer, "Min: %d", min);
	SDL_Color color = {0x00,0x00,0x00, SDL_ALPHA_OPAQUE};
	SDL_Point position = { spacing, 375};
	DrawText(state, buffer, color, position, 1, SDL_TRUE);

	sprintf(buffer, "Max: %d", max);
	position.x = spacing * 2;
	DrawText(state, buffer, color, position, 1, SDL_TRUE);

	sprintf(buffer, "Moyenne: %.2f", average);
	position.x = spacing * 3;
	DrawText(state, buffer, color, position, 1, SDL_TRUE);

	sprintf(buffer, "Ecart-type: %.2f", standardDeviation);
	position.x = spacing * 4;
	DrawText(state, buffer, color, position, 1, SDL_TRUE);
}

void DrawMoistureStats(AppState *state)
{
	if (state->measuresCount == 0)
		return;
	
	int spacing = WINDOW_WIDTH / 5;
	int min, max;
	char buffer[256];
	getMinMax(state->moistures, &min, &max);
	float average = getAverage(state->moistures);
	float standardDeviation = getStandardDeviation(state->moistures);

	sprintf(buffer, "Min: %d", min);
	SDL_Color color = {0x00,0x00,0x00, SDL_ALPHA_OPAQUE};
	SDL_Point position = { spacing, 705};
	DrawText(state, buffer, color, position, 1, SDL_TRUE);

	sprintf(buffer, "Max: %d", max);
	position.x = spacing * 2;
	DrawText(state, buffer, color, position, 1, SDL_TRUE);

	sprintf(buffer, "Moyenne: %.2f", average);
	position.x = spacing * 3;
	DrawText(state, buffer, color, position, 1, SDL_TRUE);

	sprintf(buffer, "Ecart-type: %.2f", standardDeviation);
	position.x = spacing * 4;
	DrawText(state, buffer, color, position, 1, SDL_TRUE);
}

void StopTextInput(AppState *state)
{
	SDL_StopTextInput();
	state->inputMode = NONE;
}

void StartTextInput(AppState *state, InputMode inputMode)
{
	SDL_StartTextInput();
	state->inputMode = inputMode;
}

SDL_bool IsValidMeasureValue(MeasureType type, int value)
{
	if (type == TEMPERATURE)
	{
		return value >= 0 && value <= 40;
	}
	return value >= 0 && value <= 100;
}

void Destroy(AppState *state)
{
	freeMeasures(state->temperatures);
	freeMeasures(state->moistures);
	SDL_DestroyRenderer(state->renderer);
    SDL_DestroyWindow(state->window);
    SDL_DestroyRenderer(state->editRenderer);
    SDL_DestroyWindow(state->editWindow);
    for (int i = 0; i < 2; ++i)
    {
    	TTF_CloseFont(state->font[i]);
    }
    if (state->editNumber != NULL)
    {
    	free(state->editNumber);
    }
	SDL_Quit();
}