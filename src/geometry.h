#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <GL/glew.h>

typedef struct vertex {
    float x, y, z;
    float r, g, b;
    // float pos[3];
    // float color[3];
    // float normal[3];
} Vertex;

typedef struct mesh {
    Vertex *vertices;
    //float *normals;
    unsigned int *indices;
    int vertexCount;
    int indexCount;

    unsigned int VAO, VBO, EBO;
} Mesh;

typedef struct cubeMesh {
    Vertex vertices[8];
    unsigned int indices[36];
    unsigned int VAO, VBO, EBO;
} CubeMesh;

typedef struct tetrahedronMesh {
    Vertex vertices[4];
    unsigned int indices[12];
    unsigned int VAO, VBO, EBO;
} TetrahedronMesh;

/*
typedef struct pyramidMesh {
    // Vertex pos;
    Vertex vertices[8];
    unsigned int indices[36];
    unsigned int VAO, VBO, EBO;
} PyramidMesh;
*/

CubeMesh createCubeMesh(float x, float y, float z, float size);
TetrahedronMesh createTetrahedronMesh(float x, float y, float z);

void renderCube(CubeMesh* cube, int mode);
void renderTetrahedron(TetrahedronMesh* tetra, int mode);

CubeMesh createCubeMesh(float x, float y, float z, float size) {
    CubeMesh cube = {
        .vertices = {
                // Pos                          // Color
            {   x+-0.5f*size, y+-0.5f*size, z+-0.5f*size,      1.0f, 0.0f, 0.0f    },
            {   x+0.5f*size,  y+-0.5f*size, z+-0.5f*size,      0.0f, 1.0f, 0.0f    },  
            {   x+0.5f*size,  y+0.5f*size,  z+-0.5f*size,      0.0f, 0.0f, 1.0f    },  
            {   x+-0.5f*size, y+0.5f*size,  z+-0.5f*size,      1.0f, 1.0f, 0.0f    },
            {   x+-0.5f*size, y+-0.5f*size, z+0.5f*size,       1.0f, 0.0f, 1.0f    }, 
            {   x+0.5f*size,  y+-0.5f*size, z+0.5f*size,       0.0f, 1.0f, 1.0f    },  
            {   x+0.5f*size,  y+0.5f*size,  z+0.5f*size,       1.0f, 1.0f, 1.0f    },  
            {   x+-0.5f*size, y+0.5f*size,  z+0.5f*size,       0.0f, 0.0f, 0.0f    }  
        },
        .indices = {
            0, 1, 2, 2, 3, 0, // Back face
            4, 5, 6, 6, 7, 4, // Front face
            4, 0, 3, 3, 7, 4, // Left face
            1, 5, 6, 6, 2, 1, // Right face
            3, 2, 6, 6, 7, 3, // Top face
            4, 5, 1, 1, 0, 4  // Bottom face
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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