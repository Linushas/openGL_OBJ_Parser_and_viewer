#include <stdio.h>
#include "math3d.h"

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