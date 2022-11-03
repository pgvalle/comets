#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "starship.h"
#include "app.h"
#include "shot.h"

#define PIf ((float)3.14159265358979323846)

#define SS_SHOT_COOLDOWN 0.2f // 200ms
#define SS_SHIELD_COOLDOWN 10.0f

struct {
    SDL_FPoint c;
    SDL_FPoint tri[3];
    float vx, vy;
    float shot_cooldown, shield_cooldown;
    bool shield_active;
} ss;

void starship_init() {
    float const size = 10.0f;
    int width, height;
    app_window_size(&width, &height);
    
    ss.c.x = 0.5f*width;
    ss.c.y = 0.5f*height;

    ss.shot_cooldown = ss.shield_cooldown = 0.0f;
    ss.shield_active = false;

    // top
    ss.tri[0].x = ss.c.x;
    ss.tri[0].y = ss.c.y - size;
    // bottom-right
    ss.tri[1].x = ss.c.x + size/sqrtf(3);
    ss.tri[1].y = ss.c.y + 1.0f/3.0f*size;
    // bottom-left
    ss.tri[2].x = ss.c.x - size/sqrtf(3);
    ss.tri[2].y = ss.c.y + 1.0f/3.0f*size;
}

void starship_rotate(float a) {
    a = -a * PIf / 180.0f; // convert to radians
    float const cosa = cos(a);
    float const sina = sin(a);
    float const cx = ss.c.x, cy = ss.c.y;
    float x, y;

    // rotate each point, rotate
    for (int i = 0; i < 3; i++) {
        x = ss.tri[i].x;
        y = ss.tri[i].y;

        ss.tri[i].x = (x - cx)*cosa - (y - cy)*sina + cx;
        ss.tri[i].y = (x - cx)*sina + (y - cy)*cosa + cy;
    }
}

void starship_move() {
    int points_out = 0;
    int width, height;
    app_window_size(&width, &height);

    ss.c.x += ss.vx;
    ss.c.y += ss.vy;

    for (int i = 0; i < 3; i++) {
        ss.tri[i].x += ss.vx;
        ss.tri[i].y += ss.vy;

        if (ss.tri[i].x < 0.0f || ss.tri[i].x >= width ||
          ss.tri[i].y < 0.0f || ss.tri[i].y >= height) {
            points_out++;
        }
    }

    if (points_out == 3) {
        if (ss.c.x < 0.0f || ss.c.x >= width) {
            float mx = 1.0f;
            if (ss.c.x > width) {
                mx = -1.0f;
            }
            ss.c.x += mx*width;

            for (int i = 0; i < 3; i++) {
                ss.tri[i].x += mx*width;
            }
        }
        if (ss.c.y < 0.0f || ss.c.y >= height) {
            float my = 1.0f;
            if (ss.c.y > height) {
                my = -1.0f;
            }
            ss.c.y += my*height;

            for (int i = 0; i < 3; i++) {
                ss.tri[i].y += my*height;
            }
        }
    }
}

void starship_shoot() {
    float dx = ss.tri[0].x - ss.c.x;
    float dy = ss.tri[0].y - ss.c.y;
    shot_add(ss.tri[0].x, ss.tri[0].y, dx, dy, 10.0f);
}

void starship_update() {
    float vel_incr = 0.05f;

    // rotation keys
    bool j_press = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_J];
    bool k_press = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_K];
    // movement keys
    bool w_press = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_W];
    bool s_press = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_S];
    bool a_press = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_A];
    bool d_press = SDL_GetKeyboardState(NULL)[SDL_SCANCODE_D];

    // ROTATION
    if (j_press)
        starship_rotate(2.5);
    if (k_press)
        starship_rotate(-2.5);

    // MOVEMENT
    // diagonal movement
    if ((w_press || s_press) && (a_press || d_press)) {
        vel_incr /= sqrtf(2);
    }

    if ((w_press && s_press) || !(w_press || s_press)) {
        ss.vy *= 0.98f;
    } else if (w_press && ss.vy > -2.0f) {
        ss.vy -= vel_incr;
    } else if (s_press && ss.vy < 2.0f) {
        ss.vy += vel_incr;
    }

    if ((a_press && d_press) || !(a_press || d_press)) {
        ss.vx *= 0.98f;
    } else if (a_press && ss.vx > -2.0f) {
        ss.vx -= vel_incr;
    } else if (d_press && ss.vx < 2.0f) {
        ss.vx += vel_incr;
    }

    starship_move();

    // SHOT ABILITY
    if (ss.shot_cooldown >= 0.0f)
        ss.shot_cooldown -= app_frametime();

    if (ss.shot_cooldown <= 0.0f && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_SPACE]) {
        ss.shot_cooldown = SS_SHOT_COOLDOWN;
        starship_shoot();
    }

    // SHIELD ABILITY
    // ...
}

void starship_render() {
    SDL_SetRenderDrawColor(app_renderer(), 0xff, 0xff, 0xff, 0xff);
    
    SDL_RenderDrawLineF(app_renderer(), ss.tri[0].x, ss.tri[0].y,
        ss.tri[1].x, ss.tri[1].y);
    SDL_RenderDrawLineF(app_renderer(), ss.tri[1].x, ss.tri[1].y,
        ss.tri[2].x, ss.tri[2].y);
    SDL_RenderDrawLineF(app_renderer(), ss.tri[2].x, ss.tri[2].y,
        ss.tri[0].x, ss.tri[0].y);
}

