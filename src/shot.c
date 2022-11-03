#include <stdbool.h>

#include "shot.h"
#include "app.h"

#define SHOT_VEL 2.5f

typedef struct shot_t {
    float x, y;
    float vx, vy;
    float dmg;
} shot_t;

shot_t shots[100];
int shot_count = 0;

void shot_add(float x, float y, float dx, float dy, float dmg) {
    float norm = SHOT_VEL/sqrtf(dx*dx + dy*dy);

    shots[shot_count].x = x;
    shots[shot_count].y = y;
    shots[shot_count].vx = norm*dx;
    shots[shot_count].vy = norm*dy;
    shots[shot_count].dmg = dmg;
    shot_count++;
}

bool shot_oob(shot_t const* shot) {
    int width, height;
    app_window_size(&width, &height);

    if (shot->x - 1 < 0 || shot->y - 1 < 0 ||
      shot->x + 1 >= width || shot->y + 1 >= height)
        return true;
    return false;
}

void shots_update() {
    for (int i = 0; i < shot_count; i++) {
        shots[i].x += shots[i].vx;
        shots[i].y += shots[i].vy;

        if (shot_oob(&shots[i])) {
            for (int j = i; j < shot_count; j++) {
                shots[j] = shots[j + 1];
            }
            shot_count--;
            i--;
        }
    }
}

void shots_render() {
    SDL_Rect rect = {0, 0, 2, 2};
    SDL_Renderer* ren = app_renderer();
    SDL_SetRenderDrawColor(ren, 0xff, 0xff, 0xff, 0xff);

    for (int i = 0; i < shot_count; i++) {
        rect.x = shots[i].x - 1;
        rect.y = shots[i].y - 1;
        SDL_RenderFillRect(ren, &rect);
    }
}