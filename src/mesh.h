#ifndef MESH_H
#define MESH_H

#include "geometry.h"

Mesh parseOBJ(char* file);
void renderMesh(Mesh *mesh, int mode);

#endif