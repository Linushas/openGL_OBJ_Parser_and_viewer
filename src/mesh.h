#ifndef MESH_H
#define MESH_H

#define POS(x,y,z) (float[]){x,y,z}

#define OBJ_IXO_SPHERE "models/ixo.obj"
#define OBJ_MONKEY "models/monkey.obj"
#define OBJ_TORUS "models/torus.obj"
#define OBJ_CUBE "models/cube.obj"

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

    float pos[3];
    float color[3];
    float scale;
} Mesh;

Mesh parseOBJ(char* file, float *pos, char *color, float scale);
void setColor(Mesh *mesh, char *color);
void renderMesh(Mesh mesh, int mode);
void destroyMesh(Mesh *mesh);

#endif