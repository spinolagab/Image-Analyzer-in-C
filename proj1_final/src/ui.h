#ifndef UI_H
#define UI_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#include "image_processing.h"

#define HIST_WINDOW_WIDTH 460
#define HIST_WINDOW_HEIGHT 620
#define BUTTON_W 180
#define BUTTON_H 48

typedef struct {
    SDL_FRect rect;
    bool hovered;
    bool pressed;
} Button;

void initialize_button(Button *button);
bool point_in_rect(float x, float y, SDL_FRect rect);
void render_main_window(SDL_Renderer *renderer, SDL_Texture *texture, int image_w, int image_h);
void render_histogram_window(SDL_Renderer *renderer, TTF_Font *font, const HistogramInfo *info,
                             const Button *button, bool showing_equalized);

#endif
