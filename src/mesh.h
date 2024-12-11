#ifndef MESH_H
#define MESH_H

typedef struct vertex {
    float x, y, z;
    float r, g, b;
    float nx, ny, nz;
} Vertex;

typedef struct mesh {
    Vertex *vertices;
    unsigned int *indices;
    int vertexCount, indiceCount;
    unsigned int VAO, VBO, EBO;

    // float pos[3];
    // float color[3];
    // float scale;
} Mesh;

Mesh parseOBJ(char* file);
void renderMesh(Mesh mesh, int mode);
void destroyMesh(Mesh *mesh);

#endif