#include "ui.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

#define HIST_MARGIN_X 25
#define HIST_MARGIN_TOP 30
#define HIST_PLOT_WIDTH 400
#define HIST_PLOT_HEIGHT 220
#define TEXT_WRAP_WIDTH 400

static void render_text(SDL_Renderer *renderer, TTF_Font *font, const char *text,
                        float x, float y, SDL_Color color, int wrap_width) {
    if (!font || !text || !*text) {
        return;
    }

    SDL_Surface *text_surface = NULL;
    if (wrap_width > 0) {
        text_surface = TTF_RenderText_Blended_Wrapped(font, text, strlen(text), color, wrap_width);
    } else {
        text_surface = TTF_RenderText_Blended(font, text, strlen(text), color);
    }

    if (!text_surface) {
        return;
    }

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (text_texture) {
        SDL_FRect dst = {x, y, (float)text_surface->w, (float)text_surface->h};
        SDL_RenderTexture(renderer, text_texture, NULL, &dst);
        SDL_DestroyTexture(text_texture);
    }

    SDL_DestroySurface(text_surface);
}
