#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_HEIGHT 1080
#define SCREEN_WIDTH 1920

#define PI 3145

const uint8_t map[6][8] = {
    {1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1},
    {1,0,0,1,1,1,0,1},
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
    int pr; // Rotation
    int msize; // Mini-map Size
    Uint32 col; // Entity colour
} entity;

struct RayHit {
    float distance;
    int wall_height;
};

struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    Uint32 pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

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

    engine.renderer = SDL_CreateRenderer(engine.window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (engine.renderer == NULL) {
        return -1;
    }

    SDL_SetRenderDrawColor(engine.renderer, 0xff, 0xff, 0xff, 0xff);
    
    engine.texture = SDL_CreateTexture(engine.renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (engine.texture == NULL) {
        return -1;
    }

    return 0;
}

void draw_minimap() {
    int cx = 0;
    int cy = 0;
    SDL_SetRenderDrawColor(engine.renderer, 0x0A, 0x0A, 0x0A, 0xff);
    for (int i=0; i < (int)(sizeof(map)/sizeof(map[0][0])); i++) {
        if (map[cy / MINIMAP_BLOCK_SIZE][cx / MINIMAP_BLOCK_SIZE]) {
            SDL_Rect* block = (SDL_Rect*)malloc(sizeof(SDL_Rect));
            block->h = block->w = MINIMAP_BLOCK_SIZE;
            block->x = cx;
            block->y = cy;
            SDL_RenderFillRect(engine.renderer, block);
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
        SDL_SetRenderDrawColor(engine.renderer, (ce->col >> 16) & 0xff, (ce->col >> 8) & 0xff, ce->col & 0xff, 0xff);
        SDL_RenderFillRect(engine.renderer, box);
        SDL_SetRenderDrawColor(engine.renderer, 0x00, 0xf0, 0xf0, 0xff);
        int ty = (float)(ce->py + ce->msize / 2) + sin(ce->pr / 1000.0) * 50;
        int tx = (float)(ce->px + ce->msize / 2) + cos(ce->pr / 1000.0) * 50;
        SDL_RenderDrawLine(engine.renderer, ce->px + ce->msize / 2 , ce->py + ce->msize / 2, tx, ty);
        free(box);
    }
    SDL_SetRenderDrawColor(engine.renderer, 0xff, 0xff, 0xff, 0xff);
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

/*struct RayHit cast_ray(float ray_angle) {
    float px = engine.current_player->px;
    float py = engine.current_player->py;
    
    float dx = cos(ray_angle);
    float dy = sin(ray_angle);

    int i = 0;
    printf("%f\n", engine.current_player->px);
    while (map[(int) floor(py)][(int) floor(px)] == 0) {
        px += dx * 0.1;
        py += dy * 0.1;
        i++;
        if (i > 400) break; 
    }
    printf("d%f\n", px);

    const float distance = sqrt(pow((px - engine.current_player->px), 2.0) + pow((py - engine.current_player->py), 2.0));
    const int wall_height = (int) floor(300.0 / distance);
    struct RayHit info;
    info.distance = distance;
    info.wall_height = wall_height;
    return info;
}

void draw_wall_slice(int i, struct RayHit* info, int slice_width) {
    for (int j=0; j < info->wall_height; j++) {
        int yPos = floor(300 - info->wall_height / 2 + j);

        SDL_Rect* temp_rect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
        temp_rect->x = i * slice_width;
        temp_rect->y = yPos;
        temp_rect->w = slice_width;
        temp_rect->h = 1;
        SDL_FillRect(engine.screen_surface, temp_rect, 0xff00ff);

        free(temp_rect);
    }
}

void raycast() {
    const int rays = 200;
    const int slice_width = (int) (SCREEN_WIDTH / rays);
    const int angle_step = 60 / rays;

    // Walls
    for (int i=0; i < rays; i++) {
        const float ray_angle = engine.current_player->pr - (60/2) + i * angle_step;
        struct RayHit ray_info = cast_ray(ray_angle);
        draw_wall_slice(i, &ray_info, slice_width);
    }
}*/

int main(int argc, char* args[]) {
    
    if (init() == -1) {
        goto EXIT;
    }
    engine.entity_list = NULL;
    engine.entity_list_length = 0;
    
    
    entity* player = (entity*)malloc(sizeof(entity));
    player->msize = 10;
    player->col = 0xff00ff;
    player->px = player->py = 3.0;
    player->pr = PI;
    add_entity(player);
    

    engine.current_player = player;
    
    SDL_Event e; 
    int quit = 0;
    int i = 0;
    int iter = 1;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_RIGHT:
                            engine.current_player->pr = engine.current_player->pr + (int) floorf(0.1 * PI);
                            break;
                        case SDLK_LEFT:
                            engine.current_player->pr = engine.current_player->pr - (int) floorf(0.1 * PI);
                            break;
                        case SDLK_UP:
                            engine.current_player->py += sin(engine.current_player->pr / 1000.0) * 5;
                            engine.current_player->px += cos(engine.current_player->pr / 1000.0) * 5;
                            break;
                        case SDLK_DOWN:
                            engine.current_player->py -= sin(engine.current_player->pr / 1000.0) * 5;
                            engine.current_player->px -= cos(engine.current_player->pr / 1000.0) * 5;
                            break;
                    }
                    break;
            }
        }
        SDL_RenderClear(engine.renderer);
        
        SDL_RenderCopy(engine.renderer, engine.texture, NULL, NULL); // Render texture
        draw_minimap();
        SDL_RenderPresent(engine.renderer);
    }
        
    EXIT:
    free_entities();
    SDL_DestroyTexture(engine.texture);
    SDL_DestroyRenderer(engine.renderer);
    SDL_DestroyWindow(engine.window);
    engine.window = NULL;
    engine.renderer = NULL;
    SDL_Quit();

    return 0;
}
