#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>

const int SCREEN_HEIGHT = 1080;
const int SCREEN_WIDTH = 1920;

struct {
    SDL_Window* window;
    SDL_Surface* screen_surface;
    int quit;
} engine;

const uint8_t map[48] = {
    1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,
};

int main(int argc, char* args[]) {
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialised! SDL_ERROR: %s\n", SDL_GetError());
        return -1;
    }

    engine.window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (engine.window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        goto EXIT;
    } 

    engine.screen_surface = SDL_GetWindowSurface(engine.window);

    SDL_FillRect(engine.screen_surface, NULL, SDL_MapRGB(engine.screen_surface->format, 0xff, 0x00, 0xff));
    SDL_UpdateWindowSurface(engine.window);

    SDL_Event e; 
    int quit = 0;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }
        SDL_UpdateWindowSurface(engine.window);
    }
        
    EXIT:
    
    SDL_DestroyWindow(engine.window);
    
    SDL_Quit();

    return 0;
}
