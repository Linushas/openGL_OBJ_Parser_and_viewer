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
#include <vector>
using namespace std;

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

typedef struct vec3d {
    float x, y, z;
} Vec3d;

typedef struct triangle {
    Vec3d p[3];
} Triangle;

typedef struct mesh {
    vector<Triangle> tris;
} Mesh;

typedef struct mat4x4 {
    float m[4][4] = { 0 };
} Mat4x4;

void render();
void getWindowEvents(EventH *eh, SDL_Window *win);
void toggleFullscreen(EventH *eh, SDL_Window *win);

void MultiplyMatrixVector(Vec3d &i, Vec3d &o, Mat4x4 &m) {
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

    if(w != 0.0f) {
        0.x /= w; o.y /= w; o.z /= w;
    }
}

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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

    EventH eh = {.running = 1, .fullScreen = 0};

    Mesh meshCube;
    Mat4x4 matProj;

    meshCube.tris = {
        // SOUTH
        { 0.0f, 0.0f, 0.0f,     0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 0.0f},
        { 0.0f, 0.0f, 0.0f,     1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f},

        // EAST
        { 1.0f, 0.0f, 0.0f,     1.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f},
        { 1.0f, 0.0f, 0.0f,     1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 1.0f},

        // NORTH
        { 1.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 1.0f},
        { 1.0f, 0.0f, 1.0f,     0.0f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f},

        // WEST
        { 0.0f, 0.0f, 1.0f,     0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f},
        { 0.0f, 0.0f, 1.0f,     0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 0.0f},

        // TOP
        { 0.0f, 1.0f, 0.0f,     0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f},
        { 0.0f, 1.0f, 0.0f,     1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f},

        // BOTTOM
        { 1.0f, 0.0f, 1.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f},
        { 1.0f, 0.0f, 1.0f,     0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f},
    };

    // projection matrix
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0;
    float fAspectRatio = (float)aspectRatio;
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * M_PI);

    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][3] = 0.0f;

    // MAIN LOOP
    while(eh.running) {
        getWindowEvents(&eh, win);
        render();

        for(auto tri : meshCube.tris) {
            Triangle triProjected, triTranslated;

            triTranslated = tri;
            triTranslated.p[0].z = tri.p[0].z + 3.0f;
            triTranslated.p[1].z = tri.p[1].z + 3.0f;
            triTranslated.p[2].z = tri.p[2].z + 3.0f;

            MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
            MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
            MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

            // scale to view
            triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

            triProjected.p[0].x *= 0.5f * (float)SW; triProjected.p[0].y *= 0.5f * (float)SH;
            triProjected.p[1].x *= 0.5f * (float)SW; triProjected.p[1].y *= 0.5f * (float)SH;
            triProjected.p[2].x *= 0.5f * (float)SW; triProjected.p[2].y *= 0.5f * (float)SH;

            DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
                         triProjected.p[1].x, triProjected.p[1].y,
                         triProjected.p[2].x, triProjected.p[2].y);
        }

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