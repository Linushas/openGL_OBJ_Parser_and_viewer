#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <GL/glew.h>

typedef struct vertex {
    float x, y, z;
    float r, g, b;
    float nx, ny, nz;
} Vertex;

typedef struct mesh {
    Vertex *vertices;
    float *normals;
    unsigned int *indices;
    int vertexCount;
    int indexCount;

    unsigned int VAO, VBO, EBO;
} Mesh;

typedef struct cubeMesh {
    Vertex vertices[24];
    unsigned int indices[36];
    unsigned int VAO, VBO, EBO;
} CubeMesh;

typedef struct tetrahedronMesh {
    Vertex vertices[4];
    unsigned int indices[12];
    unsigned int VAO, VBO, EBO;
} TetrahedronMesh;

CubeMesh createCubeMesh(float x, float y, float z, float size);
TetrahedronMesh createTetrahedronMesh(float x, float y, float z);

void renderCube(CubeMesh* cube, int mode);
void renderTetrahedron(TetrahedronMesh* tetra, int mode);

CubeMesh createCubeMesh(float x, float y, float z, float size) {
    CubeMesh cube = {
        .vertices = {
            // Front face
            { x + 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f },
            { x - 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f },
            { x - 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f },
            { x + 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f },

            // Back face
            { x + 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, -1.0f },
            { x - 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, -1.0f },
            { x - 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, -1.0f },
            { x + 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, -1.0f },

            // Left face
            { x - 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   -1.0f, 0.0f, 0.0f },
            { x - 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   -1.0f, 0.0f, 0.0f },
            { x - 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   -1.0f, 0.0f, 0.0f },
            { x - 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   -1.0f, 0.0f, 0.0f },

            // Right face
            { x + 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f },
            { x + 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f },
            { x + 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f },
            { x + 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f },

            // Top face
            { x + 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f },
            { x - 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f },
            { x - 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f },
            { x + 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f },

            // Bottom face
            { x + 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, -1.0f, 0.0f },
            { x - 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, -1.0f, 0.0f },
            { x - 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, -1.0f, 0.0f },
            { x + 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, -1.0f, 0.0f },
        },
        .indices = {
            // Front face
            0, 1, 2, 2, 3, 0,
            // Back face
            4, 5, 6, 6, 7, 4,
            // Left face
            8, 9, 10, 10, 11, 8,
            // Right face
            12, 13, 14, 14, 15, 12,
            // Top face
            16, 17, 18, 18, 19, 16,
            // Bottom face
            20, 21, 22, 22, 23, 20,
        }
    };

    glGenVertexArrays(1, &cube.VAO);
    glGenBuffers(1, &cube.VBO);
    glGenBuffers(1, &cube.EBO);

    glBindVertexArray(cube.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, cube.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube.vertices), &cube.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube.indices), cube.indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    return cube;
}

void renderCube(CubeMesh* cube, int mode) {
    glBindVertexArray(cube->VAO);
    glDrawElements(mode, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

TetrahedronMesh createTetrahedronMesh(float x, float y, float z) {
    TetrahedronMesh tetra = {
        .vertices = {
                // Pos                          // Color
            {   x+ -0.5f,  y+ -0.5f, z+ -0.5f,      0.0f, 1.0f, 1.0f    },  
            {   x+  0.5f,  y+ -0.5f, z+ -0.5f,      0.0f, 1.0f, 0.0f    },  
            {   x+  0.0f,  y+ -0.5f, z+  0.5f,      0.0f, 0.0f, 1.0f    },  
            {   x+  0.0f,  y+  0.5f, z+  0.0f,      1.0f, 0.0f, 0.0f    }
        },
        .indices = {
            0, 1, 2, 0, 1, 3,
            1, 2, 3, 0, 3, 2
        }
    };

    glGenVertexArrays(1, &tetra.VAO);
    glGenBuffers(1, &tetra.VBO);
    glGenBuffers(1, &tetra.EBO);

    glBindVertexArray(tetra.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, tetra.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tetra.vertices), &tetra.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tetra.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tetra.indices), tetra.indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    return tetra;
}

void renderTetrahedron(TetrahedronMesh* tetra, int mode) {
    glBindVertexArray(tetra->VAO);
    glDrawElements(mode, 12, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

#endif