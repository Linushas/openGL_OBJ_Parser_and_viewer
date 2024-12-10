#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <mesh.h>

Mesh parseOBJ(char* file) {
    Mesh newMesh;
    FILE* filePtr = fopen(file, "r");
    
    newMesh.vertexCount = 24;
    newMesh.vertices = malloc(newMesh.vertexCount * sizeof(Vertex));



    fclose(filePtr);

    glGenVertexArrays(1, &newMesh.VAO);
    glGenBuffers(1, &newMesh.VBO);
    glGenBuffers(1, &newMesh.EBO);

    glBindVertexArray(newMesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, newMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(newMesh.vertices), &newMesh.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newMesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(newMesh.indices), newMesh.indices, GL_STATIC_DRAW);

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

void renderMesh(Mesh *mesh, int mode) {
    glBindVertexArray(mesh->VAO);
    glDrawElements(mode, mesh->indecesCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}