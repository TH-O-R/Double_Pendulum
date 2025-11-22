#include "render.h"
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <math.h>

static SDL_Color tron_blue = {20, 180, 255, 255};
static SDL_Color tron_accent = {80, 220, 255, 255};
static SDL_Color bg_dark = {6, 10, 30, 255};

/* Safe wrapper for setting draw color */
static void set_color(SDL_Renderer *ren, SDL_Color c) {
    SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, c.a);
}

int render_init(AppRender *r, int w, int h) {
    if (!r) return -1;
    r->width = w; r->height = h;
    r->win = SDL_CreateWindow("Double Pendulum",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!r->win) return -1;

    r->ren = SDL_CreateRenderer(r->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!r->ren) { SDL_DestroyWindow(r->win); return -1; }

    /* Try relative path first, then system fonts */
    const char *font_paths[] = {
        "assets/JetBrainsMonoNerdFontMono-Regular.ttf",
        "./assets/JetBrainsMonoNerdFontMono-Regular.ttf",
        "../assets/JetBrainsMonoNerdFontMono-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        NULL
    };
    
    r->font = NULL;
    for (int i = 0; font_paths[i] != NULL; i++) {
        r->font = TTF_OpenFont(font_paths[i], 18);
        if (r->font) break;
    }
    
    if (!r->font) {
        fprintf(stderr, "Warning: could not open any font (TTF_OpenFont failed)\n");
    }

    /* Good defaults */
    SDL_SetRenderDrawBlendMode(r->ren, SDL_BLENDMODE_NONE);
    return 0;
}

void render_destroy(AppRender *r) {
    if (!r) return;
    if (r->font) TTF_CloseFont(r->font);
    if (r->ren) SDL_DestroyRenderer(r->ren);
    if (r->win) SDL_DestroyWindow(r->win);
}

/* Draw a filled circle using midpoint algorithm (integer) */
static void filled_circle(SDL_Renderer *ren, int cx, int cy, int radius) {
    if (radius <= 0) return;
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        SDL_RenderDrawLine(ren, cx - x, cy + y, cx + x, cy + y);
        SDL_RenderDrawLine(ren, cx - y, cy + x, cx + y, cy + x);
        SDL_RenderDrawLine(ren, cx - x, cy - y, cx + x, cy - y);
        SDL_RenderDrawLine(ren, cx - y, cy - x, cx + y, cy - x);

        y += 1;
        err += 1 + 2*y;
        if (2*(err - x) + 1 > 0) {
            x -= 1;
            err += 1 - 2*x;
        }
    }
}

/* Draw a soft glow by layering concentric filled circles with additive blending */
static void draw_glow_point(SDL_Renderer *ren, int x, int y, int maxr, SDL_Color color) {
    if (maxr <= 0) return;
    SDL_BlendMode prev = SDL_BLENDMODE_NONE;
    SDL_GetRenderDrawBlendMode(ren, &prev);
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_ADD);

    for (int r = maxr; r > 0; r -= 3) {
        float a = (float)r / (float)maxr; /* 1.0 -> 0.0 */
        SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, (Uint8)(a * 120));
        filled_circle(ren, x, y, r);
    }

    SDL_SetRenderDrawBlendMode(ren, prev);
}

static void draw_grid(SDL_Renderer *ren, int w, int h, SDL_Color col) {
    SDL_BlendMode prev_blend;
    SDL_GetRenderDrawBlendMode(ren, &prev_blend);
    
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ren, col.r, col.g, col.b, 30);
    int spacing = 40;
    for (int x = 0; x < w; x += spacing) SDL_RenderDrawLine(ren, x, 0, x, h);
    for (int y = 0; y < h; y += spacing) SDL_RenderDrawLine(ren, 0, y, w, y);
    
    SDL_SetRenderDrawBlendMode(ren, prev_blend);
}

/* Render a single frame. - p: pendulum state - trail: optional trail buffer */
void render_frame(AppRender *r, Pendulum *p, Trail *trail, double t, int show_grid, int show_trail, int show_hud) {
    if (!r || !r->ren || !p) return;

    /* background */
    set_color(r->ren, bg_dark);
    SDL_RenderClear(r->ren);

    if (show_grid) draw_grid(r->ren, r->width, r->height, tron_blue);

    int pivot_x = r->width / 2;
    int pivot_y = r->height / 4;

    int x1 = pivot_x + (int)(p->l1 * sin(p->theta1));
    int y1 = pivot_y + (int)(p->l1 * cos(p->theta1));
    int x2 = x1 + (int)(p->l2 * sin(p->theta2));
    int y2 = y1 + (int)(p->l2 * cos(p->theta2));

    /* Render trail under the rods (neon underlay) */
    if (show_trail && trail) {
        SDL_Color trail_color = tron_accent;
        trail_render(r->ren, trail, trail_color, 2.0f);
    }

    /* Rods (slightly translucent) */
    SDL_SetRenderDrawColor(r->ren, 60, 200, 255, 220);
    SDL_RenderDrawLine(r->ren, pivot_x, pivot_y, x1, y1);
    SDL_RenderDrawLine(r->ren, x1, y1, x2, y2);

    /* Bobs with glow */
    draw_glow_point(r->ren, x1, y1, 22, tron_blue);
    draw_glow_point(r->ren, x2, y2, 30, tron_accent);

    /* Crisp inner fills (foreground) */
    SDL_SetRenderDrawBlendMode(r->ren, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(r->ren, 200, 235, 255, 255);
    filled_circle(r->ren, x1, y1, 6);
    filled_circle(r->ren, x2, y2, 8);

    /* Additive pop on top */
    SDL_SetRenderDrawBlendMode(r->ren, SDL_BLENDMODE_ADD);
    SDL_SetRenderDrawColor(r->ren, tron_accent.r, tron_accent.g, tron_accent.b, 100);
    filled_circle(r->ren, x2, y2, 4);
    SDL_SetRenderDrawBlendMode(r->ren, SDL_BLENDMODE_NONE);

    /* HUD: render simple text if available */
    if (show_hud && r->font) {
        char info[256];
        double energy = pendulum_total_energy(p);
        snprintf(info, sizeof(info), "m1: %.2f  m2: %.2f   l1: %.1f  l2: %.1f   energy: %.2f",
                 p->m1, p->m2, p->l1, p->l2, energy);
        SDL_Color white = {230, 245, 255, 255};
        SDL_Surface *s = TTF_RenderText_Blended(r->font, info, white);
        if (s) {
            SDL_Texture *ttex = SDL_CreateTextureFromSurface(r->ren, s);
            if (ttex) {
                SDL_Rect dst = {10, 10, s->w, s->h};
                SDL_RenderCopy(r->ren, ttex, NULL, &dst);
                SDL_DestroyTexture(ttex);
            }
            SDL_FreeSurface(s);
        }
    }

    /* present is done by caller in main loop */
}
