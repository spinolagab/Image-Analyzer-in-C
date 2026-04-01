#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include <SDL3/SDL.h>
#include <stdbool.h>

typedef struct {
    int hist[256];
    double mean;
    double stddev;
    const char *brightness_label;
    const char *contrast_label;
} HistogramInfo;

typedef struct {
    SDL_Surface *original_gray_surface;
    SDL_Surface *equalized_surface;
    SDL_Surface *current_surface;
    SDL_Texture *main_texture;
    bool showing_equalized;
} ImageState;

bool prepare_image_state(const char *image_path, ImageState *state);
void destroy_image_state(ImageState *state);
bool update_main_texture(SDL_Renderer *renderer, ImageState *state);
bool toggle_equalization(SDL_Renderer *renderer, ImageState *state);
void compute_histogram(SDL_Surface *surface, HistogramInfo *info);

#endif
