#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#define SCREEN_HEIGHT 1080 *2/3
#define SCREEN_WIDTH SCREEN_HEIGHT *16/9
#define FPS 60

typedef struct eventHandler {
    SDL_Event event;
    int running;
    int fullScreen;
} EventH;

void render(SDL_Renderer *rend);
void getWindowEvents(EventH *eh, SDL_Window *win);
void toggleFullscreen(EventH *eh, SDL_Window *win);

int main(int argc, char *argv[]) {

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("SDL2 3D Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    EventH eh = {.running = 1, .fullScreen = 0};

    while(eh.running) {
        getWindowEvents(&eh, win);

        render(rend);

        SDL_Delay(1000/FPS);
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}

void render(SDL_Renderer *rend) {
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
    SDL_RenderClear(rend);

    SDL_RenderPresent(rend);
}

void getWindowEvents(EventH *eh, SDL_Window *win) {
    while(SDL_PollEvent(&(eh->event))) {
        switch(eh->event.type) {
            case SDL_QUIT: 
                eh->running = 0; 
                break;
            case SDL_KEYDOWN: 
                if(eh->event.key.keysym.sym == SDLK_f) {
                    toggleFullscreen(eh, win);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                break;
        }
    }
}

void toggleFullscreen(EventH *eh, SDL_Window *win) {
    eh->fullScreen = !eh->fullScreen;

    if (eh->fullScreen) {
        SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(win, 0);
    }
}