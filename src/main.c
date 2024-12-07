/*
 * 3D engine made with SDL2 and C language
 * Author: Linus Hasselkvist
 * 2024-12-07
 * 
 * */

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <GL/glew.h>

#define res         4               // 0=160*X 1=360*X 4=640*X ...
#define aspectRatio 16/9
#define SH          160*res
#define SW          SH*aspectRatio
#define SH2         SH/2
#define SW2         SW/2
#define FPS         60  
//-----------------------------------------------------------------------
typedef struct eventHandler {
    SDL_Event event;
    int running;
    int fullScreen;
} EventH;

void render();
void getWindowEvents(EventH *eh, SDL_Window *win);
void toggleFullscreen(EventH *eh, SDL_Window *win);

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("SDL2 3D Engine", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        SW, SH, SDL_WINDOW_OPENGL);

    SDL_GLContext glContext = SDL_GL_CreateContext(win);
    if (!glContext) {
        printf("Kunde inte skapa OpenGL-kontext: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return -1;
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("Kunde inte initiera GLEW\n");
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return -1;
    }

    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

    EventH eh = {.running = 1, .fullScreen = 0};

    // MAIN LOOP
    while(eh.running) {
        getWindowEvents(&eh, win);
        render();
        SDL_GL_SwapWindow(win);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
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