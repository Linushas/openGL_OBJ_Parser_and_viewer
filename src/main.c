#include <stdio.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "shader.h"
#include "mesh.h"
#include "math3d.h"

typedef struct eventHandler
{
    SDL_Event event;
    int running;
    int fullScreen;
    int r, n;
    int w, a, s, d;
    int zoom;
    float mouseMotionX, mouseMotionY;
    int mouseDown;
    int mouseMiddle;
    int shift;
} EventH;

typedef struct camera {
    Mat4x4 model, view, projection;
    Vertex eye, target, up;
    float angleX, angleY, angleZ;
} Camera;

typedef struct windowModel
{
    SDL_Window *win;
    SDL_GLContext glContext;
    EventH *eh;
    Camera *cam;
    unsigned int shaderProgram;
} WindowModel;

void render(unsigned int shaderProgram, EventH *eh, Mesh *mesh, int meshCount);
void getWindowEvents(WindowModel *wm, Vertex *eye, Vertex *target, float *angleX, float *angleY);
void toggleFullscreen(WindowModel *wm);
int initializeWindow(WindowModel *wm);

Camera setupCamera() {
    Mat4x4 model = {0}, view = {0}, projection = {0};
    Vertex eye = {0.0f, 0.0f, 4.0f};
    Vertex target = {0.0f, 0.0f, 0.0f};
    Vertex up = {0.0f, 1.0f, 0.0f};
    float angleX = 0.0f, angleY = 0.0f, angleZ = 0.0f;

    Camera cam = {
        .model = model, .view = view, .projection = projection,
        .eye = eye, .target = target, .up = up, 
        .angleX = angleX, .angleY = angleY, .angleZ = angleZ
    };
    return cam;
}

int main(int argc, char *argv[])
{   
    WindowModel wm;
    if (!initializeWindow(&wm))
        return -1;
    EventH eh = {.running = 1, .fullScreen = 0, .r = 0, .n = 0};
    Camera cam = setupCamera();
    wm.eh = &eh;
    wm.cam = &cam;

    setupMatrices(&cam.model, &cam.view, &cam.projection, wm.shaderProgram, cam.eye, cam.target, cam.up);

    Mesh meshes[10];
    int meshCount = 3;
    meshes[0] = parseOBJ(OBJ_IXO_SPHERE, POS(0.0f, 0.0f, 0.0f), "red", 0.5f);
    meshes[1] = parseOBJ(OBJ_MONKEY, POS(2.0f, 0.0f, 0.0f), "yellow", 1.0f);
    meshes[2] = parseOBJ("models/Helicopter.obj", POS(-2.0f, 0.0f, 0.0f), "cyan", 1.0f);

    loadShaders(&wm.shaderProgram);
    
    while (wm.eh->running)
    {
        getWindowEvents(&wm, &cam.eye, &cam.target, &cam.angleX, &cam.angleY);

        glUniform3f(glGetUniformLocation(wm.shaderProgram, "lightPos"), 6.0f, 2.0f, 6.0f);
        glUniform3f(glGetUniformLocation(wm.shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(wm.shaderProgram, "objectColor"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(wm.shaderProgram, "viewPos"), cam.eye.x, cam.eye.y, cam.eye.z);

        setupMatrices(&cam.model, &cam.view, &cam.projection, wm.shaderProgram, cam.eye, cam.target, cam.up);
        createRotationMatrix(&cam.model, cam.angleX, cam.angleY, cam.angleZ);

        unsigned int modelLoc = glGetUniformLocation(wm.shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(wm.shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(wm.shaderProgram, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &cam.model.m[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &cam.view.m[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &cam.projection.m[0][0]);

        render(wm.shaderProgram, wm.eh, meshes, meshCount);
        SDL_GL_SwapWindow(wm.win);
    }

    for (int i = 0; i < meshCount; i++)
    {
        destroyMesh(&meshes[i]);
    }

    glDeleteProgram(wm.shaderProgram);

    SDL_GL_DeleteContext(wm.glContext);
    SDL_DestroyWindow(wm.win);
    SDL_Quit();

    return 0;
}

void render(unsigned int shaderProgram, EventH *eh, Mesh *mesh, int meshCount)
{
    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    for (int i = 0; i < meshCount; i++)
    {
        if (eh->r)
        {
            renderMesh(mesh[i], GL_TRIANGLES);
        }
        else
        {
            renderMesh(mesh[i], GL_LINE_LOOP);
        }
    }
    glBindVertexArray(0);
}

void getWindowEvents(WindowModel *wm, Vertex *eye, Vertex *target, float *angleX, float *angleY)
{
    wm->eh->zoom = 0;
    wm->eh->mouseMotionX = 0;
    wm->eh->mouseMotionY = 0;

    while (SDL_PollEvent(&(wm->eh->event)))
    {
        switch (wm->eh->event.type)
        {
        case SDL_QUIT:
            wm->eh->running = 0;
            break;

        case SDL_KEYDOWN:
            if (wm->eh->event.key.keysym.sym == SDLK_F11)
            {
                toggleFullscreen(wm);
            }
            if (wm->eh->event.key.keysym.sym == SDLK_r)
            {
                wm->eh->r = !wm->eh->r;
            }
            if (wm->eh->event.key.keysym.sym == SDLK_n)
            {
                wm->eh->n = (wm->eh->n + 1) % 4;
            }
            if (wm->eh->event.key.keysym.sym == SDLK_LSHIFT)
            {
                wm->eh->shift = 1;
            }
            break;
        case SDL_KEYUP:
            if (wm->eh->event.key.keysym.sym == SDLK_LSHIFT)
            {
                wm->eh->shift = 0;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            wm->eh->mouseDown = 1;
            if (wm->eh->event.button.button == SDL_BUTTON_LEFT)
                wm->eh->mouseMiddle = 1;
            break;
        case SDL_MOUSEBUTTONUP:
            wm->eh->mouseDown = 0;
            if (wm->eh->event.button.button == SDL_BUTTON_LEFT)
                wm->eh->mouseMiddle = 0;
            break;

        case SDL_MOUSEWHEEL:
            if (wm->eh->event.wheel.y > 0)
            {
                wm->eh->zoom = 1; // Zoom in
            }
            else if (wm->eh->event.wheel.y < 0)
            {
                wm->eh->zoom = 2; // Zoom out
            }
            break;

        case SDL_MOUSEMOTION:
            wm->eh->mouseMotionX = wm->eh->event.motion.xrel;
            wm->eh->mouseMotionY = wm->eh->event.motion.yrel;
            break;
        }
    }

    if (wm->eh->mouseDown)
    {
        if (wm->eh->mouseMiddle)
        {
            if (wm->eh->shift)
            {
                // Pan
                eye->x += -wm->eh->mouseMotionX * sensitivity;
                eye->y += wm->eh->mouseMotionY * sensitivity * 1.3f;

                target->x += -wm->eh->mouseMotionX * sensitivity;
                target->y += wm->eh->mouseMotionY * sensitivity * 1.3f;
            }
            else
            {
                // Orbit
                *angleX += -wm->eh->mouseMotionY * sensitivity;
                *angleY += -wm->eh->mouseMotionX * sensitivity;
            }
        }
    }

    if (wm->eh->zoom == 1)
    {
        eye->z -= 0.2f;
    }
    else if (wm->eh->zoom == 2)
    {
        eye->z += 0.2f;
    }
}

void toggleFullscreen(WindowModel *wm)
{
    wm->eh->fullScreen = !wm->eh->fullScreen;

    if (wm->eh->fullScreen)
    {
        SDL_SetWindowFullscreen(wm->win, SDL_WINDOW_FULLSCREEN_DESKTOP);
        // SDL_ShowCursor(0);
    }
    else
    {
        SDL_SetWindowFullscreen(wm->win, 0);
        // SDL_ShowCursor(1);
    }

    int width, height;
    SDL_GetWindowSize(wm->win, &width, &height);
    glViewport(0, 0, width, height);
}

int initializeWindow(WindowModel *wm)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
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
    if (!wm->glContext)
    {
        printf("Failed to create OpenGL context: %s\n", SDL_GetError());
        SDL_DestroyWindow(wm->win);
        SDL_Quit();
        return 0;
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
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