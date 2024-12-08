/*
 * 3D engine made with SDL2 and C language
 * Author: Linus Hasselkvist
 * 2024-12-07
 * 
 * */

#include <stdio.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "shader.h"

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

typedef struct windowModel {
    SDL_Window *win;
    SDL_GLContext glContext;
} WindowModel;

void render(unsigned int shaderProgram, unsigned int VAO);
void getWindowEvents(EventH *eh, SDL_Window *win);
void toggleFullscreen(EventH *eh, SDL_Window *win);
int initializeWindow(WindowModel *wm);

int main(int argc, char *argv[]) {
    WindowModel wm;
    if(!initializeWindow(&wm)) return -1;
    
    float vertices[] = {
        // Positions        // Colors
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom-left (red)
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom-right (green)
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Top (blue)
    };


//-------------------------------------------------------------- VAO and VBO
    // Create VAO and VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for vertex shader compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex Shader Compilation Failed:\n%s\n", infoLog);
    }

    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for fragment shader compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment Shader Compilation Failed:\n%s\n", infoLog);
    }

    // Link shaders into a program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Shader Program Linking Failed:\n%s\n", infoLog);
    }

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


//-------------------------------------------------------------- MAIN LOOP 
    EventH eh = {.running = 1, .fullScreen = 0};
    while(eh.running) {
        getWindowEvents(&eh, wm.win);
        render(shaderProgram, VAO);
        SDL_GL_SwapWindow(wm.win);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    SDL_GL_DeleteContext(wm.glContext);
    SDL_DestroyWindow(wm.win);
    SDL_Quit();

    return 0;
}

void render(unsigned int shaderProgram, unsigned int VAO) {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
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

int initializeWindow(WindowModel *wm) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    wm->win = SDL_CreateWindow("SDL2 3D Engine", 
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                        SW, SH, SDL_WINDOW_OPENGL);

    wm->glContext = SDL_GL_CreateContext(wm->win);
    if (!wm->glContext) {
        printf("Failed to create OpenGL context: %s\n", SDL_GetError());
        SDL_DestroyWindow(wm->win);
        SDL_Quit();
        return 0;
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        SDL_GL_DeleteContext(wm->glContext);
        SDL_DestroyWindow(wm->win);
        SDL_Quit();
        return 0;
    }

    // Enable V-Sync
    SDL_GL_SetSwapInterval(1);

    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    return 1;
}