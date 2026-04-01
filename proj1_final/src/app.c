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

static bool initialize_libraries(void) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Falha ao inicializar SDL: %s", SDL_GetError());
        return false;
    }

    const int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(img_flags) & img_flags) != img_flags) {
        SDL_Log("Falha ao inicializar SDL_image: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    if (!TTF_Init()) {
        SDL_Log("Falha ao inicializar SDL_ttf: %s", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    return true;
}

static TTF_Font *load_app_font(void) {
    const char *font_path = find_font_path();
    if (!font_path) {
        SDL_Log("Nenhuma fonte .ttf encontrada. Coloque uma fonte em assets/DejaVuSans.ttf ou assets/arial.ttf");
        return NULL;
    }

    TTF_Font *font = TTF_OpenFont(font_path, 18.0f);
    if (!font) {
        SDL_Log("Falha ao abrir fonte '%s': %s", font_path, SDL_GetError());
    }

    return font;
}
