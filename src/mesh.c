#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include "mesh.h"

Mesh parseOBJ(char* file, float *pos, char *color, float scale) {
    Mesh newMesh;
    newMesh.vertices = NULL;
    newMesh.indices = NULL;
    newMesh.vertexCount = 0;
    newMesh.indiceCount = 0;
    newMesh.scale = scale;
    newMesh.pos[0] = pos[0];
    newMesh.pos[1] = pos[1];
    newMesh.pos[2] = pos[2];
    
    setColor(&newMesh, color);

    FILE* fp = fopen(file, "r");
    if (!fp) {
        printf("Could not open file %s\n", file);
        return newMesh;
    }

    float vertices[3][100*100] = {0};
    float normals[3][100*100] = {0};
    int vertexCount = 0;
    int normalCount = 0;

    int vertexIndexArr[100*100] = {0};
    int normalIndexArr[100*100] = {0};
    int indiceCount = 0;
    int facesCount = 0;
    int skip = 0;

    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        if (line[0] == 'v' && line[1] == ' ') {
            float x, y, z;
            if (sscanf(line, "v %f %f %f", &x, &y, &z) == 3) {
                vertices[0][vertexCount] = (x * newMesh.scale) + newMesh.pos[0];
                vertices[1][vertexCount] = (y * newMesh.scale) + newMesh.pos[1];
                vertices[2][vertexCount] = (z * newMesh.scale) + newMesh.pos[2];
                vertexCount++;
            }
        }
        if (line[0] == 'v' && line[1] == 'n') {
            float nx, ny, nz;
            if (sscanf(line, "vn %f %f %f", &nx, &ny, &nz) == 3) {
                normals[0][normalCount] = nx;
                normals[1][normalCount] = ny;
                normals[2][normalCount] = nz;
                normalCount++;
            }
        }
        if (line[0] == 'f' && line[1] == ' ') {
            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
            &vertexIndexArr[indiceCount], &skip, &normalIndexArr[indiceCount],
            &vertexIndexArr[indiceCount+1], &skip, &normalIndexArr[indiceCount+1],
            &vertexIndexArr[indiceCount+2], &skip, &normalIndexArr[indiceCount+2]);
            
            indiceCount += 3;
            facesCount++;
        }
    }
    fclose(fp);

    newMesh.vertexCount = newMesh.indiceCount = indiceCount;
    newMesh.vertices = malloc(newMesh.vertexCount * sizeof(Vertex));
    newMesh.indices = malloc(newMesh.indiceCount * sizeof(unsigned int));

    for(int i = 0; i < indiceCount; i++) {
        newMesh.vertices[i].x = vertices[0][vertexIndexArr[i] -1];
        newMesh.vertices[i].y = vertices[1][vertexIndexArr[i] -1];
        newMesh.vertices[i].z = vertices[2][vertexIndexArr[i] -1];
        newMesh.vertices[i].nx = normals[0][normalIndexArr[i] -1];
        newMesh.vertices[i].ny = normals[1][normalIndexArr[i] -1];
        newMesh.vertices[i].nz = normals[2][normalIndexArr[i] -1];
        newMesh.vertices[i].r = newMesh.color[0];
        newMesh.vertices[i].g = newMesh.color[1];
        newMesh.vertices[i].b = newMesh.color[2];

        newMesh.indices[i] = i;
    }
    
    glGenVertexArrays(1, &newMesh.VAO);
    glGenBuffers(1, &newMesh.VBO);
    glGenBuffers(1, &newMesh.EBO);

    glBindVertexArray(newMesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, newMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, newMesh.vertexCount * sizeof(Vertex), newMesh.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newMesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, newMesh.indiceCount * sizeof(unsigned int), newMesh.indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);

    return newMesh;
}

void renderMesh(Mesh mesh, int mode) {
    glBindVertexArray(mesh.VAO);
    glDrawElements(mode, mesh.indiceCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void destroyMesh(Mesh *mesh) {
    glDeleteVertexArrays(1, &mesh->VAO);
    glDeleteBuffers(1, &mesh->VBO);
    glDeleteBuffers(1, &mesh->EBO);
    free(mesh->indices);
    free(mesh->vertices);
}

void setColor(Mesh *mesh, char *color) {
    if(strcmp(color, "red") == 0) {
        mesh->color[0] = 1.0f;
        mesh->color[1] = 0.0f;
        mesh->color[2] = 0.0f;
    }
    else if(strcmp(color, "green") == 0) {
        mesh->color[0] = 0.0f;
        mesh->color[1] = 1.0f;
        mesh->color[2] = 0.0f;
    }
    else if(strcmp(color, "blue") == 0) {
        mesh->color[0] = 0.0f;
        mesh->color[1] = 0.0f;
        mesh->color[2] = 1.0f;
    }
    else if(strcmp(color, "yellow") == 0) {
        mesh->color[0] = 1.0f;
        mesh->color[1] = 1.0f;
        mesh->color[2] = 0.0f;
    }
    else if(strcmp(color, "purple") == 0) {
        mesh->color[0] = 1.0f;
        mesh->color[1] = 0.0f;
        mesh->color[2] = 1.0f;
    }
    else if(strcmp(color, "cyan") == 0) {
        mesh->color[0] = 0.0f;
        mesh->color[1] = 1.0f;
        mesh->color[2] = 1.0f;
    }
    else if(strcmp(color, "white") == 0) {
        mesh->color[0] = 1.0f;
        mesh->color[1] = 1.0f;
        mesh->color[2] = 1.0f;
    }
    else if(strcmp(color, "black") == 0) {
        mesh->color[0] = 0.0f;
        mesh->color[1] = 0.0f;
        mesh->color[2] = 0.0f;
    }
    else if(strcmp(color, "grey") == 0) {
        mesh->color[0] = 0.5f;
        mesh->color[1] = 0.5f;
        mesh->color[2] = 0.5f;
    }
    else {
        mesh->color[0] = 0.5f;
        mesh->color[1] = 0.5;
        mesh->color[2] = 0.5f;
    }
}