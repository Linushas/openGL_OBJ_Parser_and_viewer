/*
 * 3D engine made with SDL2 and C language
 * Author: Linus Hasselkvist
 * 2024-12-07
 * 
 * */

#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "shader.h"
#include "mesh.h"

#define res         4               // 0=160*X 1=360*X 4=640*X ...
#define aspectRatio (16.0f / 9.0f)
#define SH          160*res
#define SW          SH*aspectRatio
#define SH2         SH/2
#define SW2         SW/2
#define FPS         60  
#define sensitivity 0.01f
//-----------------------------------------------------------------------
typedef struct eventHandler {
    SDL_Event event;
    int running;
    int fullScreen;
    int r;
    int zoom;
    float mouseMotionX, mouseMotionY;
    int mouseDown;
    int mouseMiddle;
    int shift;
} EventH;

typedef struct windowModel {
    SDL_Window *win;
    SDL_GLContext glContext;
} WindowModel;

typedef struct mat4x4 {
    float m[4][4];
} Mat4x4;

void render(unsigned int shaderProgram, EventH *eh, Mesh *mesh);
void getWindowEvents(EventH *eh, WindowModel *wm);
void toggleFullscreen(EventH *eh, WindowModel *wm);
int initializeWindow(WindowModel *wm);

Vertex normalize(Vertex v);
Vertex subtractVec3d(Vertex v1, Vertex v2);
float dotProduct(Vertex v1, Vertex v2);
Mat4x4 multiplyMatrices(Mat4x4 a, Mat4x4 b);
Vertex multiplyMatrixVector(Mat4x4 mat, Vertex vec);
Vertex crossProduct(Vertex v1, Vertex v2);

void setupMatrices(Mat4x4 *model, Mat4x4 *view, Mat4x4 *projection, unsigned int shaderProgram, Vertex eye, Vertex target, Vertex up);
void createPerspectiveProjection(Mat4x4 *mat, float fov, float aspect, float zNear, float zFar);
void createRotationMatrix(Mat4x4* model, float angleX, float angleY, float angleZ);
void lookAt(Mat4x4* view, Vertex eye, Vertex target, Vertex up);

void loadShaders(unsigned int *shaderProgram);

int main(int argc, char *argv[]) {
    WindowModel wm;
    if(!initializeWindow(&wm)) return -1;


    Mesh newMesh = parseOBJ("models/cube.obj");
    printf("\nvertices:\n");
    for(int m = 0; m < newMesh.vertexCount; m++)
        printf("%f %f %f, %f %f %f, %f %f %f\n", newMesh.vertices[m].x, newMesh.vertices[m].y, newMesh.vertices[m].z, newMesh.vertices[m].r, newMesh.vertices[m].g, newMesh.vertices[m].b, newMesh.vertices[m].nx, newMesh.vertices[m].ny, newMesh.vertices[m].nz);
    printf("\nindices:\n");
    for(int m = 0; m < newMesh.indiceCount; m++){
        if(m % 3 == 0) printf("\t");
        if(m % 6 == 0) printf("\n");
        printf("%d, ", newMesh.indices[m]);
    }
        



    unsigned int shaderProgram;
    loadShaders(&shaderProgram);

    Mat4x4 model = {0}, view = {0}, projection = {0};
    
    Vertex eye = {0.0f, 0.0f, 4.0f};
    Vertex target = {0.0f, 0.0f, 0.0f};
    Vertex up = {0.0f, 1.0f, 0.0f};
    setupMatrices(&model, &view, &projection, shaderProgram, eye, target, up);

    float angleX = 0.0f, angleY = 0.0f, angleZ = 0.0f;

    EventH eh = {.running = 1, .fullScreen = 0, .r = 0};

//-------------------------------------------------------------- MAIN LOOP 
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

        render(shaderProgram, &eh, &newMesh);

        SDL_GL_SwapWindow(wm.win);
    }

    // glDeleteVertexArrays(1, &cube2.VAO);
    // glDeleteBuffers(1, &cube2.VBO);
    // glDeleteVertexArrays(1, &tetra1.VAO);
    // glDeleteBuffers(1, &tetra1.VBO);
    destroyMesh(&newMesh);

    glDeleteProgram(shaderProgram);
    
    SDL_GL_DeleteContext(wm.glContext);
    SDL_DestroyWindow(wm.win);
    SDL_Quit();

    return 0;
}

void render(unsigned int shaderProgram, EventH *eh, Mesh *mesh) {
    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    if(eh->r) {
        renderMesh(*mesh, GL_TRIANGLES);
    } 
    else {
        renderMesh(*mesh, GL_LINE_LOOP);
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

Vertex normalize(Vertex v) {
    float magnitude = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    Vertex normalized = { v.x / magnitude, v.y / magnitude, v.z / magnitude };
    return normalized;
}

Vertex subtractVec3d(Vertex v1, Vertex v2) {
    Vertex result = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
    return result;
}

float dotProduct(Vertex v1, Vertex v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Mat4x4 multiplyMatrices(Mat4x4 a, Mat4x4 b) {
    Mat4x4 result = {0};
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            result.m[row][col] = a.m[row][0] * b.m[0][col] +
                                 a.m[row][1] * b.m[1][col] +
                                 a.m[row][2] * b.m[2][col] +
                                 a.m[row][3] * b.m[3][col];
        }
    }
    return result;
}

Vertex multiplyMatrixVector(Mat4x4 mat, Vertex vec) {
    Vertex result = {
        .x = mat.m[0][0] * vec.x + mat.m[0][1] * vec.y + mat.m[0][2] * vec.z + mat.m[0][3],
        .y = mat.m[1][0] * vec.x + mat.m[1][1] * vec.y + mat.m[1][2] * vec.z + mat.m[1][3],
        .z = mat.m[2][0] * vec.x + mat.m[2][1] * vec.y + mat.m[2][2] * vec.z + mat.m[2][3]
    };
    float w = mat.m[3][0] * vec.x + mat.m[3][1] * vec.y + mat.m[3][2] * vec.z + mat.m[3][3];
    if (w != 0.0f) {
        result.x /= w;
        result.y /= w;
        result.z /= w;
    }
    return result;
}

Vertex crossProduct(Vertex v1, Vertex v2) {
    Vertex cross = {
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
    return cross;
}

void createPerspectiveProjection(Mat4x4 *mat, float fov, float aspect, float zNear, float zFar) {
    float tanHalfFov = tanf(fov / 2.0f);
    mat->m[0][0] = 1.0f / (aspect * tanHalfFov);
    mat->m[1][1] = 1.0f / tanHalfFov;
    mat->m[2][2] = -(zFar + zNear) / (zFar - zNear);
    mat->m[2][3] = -1.0f;
    mat->m[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
    mat->m[3][3] = 0.0f;
}

void createRotationMatrix(Mat4x4* model, float angleX, float angleY, float angleZ) {
    float cosX = cosf(angleX), sinX = sinf(angleX);
    float cosY = cosf(angleY), sinY = sinf(angleY);
    float cosZ = cosf(angleZ), sinZ = sinf(angleZ);

    model->m[0][0] = cosY * cosZ;
    model->m[0][1] = -cosY * sinZ;
    model->m[0][2] = sinY;
    model->m[1][0] = sinX * sinY * cosZ + cosX * sinZ;
    model->m[1][1] = -sinX * sinY * sinZ + cosX * cosZ;
    model->m[1][2] = -sinX * cosY;
    model->m[2][0] = -cosX * sinY * cosZ + sinX * sinZ;
    model->m[2][1] = cosX * sinY * sinZ + sinX * cosZ;
    model->m[2][2] = cosX * cosY;
    model->m[3][3] = 1.0f;
}

void lookAt(Mat4x4* view, Vertex eye, Vertex target, Vertex up) {
    Vertex zAxis = normalize(subtractVec3d(eye, target));
    Vertex xAxis = normalize(crossProduct(up, zAxis));
    Vertex yAxis = crossProduct(zAxis, xAxis);

    view->m[0][0] = xAxis.x; view->m[0][1] = yAxis.x; view->m[0][2] = zAxis.x; view->m[0][3] = 0.0f;
    view->m[1][0] = xAxis.y; view->m[1][1] = yAxis.y; view->m[1][2] = zAxis.y; view->m[1][3] = 0.0f;
    view->m[2][0] = xAxis.z; view->m[2][1] = yAxis.z; view->m[2][2] = zAxis.z; view->m[2][3] = 0.0f;
    view->m[3][0] = -dotProduct(xAxis, eye); 
    view->m[3][1] = -dotProduct(yAxis, eye); 
    view->m[3][2] = -dotProduct(zAxis, eye); 
    view->m[3][3] = 1.0f;
}

void loadShaders(unsigned int *shaderProgram) {
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
    *shaderProgram = glCreateProgram();
    glAttachShader(*shaderProgram, vertexShader);
    glAttachShader(*shaderProgram, fragmentShader);
    glLinkProgram(*shaderProgram);

    // Check for linking errors
    glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
        printf("Shader Program Linking Failed:\n%s\n", infoLog);
    }

    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void setupMatrices(Mat4x4 *model, Mat4x4 *view, Mat4x4 *projection, unsigned int shaderProgram, Vertex eye, Vertex target, Vertex up) {
    // Setup matrices (e.g., create rotation, translation, and projection)
    createPerspectiveProjection(projection, M_PI / 4.0f, aspectRatio, 0.1f, 1000.0f);

    lookAt(view, eye, target, up);
    
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");

    // Send matrices to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model->m[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view->m[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection->m[0][0]);
}