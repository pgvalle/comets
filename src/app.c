#include <stdbool.h>

#include "app.h"
#include "starship.h"
#include "shot.h"

struct {
    bool run;
    SDL_Window* win;
    SDL_Renderer* ren;
    SDL_Event evt;
} app;

float app_compute_frametime(bool compute) {
    static Uint32 t0, t1 = 0;
    if (compute) {
        t0 = t1;
        t1 = SDL_GetTicks();
    }

    return 1e-3f*(t1 - t0);
}

void app_init() {
    SDL_Init(SDL_INIT_EVERYTHING);
    app.run = true;
    app.win = SDL_CreateWindow("Astroids clone (Comets) by PG",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        500, 500, 0);
    app.ren = SDL_CreateRenderer(app.win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    starship_init();
}

void app_destroy(){
    SDL_DestroyRenderer(app.ren);
    SDL_DestroyWindow(app.win);
    SDL_Quit();
}

void app_update() {
    starship_update();
    shots_update();
}

void app_render() {
    SDL_SetRenderDrawColor(app.ren, 0x12, 0x12, 0x12, 0xff);
    SDL_RenderClear(app.ren);

    starship_render();
    shots_render();

    SDL_RenderPresent(app.ren);
}

int main() {
    // INICIALIZACAO
    app_init();

    while (app.run) {
        app_compute_frametime(true);

        while (SDL_PollEvent(&app.evt)) {
            if (app.evt.type == SDL_QUIT)
                app.run = false;
        }

        app_update();
        app_render();
    }

    // FINALIZACAO
    app_destroy();
}

SDL_Renderer* app_renderer() {
    return app.ren;
}

float app_frametime() {
    return app_compute_frametime(false);
}

void app_window_size(int* width, int* height) {
    SDL_GetWindowSize(app.win, width, height);
}
