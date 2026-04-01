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

    if (!TTF_Init()) {
        SDL_Log("Falha ao inicializar SDL_ttf: %s", SDL_GetError());
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

static bool create_windows_and_renderers(const ImageState *state,
                                         SDL_Window **main_window,
                                         SDL_Window **hist_window,
                                         SDL_Renderer **main_renderer,
                                         SDL_Renderer **hist_renderer) {
    *main_window = SDL_CreateWindow(
        "Proj1 - Imagem",
        state->current_surface->w,
        state->current_surface->h,
        0
    );
    if (!*main_window) {
        SDL_Log("Falha ao criar janela principal: %s", SDL_GetError());
        return false;
    }

    SDL_SetWindowPosition(*main_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    *hist_window = SDL_CreateWindow("Proj1 - Histograma", HIST_WINDOW_WIDTH, HIST_WINDOW_HEIGHT, 0);
    if (!*hist_window) {
        SDL_Log("Falha ao criar janela secundária: %s", SDL_GetError());
        return false;
    }

    SDL_SetWindowParent(*hist_window, *main_window);

    int main_x = 0;
    int main_y = 0;
    SDL_GetWindowPosition(*main_window, &main_x, &main_y);
    SDL_SetWindowPosition(*hist_window, main_x + state->current_surface->w + 20, main_y);

    *main_renderer = SDL_CreateRenderer(*main_window, NULL);
    *hist_renderer = SDL_CreateRenderer(*hist_window, NULL);
    if (!*main_renderer || !*hist_renderer) {
        SDL_Log("Falha ao criar renderizadores: %s", SDL_GetError());
        return false;
    }

    return true;
}

static bool handle_event(const SDL_Event *event,
                         SDL_Window *hist_window,
                         SDL_Renderer *main_renderer,
                         ImageState *state,
                         Button *button,
                         bool *running) {
    switch (event->type) {
        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            *running = false;
            break;

        case SDL_EVENT_MOUSE_MOTION:
            if (event->motion.windowID == SDL_GetWindowID(hist_window)) {
                button->hovered = point_in_rect(event->motion.x, event->motion.y, button->rect);
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (event->button.windowID == SDL_GetWindowID(hist_window) && event->button.button == SDL_BUTTON_LEFT) {
                if (point_in_rect(event->button.x, event->button.y, button->rect)) {
                    button->pressed = true;
                }
            }
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (event->button.windowID == SDL_GetWindowID(hist_window) && event->button.button == SDL_BUTTON_LEFT) {
                const bool inside = point_in_rect(event->button.x, event->button.y, button->rect);
                if (button->pressed && inside) {
                    if (!toggle_equalization(main_renderer, state)) {
                        SDL_Log("Falha ao alternar imagem: %s", SDL_GetError());
                        return false;
                    }
                }
                button->pressed = false;
            }
            break;

        case SDL_EVENT_KEY_DOWN:
            if (event->key.scancode == SDL_SCANCODE_S) {
                if (!IMG_SavePNG(state->current_surface, "output_image.png")) {
                    SDL_Log("Falha ao salvar output_image.png: %s", SDL_GetError());
                } else {
                    SDL_Log("Imagem salva em output_image.png");
                }
            }
            break;
    }

    return true;
}

static int run_loop(SDL_Window *hist_window,
                    SDL_Renderer *main_renderer,
                    SDL_Renderer *hist_renderer,
                    TTF_Font *font,
                    ImageState *state) {
    Button button;
    initialize_button(&button);

    bool running = true;
    SDL_Event event;

    while (running) {
        HistogramInfo info;
        compute_histogram(state->current_surface, &info);

        while (SDL_PollEvent(&event)) {
            if (!handle_event(&event, hist_window, main_renderer, state, &button, &running)) {
                return 1;
            }
        }

        render_main_window(main_renderer, state->main_texture, state->current_surface->w, state->current_surface->h);
        render_histogram_window(hist_renderer, font, &info, &button, state->showing_equalized);
        SDL_Delay(16);
    }

    return 0;
}

int run_app(int argc, char *argv[]) {
    if (argc != 2) {
        SDL_Log("Uso: %s <caminho_da_imagem>", argv[0]);
        return 1;
    }

    if (!initialize_libraries()) {
        return 1;
    }

    TTF_Font *font = load_app_font();
    if (!font) {
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    ImageState state;
    if (!prepare_image_state(argv[1], &state)) {
        SDL_Log("Falha ao preparar imagem '%s': %s", argv[1], SDL_GetError());
        cleanup_everything(NULL, NULL, NULL, NULL, font, &(ImageState){0});
        return 1;
    }

    SDL_Window *main_window = NULL;
    SDL_Window *hist_window = NULL;
    SDL_Renderer *main_renderer = NULL;
    SDL_Renderer *hist_renderer = NULL;

    if (!create_windows_and_renderers(&state, &main_window, &hist_window, &main_renderer, &hist_renderer)) {
        cleanup_everything(hist_window, main_window, hist_renderer, main_renderer, font, &state);
        return 1;
    }

    if (!update_main_texture(main_renderer, &state)) {
        SDL_Log("Falha ao criar textura principal: %s", SDL_GetError());
        cleanup_everything(hist_window, main_window, hist_renderer, main_renderer, font, &state);
        return 1;
    }

    const int status = run_loop(hist_window, main_renderer, hist_renderer, font, &state);
    cleanup_everything(hist_window, main_window, hist_renderer, main_renderer, font, &state);
    return status;
}