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
    int w,a,s,d;
    int r;
    int zoom;
    float mouseMotionX, mouseMotionY;
    int mouseDown;
} EventH;

typedef struct windowModel {
    SDL_Window *win;
    SDL_GLContext glContext;
} WindowModel;

typedef struct vec3d {
    float x, y, z;
    float r, g, b;
} Vec3d;

typedef struct triangle {
    Vec3d p[3];
} Triangle;


typedef struct cubeMesh {
    Triangle tris[12];
} CubeMesh;


typedef struct mat4x4 {
    float m[4][4];
} Mat4x4;

void render(unsigned int shaderProgram, unsigned int VAO, EventH *eh);
void getWindowEvents(EventH *eh, WindowModel *wm);
void toggleFullscreen(EventH *eh, WindowModel *wm);
int initializeWindow(WindowModel *wm);
Vec3d normalize(Vec3d v);
Vec3d subtractVec3d(Vec3d v1, Vec3d v2);
float dotProduct(Vec3d v1, Vec3d v2);
Mat4x4 multiplyMatrices(Mat4x4 a, Mat4x4 b);
Vec3d multiplyMatrixVector(Mat4x4 mat, Vec3d vec);
Vec3d crossProduct(Vec3d v1, Vec3d v2);
void createPerspectiveProjection(Mat4x4 *mat, float fov, float aspect, float zNear, float zFar);
void createRotationMatrix(Mat4x4* model, float angleX, float angleY, float angleZ);
void lookAt(Mat4x4* view, Vec3d eye, Vec3d target, Vec3d up);
CubeMesh createCubeMesh();
void renderCube(unsigned int shaderProgram, unsigned int VAO, CubeMesh* cube);
void loadShaders(unsigned int *shaderProgram);
void setupMatrices(Mat4x4 *model, Mat4x4 *view, Mat4x4 *projection, unsigned int shaderProgram, Vec3d eye, Vec3d target, Vec3d up);

int main(int argc, char *argv[]) {
    WindowModel wm;
    if(!initializeWindow(&wm)) return -1;
    
    float cubeVertices[] = {
        // Position             // Color
        -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,     0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,     0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,     0.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,     1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 0.0f
    };

    unsigned int cubeIndices[] = {
        0, 1, 2, 2, 3, 0, // Back face
        4, 5, 6, 6, 7, 4, // Front face
        4, 0, 3, 3, 7, 4, // Left face
        1, 5, 6, 6, 2, 1, // Right face
        3, 2, 6, 6, 7, 3, // Top face
        4, 5, 1, 1, 0, 4  // Bottom face
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    unsigned int shaderProgram;
    loadShaders(&shaderProgram);

    Mat4x4 model = {0};
    Mat4x4 view = {0};
    Mat4x4 projection = {0};

    Vec3d eye = {0.0f, 0.0f, 4.0f};
    Vec3d target = {0.0f, 0.0f, 0.0f};
    Vec3d up = {0.0f, 1.0f, 0.0f};
    setupMatrices(&model, &view, &projection, shaderProgram, eye, target, up);


//-------------------------------------------------------------- MAIN LOOP 

    float angleX = 0.0f;
    float angleY = 0.0f;
    float angleZ = 0.0f;

    EventH eh = {.running = 1, .fullScreen = 0, .r = 0};
    while(eh.running) {
        getWindowEvents(&eh, &wm);

        if(eh.mouseDown == 1) {
            if(abs(eh.mouseMotionX) > 3) target.x  += eh.mouseMotionX * 0.01f;
            if(abs(eh.mouseMotionY) > 3) target.y += -eh.mouseMotionY * 0.01f;
        }

        if(eh.w) {
            eye.z -= 0.06f;
            target.z -= 0.06f;
        }
        if(eh.a) {
            eye.x -= 0.03f;
            target.x -= 0.03f;
        } 
        if(eh.s) {
            eye.z += 0.06f;
            target.z += 0.06f;
        } 
        if(eh.d) {
            eye.x += 0.03f;
            target.x += 0.03f;
        } 

        setupMatrices(&model, &view, &projection, shaderProgram, eye, target, up);
        
        createRotationMatrix(&model, angleX, angleY, angleZ);
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model.m[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view.m[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection.m[0][0]);

        render(shaderProgram, VAO, &eh);

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

void render(unsigned int shaderProgram, unsigned int VAO, EventH *eh) {
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    if(eh->r) glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    else glDrawElements(GL_LINE_LOOP, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void getWindowEvents(EventH *eh, WindowModel *wm) {
    eh->zoom = 0;
    while(SDL_PollEvent(&(eh->event))) {
        switch(eh->event.type) {
            case SDL_QUIT: 
                eh->running = 0; 
                break;
            case SDL_KEYDOWN: 
                if(eh->event.key.keysym.sym == SDLK_f) {
                    toggleFullscreen(eh, wm);
                }
                if(eh->event.key.keysym.sym == SDLK_w) {
                    eh->w = 1;
                }
                if(eh->event.key.keysym.sym == SDLK_a) {
                    eh->a = 1;
                }
                if(eh->event.key.keysym.sym == SDLK_s) {
                    eh->s = 1;
                }
                if(eh->event.key.keysym.sym == SDLK_d) {
                    eh->d = 1;
                }
                if(eh->event.key.keysym.sym == SDLK_r) {
                    eh->r = !eh->r;
                }
                break;
            case SDL_KEYUP:
                if(eh->event.key.keysym.sym == SDLK_w) {
                    eh->w = 0;
                }
                if(eh->event.key.keysym.sym == SDLK_a) {
                    eh->a = 0;
                }
                if(eh->event.key.keysym.sym == SDLK_s) {
                    eh->s = 0;
                }
                if(eh->event.key.keysym.sym == SDLK_d) {
                    eh->d = 0;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                eh->mouseDown = 1;
                break;
            case SDL_MOUSEBUTTONUP:
                eh->mouseDown = 0;
                break;
            case SDL_MOUSEWHEEL:
                if(eh->event.wheel.y > 0) // scroll up
                {
                    eh->zoom = 1;
                }
                else if(eh->event.wheel.y < 0) // scroll down
                {
                    eh->zoom = 2;
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
        SDL_ShowCursor(0);
    } else {
        SDL_SetWindowFullscreen(wm->win, 0);
        SDL_ShowCursor(1);
    }

    // Get the new window size
    int width, height;
    SDL_GetWindowSize(wm->win, &width, &height);

    // Update OpenGL viewport
    glViewport(0, 0, width, height);

    if (eh->fullScreen) {
        SDL_SetWindowFullscreen(wm->win, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_ShowCursor(0);
    } else {
        SDL_SetWindowFullscreen(wm->win, 0);
        SDL_ShowCursor(1);
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
    glEnable(GL_DEPTH_TEST);

    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    return 1;
}

Vec3d normalize(Vec3d v) {
    float magnitude = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    Vec3d normalized = { v.x / magnitude, v.y / magnitude, v.z / magnitude };
    return normalized;
}

Vec3d subtractVec3d(Vec3d v1, Vec3d v2) {
    Vec3d result = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
    return result;
}

float dotProduct(Vec3d v1, Vec3d v2) {
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

Vec3d multiplyMatrixVector(Mat4x4 mat, Vec3d vec) {
    Vec3d result = {
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

Vec3d crossProduct(Vec3d v1, Vec3d v2) {
    Vec3d cross = {
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

void lookAt(Mat4x4* view, Vec3d eye, Vec3d target, Vec3d up) {
    Vec3d zAxis = normalize(subtractVec3d(eye, target));
    Vec3d xAxis = normalize(crossProduct(up, zAxis));
    Vec3d yAxis = crossProduct(zAxis, xAxis);

    view->m[0][0] = xAxis.x; view->m[0][1] = yAxis.x; view->m[0][2] = zAxis.x; view->m[0][3] = 0.0f;
    view->m[1][0] = xAxis.y; view->m[1][1] = yAxis.y; view->m[1][2] = zAxis.y; view->m[1][3] = 0.0f;
    view->m[2][0] = xAxis.z; view->m[2][1] = yAxis.z; view->m[2][2] = zAxis.z; view->m[2][3] = 0.0f;
    view->m[3][0] = -dotProduct(xAxis, eye); 
    view->m[3][1] = -dotProduct(yAxis, eye); 
    view->m[3][2] = -dotProduct(zAxis, eye); 
    view->m[3][3] = 1.0f;
}

CubeMesh createCubeMesh() {
    CubeMesh cube = {
        .tris = {
            // Front face
            {-0.5, -0.5, 0.5, 1, 0, 0,      0.5, -0.5, 0.5, 1, 0, 0,    0.5, 0.5, 0.5, 1, 0, 0},
            {-0.5, -0.5, 0.5, 1, 0, 0,      0.5, 0.5, 0.5, 1, 0, 0,     -0.5, 0.5, 0.5, 1, 0, 0},
            // Back face
            {-0.5, -0.5, -0.5, 0, 1, 0,     0.5, -0.5, -0.5, 0, 1, 0,   0.5, 0.5, -0.5, 0, 1, 0},
            {-0.5, -0.5, -0.5, 0, 1, 0,     0.5, 0.5, -0.5, 0, 1, 0,    -0.5, 0.5, -0.5, 0, 1, 0},
            // Left face
            {-0.5, -0.5, -0.5, 0, 0, 1,     -0.5, 0.5, -0.5, 0, 0, 1,   -0.5, 0.5, 0.5, 0, 0, 1},
            {-0.5, -0.5, -0.5, 0, 0, 1,     -0.5, 0.5, 0.5, 0, 0, 1,    -0.5, -0.5, 0.5, 0, 0, 1},
            // Right face
            {0.5, -0.5, -0.5, 1, 1, 0,      0.5, 0.5, -0.5, 1, 1, 0,    0.5, 0.5, 0.5, 1, 1, 0},
            {0.5, -0.5, -0.5, 1, 1, 0,      0.5, 0.5, 0.5, 1, 1, 0,     0.5, -0.5, 0.5, 1, 1, 0},
            // Top face
            {-0.5, 0.5, -0.5, 0, 1, 1,      0.5, 0.5, -0.5, 0, 1, 1,    0.5, 0.5, 0.5, 0, 1, 1},
            {-0.5, 0.5, -0.5, 0, 1, 1,      0.5, 0.5, 0.5, 0, 1, 1,     -0.5, 0.5, 0.5, 0, 1, 1},
            // Bottom face
            {-0.5, -0.5, -0.5, 1, 0, 1,     0.5, -0.5, -0.5, 1, 0, 1,   0.5, -0.5, 0.5, 1, 0, 1},
            {-0.5, -0.5, -0.5, 1, 0, 1,     0.5, -0.5, 0.5, 1, 0, 1,    -0.5, -0.5, 0.5, 1, 0, 1}
        }
    };
    return cube;
}

void renderCube(unsigned int shaderProgram, unsigned int VAO, CubeMesh* cube) {
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Enable depth buffer

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    for (int i = 0; i < 12; i++) {
        glDrawArrays(GL_TRIANGLES, i * 3, 3);
    }

    glBindVertexArray(0);
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

void setupMatrices(Mat4x4 *model, Mat4x4 *view, Mat4x4 *projection, unsigned int shaderProgram, Vec3d eye, Vec3d target, Vec3d up) {
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