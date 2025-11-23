#include "trails.h"
#include <stdlib.h>
#include <math.h>


Trail *trail_create(int max_points) {
    if (max_points <= 0) return NULL;
        
    Trail *t = (Trail*)malloc(sizeof(Trail));
    if (!t) return NULL;
    
    t->xs = (float*)malloc(sizeof(float) * max_points);
    if (!t->xs) {
        free(t);
        return NULL;
    }
    
    t->ys = (float*)malloc(sizeof(float) * max_points);
    if (!t->ys) {
        free(t->xs);
        free(t);
        return NULL;
    }
    t->max_points = max_points;
    t->count = 0;
    t->head = 0;
       
    return t;
}


void trail_free(Trail *t) {
    if (!t) return;
    free(t->xs); free(t->ys); free(t);
}


void trail_push(Trail *t, float x, float y) {
    if (!t || !t->xs || !t->ys) return;
    t->xs[t->head] = x;
    t->ys[t->head] = y;
    t->head = (t->head + 1) % t->max_points;
    if (t->count < t->max_points) t->count++;
}


void trail_clear(Trail *t) {
    if (!t) return;
    t->count = 0; t->head = 0;
}


void trail_render(SDL_Renderer *renderer, Trail *t, SDL_Color color, float width) {
    if (!t || !t->xs || !t->ys || t->count < 2) return;
    
    // Save current blend mode
    SDL_BlendMode prev_blend;
    SDL_GetRenderDrawBlendMode(renderer, &prev_blend);
    
    // Additive blending for neon glow
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
    
    int idx = (t->head - t->count + t->max_points) % t->max_points;
    for (int i = 0; i < t->count - 1; ++i) {
        int j = (idx + i) % t->max_points;
        int k = (idx + i + 1) % t->max_points;
        float alpha = (float)i / (float)t->count;
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, (Uint8)(alpha * 255));
        SDL_RenderDrawLine(renderer, (int)floor(t->xs[j]), (int)floor(t->ys[j]), 
                                     (int)floor(t->xs[k]), (int)floor(t->ys[k]));
    }
    
    // Restore previous blend mode
    SDL_SetRenderDrawBlendMode(renderer, prev_blend);
}
