#ifndef MATH3D_H
#define MATH3D_H

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <math.h>
#include "mesh.h"

#define res         4               // 0=160*X 1=360*X 4=640*X ...
#define aspectRatio (16.0f / 9.0f)
#define SH          160*res
#define SW          SH*aspectRatio
#define SH2         SH/2
#define SW2         SW/2
#define FPS         60  
#define sensitivity 0.01f

typedef struct vec3d {
    float x, y, z;
} Vec3d;

typedef struct mat4x4 {
    float m[4][4];
} Mat4x4;

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

#endif