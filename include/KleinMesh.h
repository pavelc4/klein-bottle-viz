#ifndef KLEIN_MESH_H
#define KLEIN_MESH_H

#include "KleinMath.h"

void KleinMesh_InitLighting();
void KleinMesh_Render(float bottleRadius, int resU, int resV);
void KleinMesh_RenderNormals(float bottleRadius, int resU, int resV, float lineLen);

void KleinMesh_ComputeNormal(float u, float v, float radius, float& nx, float& ny, float& nz);
Point3D KleinMesh_Eval(float u, float v, float radius);

void KleinMesh_SetColor(int index);
int  KleinMesh_GetColorIndex();

void KleinMesh_SetClipPlane(float x);
float KleinMesh_GetClipPlane();
void KleinMesh_EnableClip(bool on);
bool KleinMesh_IsClipEnabled();

void KleinMesh_ExportOBJ(const char* filename, float bottleRadius, int resU, int resV);

#endif
