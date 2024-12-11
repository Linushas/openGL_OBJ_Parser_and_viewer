#include <stdio.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "shader.h"
#include "mesh.h"
#include "projection.h"

typedef struct eventHandler {
    SDL_Event event;
    int running;
    int fullScreen;
    int r, n;
    int zoom;
    float mouseMotionX, mouseMotionY;
    int mouseDown;
    int mouseMiddle;
    int shift;
} EventH;

typedef struct windowModel {
    SDL_Window *win;
    SDL_GLContext glContext;
    EventH *eh;
    unsigned int shaderProgram;
} WindowModel;

void render(unsigned int shaderProgram, EventH *eh, Mesh *mesh, int meshCount);
void getWindowEvents(EventH *eh, WindowModel *wm);
void toggleFullscreen(EventH *eh, WindowModel *wm);
int initializeWindow(WindowModel *wm);

int main(int argc, char *argv[]) {
    WindowModel wm;
    EventH eh = {.running = 1, .fullScreen = 0, .r = 0, .n = 0};
    if(!initializeWindow(&wm)) return -1;
    wm.eh = &eh;

    Mesh meshes[10];
    int meshCount = 3;

    meshes[0] = parseOBJ(OBJ_IXO_SPHERE, POS(0.0f, 0.0f, 0.0f), "red", 0.5f);
    meshes[1] = parseOBJ(OBJ_MONKEY, POS(2.0f, 0.0f, 0.0f), "yellow", 1.0f);
    meshes[2] = parseOBJ(OBJ_TORUS, POS(-2.0f, 0.0f, 0.0f), "cyan", 1.0f);

    unsigned int shaderProgram;
    loadShaders(&shaderProgram);

    Mat4x4 model = {0}, view = {0}, projection = {0};
    Vertex eye = {0.0f, 0.0f, 4.0f};
    Vertex target = {0.0f, 0.0f, 0.0f};
    Vertex up = {0.0f, 1.0f, 0.0f};
    setupMatrices(&model, &view, &projection, shaderProgram, eye, target, up);

    float angleX = 0.0f, angleY = 0.0f, angleZ = 0.0f;

    while(eh.running) {
        getWindowEvents(&eh, &wm);

        if(eh.mouseDown) {
            if(eh.mouseMiddle) {
                if(eh.shift) {
                    // Pan
                    eye.x += -eh.mouseMotionX * sensitivity;
                    eye.y += eh.mouseMotionY * sensitivity/2*1.3f;
                    
                    target.x += -eh.mouseMotionX * sensitivity;
                    target.y += eh.mouseMotionY * sensitivity*1.3f;
                }
                else {
                    // Orbit
                    angleX += -eh.mouseMotionY * sensitivity;
                    angleY += -eh.mouseMotionX * sensitivity;
                }
            }
        }

        if (eh.zoom == 1) {
            eye.z -= 0.2f; // Zoom in
        } 
        else if (eh.zoom == 2) {
            eye.z += 0.2f; // Zoom out
        }

        // Set up light properties
        glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 1.8f, 6.0f, 8.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), eye.x, eye.y, eye.z);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 0.5f, 0.5f, 0.5f);

        setupMatrices(&model, &view, &projection, shaderProgram, eye, target, up);
        createRotationMatrix(&model, angleX, angleY, angleZ);

        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model.m[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view.m[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection.m[0][0]);

        render(shaderProgram, &eh, meshes, meshCount);

        SDL_GL_SwapWindow(wm.win);
    }

    for(int i = 0; i < meshCount; i++){
        destroyMesh(&meshes[i]);
    }
    
    glDeleteProgram(shaderProgram);
    
    SDL_GL_DeleteContext(wm.glContext);
    SDL_DestroyWindow(wm.win);
    SDL_Quit();

    return 0;
}

void render(unsigned int shaderProgram, EventH *eh, Mesh *mesh, int meshCount) {
    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    for(int i = 0; i < meshCount; i++) {
        if(eh->r) {
            renderMesh(mesh[i], GL_TRIANGLES);
        } 
        else {
            renderMesh(mesh[i], GL_LINE_LOOP);
        } 
    }
    glBindVertexArray(0);
}

void getWindowEvents(EventH *eh, WindowModel *wm) {
    eh->zoom = 0;
    eh->mouseMotionX = 0;
    eh->mouseMotionY = 0;

    while(SDL_PollEvent(&(eh->event))) {
        switch(eh->event.type) {
            case SDL_QUIT: 
                eh->running = 0; 
                break;
            
            case SDL_KEYDOWN: 
                if(eh->event.key.keysym.sym == SDLK_F11) {
                    toggleFullscreen(eh, wm);
                }
                if(eh->event.key.keysym.sym == SDLK_r) {
                    eh->r = !eh->r;
                }
                if(eh->event.key.keysym.sym == SDLK_n) {
                    eh->n = (eh->n + 1) % 4;
                }
                if(eh->event.key.keysym.sym == SDLK_LSHIFT) {
                    eh->shift = 1;
                }
                break;
            case SDL_KEYUP:
                if(eh->event.key.keysym.sym == SDLK_LSHIFT) {
                    eh->shift = 0;
                }
                break;
            
            case SDL_MOUSEBUTTONDOWN:
                eh->mouseDown = 1;
                if(eh->event.button.button == SDL_BUTTON_LEFT) eh->mouseMiddle = 1;
                break;
            case SDL_MOUSEBUTTONUP:
                eh->mouseDown = 0;
                if(eh->event.button.button == SDL_BUTTON_LEFT) eh->mouseMiddle = 0;
                break;
            
            case SDL_MOUSEWHEEL:
                if (eh->event.wheel.y > 0) {
                    eh->zoom = 1; // Zoom in
                } 
                else if (eh->event.wheel.y < 0) {
                    eh->zoom = 2; // Zoom out
                }
                break;
            
            case SDL_MOUSEMOTION:
                eh->mouseMotionX = eh->event.motion.xrel;
                eh->mouseMotionY = eh->event.motion.yrel;
                break;
        }
    }
}

void toggleFullscreen(EventH *eh, WindowModel *wm) {
    eh->fullScreen = !eh->fullScreen;

    if (eh->fullScreen) {
        SDL_SetWindowFullscreen(wm->win, SDL_WINDOW_FULLSCREEN_DESKTOP);
        // SDL_ShowCursor(0);
    } else {
        SDL_SetWindowFullscreen(wm->win, 0);
        // SDL_ShowCursor(1);
    }

    int width, height;
    SDL_GetWindowSize(wm->win, &width, &height);
    glViewport(0, 0, width, height);
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
    glEnable(GL_DEPTH_TEST);

    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    return 1;
}