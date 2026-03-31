#include "image_processing.h"

#include <SDL3_image/SDL_image.h>
#include <math.h>
#include <string.h>

#define CLAMP_255(v) ((v) < 0 ? 0 : ((v) > 255 ? 255 : (v)))

static void destroy_texture(SDL_Texture **texture) {
    if (*texture) {
        SDL_DestroyTexture(*texture);
        *texture = NULL;
    }
}

static bool is_grayscale_surface(SDL_Surface *surface) {
    Uint8 *pixels = (Uint8 *)surface->pixels;

    for (int y = 0; y < surface->h; ++y) {
        Uint8 *row = pixels + y * surface->pitch;
        for (int x = 0; x < surface->w; ++x) {
            Uint8 *p = row + x * 4;
            if (!(p[0] == p[1] && p[1] == p[2])) {
                return false;
            }
        }
    }

    return true;
}

static void convert_to_grayscale(SDL_Surface *surface) {
    Uint8 *pixels = (Uint8 *)surface->pixels;

    for (int y = 0; y < surface->h; ++y) {
        Uint8 *row = pixels + y * surface->pitch;
        for (int x = 0; x < surface->w; ++x) {
            Uint8 *p = row + x * 4;
            const Uint8 r = p[0];
            const Uint8 g = p[1];
            const Uint8 b = p[2];
            const Uint8 gray = (Uint8)CLAMP_255((int)lround(0.2125 * r + 0.7154 * g + 0.0721 * b));
            p[0] = gray;
            p[1] = gray;
            p[2] = gray;
        }
    }
}

static SDL_Surface *duplicate_and_equalize(SDL_Surface *source_gray) {
    SDL_Surface *dst = SDL_DuplicateSurface(source_gray);
    if (!dst) {
        return NULL;
    }

    int hist[256] = {0};
    Uint8 *pixels = (Uint8 *)dst->pixels;
    const int total_pixels = dst->w * dst->h;

    for (int y = 0; y < dst->h; ++y) {
        Uint8 *row = pixels + y * dst->pitch;
        for (int x = 0; x < dst->w; ++x) {
            Uint8 *p = row + x * 4;
            hist[p[0]]++;
        }
    }

    int cdf[256] = {0};
    cdf[0] = hist[0];
    for (int i = 1; i < 256; ++i) {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    int cdf_min = 0;
    for (int i = 0; i < 256; ++i) {
        if (cdf[i] != 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    if (total_pixels == cdf_min) {
        return dst;
    }

    for (int y = 0; y < dst->h; ++y) {
        Uint8 *row = pixels + y * dst->pitch;
        for (int x = 0; x < dst->w; ++x) {
            Uint8 *p = row + x * 4;
            const Uint8 oldv = p[0];
            const double norm = ((double)(cdf[oldv] - cdf_min) / (double)(total_pixels - cdf_min)) * 255.0;
            const Uint8 newv = (Uint8)CLAMP_255((int)lround(norm));
            p[0] = newv;
            p[1] = newv;
            p[2] = newv;
        }
    }

    return dst;
}

bool prepare_image_state(const char *image_path, ImageState *state) {
    memset(state, 0, sizeof(*state));

    SDL_Surface *loaded = IMG_Load(image_path);
    if (!loaded) {
        return false;
    }

    SDL_Surface *rgba = SDL_ConvertSurface(loaded, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(loaded);
    if (!rgba) {
        return false;
    }

    if (!is_grayscale_surface(rgba)) {
        convert_to_grayscale(rgba);
    }

    state->original_gray_surface = rgba;
    state->equalized_surface = duplicate_and_equalize(rgba);
    if (!state->equalized_surface) {
        destroy_image_state(state);
        return false;
    }

    state->current_surface = state->original_gray_surface;
    state->showing_equalized = false;
    return true;
}

void destroy_image_state(ImageState *state) {
    if (!state) {
        return;
    }

    destroy_texture(&state->main_texture);

    if (state->equalized_surface) {
        SDL_DestroySurface(state->equalized_surface);
        state->equalized_surface = NULL;
    }

    if (state->original_gray_surface) {
        SDL_DestroySurface(state->original_gray_surface);
        state->original_gray_surface = NULL;
    }

    state->current_surface = NULL;
    state->showing_equalized = false;
}

bool update_main_texture(SDL_Renderer *renderer, ImageState *state) {
    destroy_texture(&state->main_texture);
    state->main_texture = SDL_CreateTextureFromSurface(renderer, state->current_surface);
    return state->main_texture != NULL;
}

bool toggle_equalization(SDL_Renderer *renderer, ImageState *state) {
    state->showing_equalized = !state->showing_equalized;
    state->current_surface = state->showing_equalized ? state->equalized_surface : state->original_gray_surface;
    return update_main_texture(renderer, state);
}

void compute_histogram(SDL_Surface *surface, HistogramInfo *info) {
    memset(info, 0, sizeof(*info));

    const int total_pixels = surface->w * surface->h;
    Uint8 *pixels = (Uint8 *)surface->pixels;

    for (int y = 0; y < surface->h; ++y) {
        Uint8 *row = pixels + y * surface->pitch;
        for (int x = 0; x < surface->w; ++x) {
            Uint8 *p = row + x * 4;
            info->hist[p[0]]++;
        }
    }

    double weighted_sum = 0.0;
    for (int i = 0; i < 256; ++i) {
        weighted_sum += (double)i * (double)info->hist[i];
    }
    info->mean = weighted_sum / (double)total_pixels;

    double variance_sum = 0.0;
    for (int i = 0; i < 256; ++i) {
        const double diff = (double)i - info->mean;
        variance_sum += diff * diff * (double)info->hist[i];
    }
    info->stddev = sqrt(variance_sum / (double)total_pixels);

    if (info->mean < 85.0) {
        info->brightness_label = "escura";
    } else if (info->mean < 170.0) {
        info->brightness_label = "media";
    } else {
        info->brightness_label = "clara";
    }

    if (info->stddev < 40.0) {
        info->contrast_label = "baixo";
    } else if (info->stddev < 80.0) {
        info->contrast_label = "medio";
    } else {
        info->contrast_label = "alto";
    }
}
