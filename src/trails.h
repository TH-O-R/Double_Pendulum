#ifndef TRAILS_H
#define TRAILS_H


#include <SDL2/SDL.h>


typedef struct {
    int max_points;
    int count;
    int head;
    float *xs; // size max_points
    float *ys; // size max_points
} Trail;


Trail *trail_create(int max_points);
void trail_free(Trail *t);
void trail_push(Trail *t, float x, float y);
void trail_render(SDL_Renderer *renderer, Trail *t, SDL_Color color, float width);
void trail_clear(Trail *t);


#endif // TRAILS_H
