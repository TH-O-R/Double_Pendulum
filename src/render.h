#ifndef RENDER_H
#define RENDER_H


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "physics.h"
#include "trails.h"


typedef struct {
    SDL_Window *win;
    SDL_Renderer *ren;
    TTF_Font *font;
    int width, height;
} AppRender;


int render_init(AppRender *r, int w, int h);
void render_destroy(AppRender *r);
void render_frame(AppRender *r, Pendulum *p, Trail *trail, double t, int show_grid, int show_trail, int show_hud);


#endif // RENDER_H
