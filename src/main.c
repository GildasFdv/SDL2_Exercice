/*
    GNU/Linux et MacOS
        > gcc src/*.c -o bin/prog $(sdl2-config --cflags --libs)
    Windows
        > gcc src/*.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
*/
#include "renderer.h"

int main(int argc, char** argv)
{
    srand(time(NULL));
    AppState state;

    Init(&state);

    while(loop(&state));
    
    Destroy(&state);
    return 0;
}

