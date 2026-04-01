#include "app.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdbool.h>

#include "font_utils.h"
#include "image_processing.h"
#include "ui.h"

static void cleanup_everything(SDL_Window *hist_window,
                               SDL_Window *main_window,
                               SDL_Renderer *hist_renderer,
                               SDL_Renderer *main_renderer,
                               TTF_Font *font,
                               ImageState *state) {
    destroy_image_state(state);

    if (hist_renderer) {
        SDL_DestroyRenderer(hist_renderer);
    }
    if (main_renderer) {
        SDL_DestroyRenderer(main_renderer);
    }
    if (hist_window) {
        SDL_DestroyWindow(hist_window);
    }
    if (main_window) {
        SDL_DestroyWindow(main_window);
    }
    if (font) {
        TTF_CloseFont(font);
    }

    TTF_Quit();
    SDL_Quit();
}
