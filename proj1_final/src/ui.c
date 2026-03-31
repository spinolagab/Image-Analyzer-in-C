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

static void draw_button(SDL_Renderer *renderer, TTF_Font *font, const Button *button, const char *label) {
    Uint8 r = 35, g = 115, b = 220;
    if (button->pressed) {
        r = 20;
        g = 80;
        b = 170;
    } else if (button->hovered) {
        r = 90;
        g = 160;
        b = 245;
    }

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderFillRect(renderer, &button->rect);

    SDL_SetRenderDrawColor(renderer, 15, 45, 90, 255);
    SDL_RenderRect(renderer, &button->rect);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, label, strlen(label), white);
    if (!text_surface) {
        return;
    }

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (text_texture) {
        SDL_FRect dst = {
            button->rect.x + (button->rect.w - (float)text_surface->w) / 2.0f,
            button->rect.y + (button->rect.h - (float)text_surface->h) / 2.0f,
            (float)text_surface->w,
            (float)text_surface->h
        };
        SDL_RenderTexture(renderer, text_texture, NULL, &dst);
        SDL_DestroyTexture(text_texture);
    }

    SDL_DestroySurface(text_surface);
}

static void draw_histogram(SDL_Renderer *renderer, const HistogramInfo *info) {
    int max_count = 1;
    for (int i = 0; i < 256; ++i) {
        if (info->hist[i] > max_count) {
            max_count = info->hist[i];
        }
    }

    SDL_FRect border = {(float)HIST_MARGIN_X, (float)HIST_MARGIN_TOP, (float)HIST_PLOT_WIDTH, (float)HIST_PLOT_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
    SDL_RenderRect(renderer, &border);

    for (int i = 0; i < 256; ++i) {
        const float bar_w = (float)HIST_PLOT_WIDTH / 256.0f;
        const float normalized = (float)info->hist[i] / (float)max_count;
        const float bar_h = normalized * (float)(HIST_PLOT_HEIGHT - 4);
        SDL_FRect bar = {
            HIST_MARGIN_X + i * bar_w,
            HIST_MARGIN_TOP + HIST_PLOT_HEIGHT - bar_h,
            (bar_w > 1.0f ? bar_w : 1.0f),
            bar_h
        };
        SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
        SDL_RenderFillRect(renderer, &bar);
    }
}

void initialize_button(Button *button) {
    button->rect.x = (HIST_WINDOW_WIDTH - BUTTON_W) / 2.0f;
    button->rect.y = 500.0f;
    button->rect.w = (float)BUTTON_W;
    button->rect.h = (float)BUTTON_H;
    button->hovered = false;
    button->pressed = false;
}

bool point_in_rect(float x, float y, SDL_FRect rect) {
    return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

void render_main_window(SDL_Renderer *renderer, SDL_Texture *texture, int image_w, int image_h) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_FRect dst = {0.0f, 0.0f, (float)image_w, (float)image_h};
    SDL_RenderTexture(renderer, texture, NULL, &dst);
    SDL_RenderPresent(renderer);
}

void render_histogram_window(SDL_Renderer *renderer, TTF_Font *font, const HistogramInfo *info,
                             const Button *button, bool showing_equalized) {
    SDL_SetRenderDrawColor(renderer, 248, 248, 248, 255);
    SDL_RenderClear(renderer);

    SDL_Color black = {20, 20, 20, 255};
    SDL_Color gray = {80, 80, 80, 255};

    render_text(renderer, font, "Histograma da imagem", 24.0f, 6.0f, black, 0);
    draw_histogram(renderer, info);

    char line1[128];
    char line2[128];
    char line3[128];
    SDL_snprintf(line1, sizeof(line1), "Media de intensidade: %.2f (%s)", info->mean, info->brightness_label);
    SDL_snprintf(line2, sizeof(line2), "Desvio padrao: %.2f (contraste %s)", info->stddev, info->contrast_label);
    SDL_snprintf(line3, sizeof(line3), "Estado atual: %s", showing_equalized ? "equalizada" : "original em cinza");

    render_text(renderer, font, line1, 24.0f, 275.0f, black, TEXT_WRAP_WIDTH);
    render_text(renderer, font, line2, 24.0f, 315.0f, black, TEXT_WRAP_WIDTH);
    render_text(renderer, font, line3, 24.0f, 355.0f, black, TEXT_WRAP_WIDTH);

    render_text(renderer, font,
                "Tecla S: salva a imagem exibida como output_image.png",
                24.0f, 410.0f, gray, TEXT_WRAP_WIDTH);

    draw_button(renderer, font, button, showing_equalized ? "Ver original" : "Equalizar");

    SDL_RenderPresent(renderer);
}
