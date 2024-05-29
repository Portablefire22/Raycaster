#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const int SCREEN_HEIGHT = 1080;
const int SCREEN_WIDTH = 1920;

const uint8_t map[6][8] = {
    {1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,0,1},
    {1,0,0,0,0,1,0,1},
    {1,0,0,1,0,0,0,1},
    {1,1,1,1,1,1,1,1}
};

const int MAP_WIDTH = (int) (sizeof(map[0])/sizeof(map[0][0]));
const int MAP_HEIGHT = (int) (sizeof(map)/sizeof(map[0]));

const int MINIMAP_BLOCK_SIZE = 50 * (SCREEN_WIDTH / SCREEN_HEIGHT);

typedef struct EntityStruct {
    float px; // Position X
    float py; // Position Y
    float pr; // Rotation
    int msize; // Mini-map Size
    Uint32 col; // Entity colour
} entity;

struct {
    SDL_Window* window;
    SDL_Surface* screen_surface;
    Uint32* buffer;
    entity** entity_list;
    entity* current_player;
    uint8_t entity_list_length;
    int quit;
} engine;

int init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not be initialised! SDL_ERROR: %s\n", SDL_GetError());
        return -1;
    }

    engine.window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (engine.window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    } 

    engine.screen_surface = SDL_GetWindowSurface(engine.window);
    return 0;
}

void draw_minimap() {
    int cx = 0;
    int cy = 0;

    for (int i=0; i < (int)(sizeof(map)/sizeof(map[0][0])); i++) {
        if (map[cy / MINIMAP_BLOCK_SIZE][cx / MINIMAP_BLOCK_SIZE]) {
            SDL_Rect* block = (SDL_Rect*)malloc(sizeof(SDL_Rect));
            block->h = block->w = MINIMAP_BLOCK_SIZE;
            block->x = cx;
            block->y = cy;
            SDL_FillRect(engine.screen_surface, block, i);
            free(block);
        }

        cx += MINIMAP_BLOCK_SIZE;
        if (cx / MINIMAP_BLOCK_SIZE >= MAP_WIDTH) {
            cx = 0;
            cy += MINIMAP_BLOCK_SIZE;
        }
    }
    
    for (int i=0; i < engine.entity_list_length; i++) {
        SDL_Rect* box = (SDL_Rect*)malloc(sizeof(SDL_Rect));
        entity* ce = engine.entity_list[i];
        box->w = box->h = ce->msize;
        box->x = ce->px;
        box->y = ce->py;
        SDL_FillRect(engine.screen_surface, box, ce->col);
        free(box);
    }
}

void add_entity(entity* ent) {
    engine.entity_list = (entity**) realloc(engine.entity_list, (engine.entity_list_length + 1) * sizeof(entity*));
    engine.entity_list[engine.entity_list_length] = ent;
    engine.entity_list_length++;
}

void free_entities() {
    int i = 0;
    while (i != engine.entity_list_length) {
        free(engine.entity_list[i]);
        i++;
    }
}

int main(int argc, char* args[]) {
    
    if (init() == -1) {
        goto EXIT;
    }
    engine.entity_list = NULL;
    engine.entity_list_length = 0;
    
    
    entity* player = (entity*)malloc(sizeof(entity));
    player->msize = 10;
    player->col = 0xff00ff;
    player->pr = player->px = player->py = 0.0;
    add_entity(player);
    
    entity* p2 = (entity*)malloc(sizeof(entity));
    p2->msize = 20;
    p2->col = 0xffff00;
    p2->pr = p2->px = p2->py = 40.0;
    add_entity(p2);

    engine.current_player = player;
    
    printf("%d, %d\n",engine.entity_list_length, engine.entity_list[0]->msize);
    
    engine.buffer = (Uint32*) engine.screen_surface->pixels;

    SDL_FillRect(engine.screen_surface, NULL, SDL_MapRGB(engine.screen_surface->format, 0xff, 0xff, 0xff));
    SDL_UpdateWindowSurface(engine.window);


    SDL_Event e; 
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_LEFT:
                            break;
                        case SDLK_RIGHT:
                            break;
                        case SDLK_UP:
                            break;
                        case SDLK_DOWN:
                            break;
                    }
            }
        }
        draw_minimap();
        SDL_UpdateWindowSurface(engine.window);
    }
        
    EXIT:
    free_entities();
    SDL_FreeSurface(engine.screen_surface);
    SDL_DestroyWindow(engine.window);
    SDL_Quit();

    return 0;
}
