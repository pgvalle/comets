#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>

SDL_Renderer* app_renderer();
float app_frametime();
void app_window_size(int* width, int* height);

#endif // APP_H