#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "physics.h"
#include "render.h"
#include "trails.h"

int main(int argc, char **argv) {
    (void)argc; (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() == -1) {
        fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    AppRender app;
    if (render_init(&app, 1280, 720) != 0) {
        fprintf(stderr, "render_init failed\n");
        TTF_Quit(); SDL_Quit(); return 1;
    }

    Pendulum p;
    pendulum_init(&p);

    Trail *trail = trail_create(4000);
    if (!trail) {
        fprintf(stderr, "Failed to create trail\n");
        render_destroy(&app);
        TTF_Quit(); SDL_Quit();
        return 1;
    }

    int running = 1;
    int paused = 0;
    int show_grid = 1, show_trail = 1, show_hud = 1;
    //double sim_speed = 1.0;
    double sim_speed = 10.5;

    Uint32 last = SDL_GetTicks();
    double t_acc = 0.0;
    double dt = 0.008; // physics timestep

    srand((unsigned)time(NULL));

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = 0;
            if (ev.type == SDL_KEYDOWN) {
                SDL_Keycode k = ev.key.keysym.sym;
                if (k == SDLK_ESCAPE) running = 0;
                else if (k == SDLK_SPACE) paused = !paused;
                else if (k == SDLK_r) { pendulum_reset(&p); trail_clear(trail); }
                else if (k == SDLK_x) {
                    p.theta1 = ((double)rand()/RAND_MAX)*M_PI;
                    p.theta2 = ((double)rand()/RAND_MAX)*M_PI;
                    p.omega1 = 0; p.omega2 = 0;
                    trail_clear(trail);
                }
                else if (k == SDLK_z) sim_speed *= 0.6;
                else if (k == SDLK_c) sim_speed *= 1.6;
                else if (k == SDLK_t) show_trail = !show_trail;
                else if (k == SDLK_g) show_grid = !show_grid;
                else if (k == SDLK_h) show_hud = !show_hud;
            }
        }

        Uint32 now = SDL_GetTicks();
        double elapsed = (now - last) / 1000.0;
        last = now;

        if (!paused) {
            t_acc += elapsed * sim_speed;
            /* advance physics in fixed steps */
            while (t_acc >= dt) {
                pendulum_step(&p, dt);
                t_acc -= dt;
                /* record trail */
                int pivot_x = app.width/2;
                int pivot_y = app.height/4;
                float x1 = pivot_x + (float)(p.l1 * sin(p.theta1));
                float y1 = pivot_y + (float)(p.l1 * cos(p.theta1));
                float x2 = x1 + (float)(p.l2 * sin(p.theta2));
                float y2 = y1 + (float)(p.l2 * cos(p.theta2));
                trail_push(trail, x2, y2);
            }
        }

        render_frame(&app, &p, trail, (double)SDL_GetTicks()/1000.0, show_grid, show_trail, show_hud);
        SDL_RenderPresent(app.ren); /* present here */

        SDL_Delay(8); /* small sleep to yield CPU */
    }

    trail_free(trail);
    render_destroy(&app);
    TTF_Quit(); SDL_Quit();
    return 0;
}
