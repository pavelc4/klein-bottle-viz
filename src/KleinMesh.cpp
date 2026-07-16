#include "KleinMesh.h"
#include "KleinMath.h"
#include <GL/gl.h>
#include <cmath>
#include <cstdio>

static const float PI = 3.14159265f;

static int colorScheme = 0;
static const int COLOR_SCHEME_COUNT = 5;

static bool clipEnabled = false;
static float clipX = 0.0f;

static void HSVtoRGB(float h, float s, float v, float& r, float& g, float& b) {
    float c = v * s;
    float x = c * (1.0f - std::fabs(std::fmod(h * 6.0f, 2.0f) - 1.0f));
    float m = v - c;
    if      (h < 1.0f/6) { r = c; g = x; b = 0; }
    else if (h < 2.0f/6) { r = x; g = c; b = 0; }
    else if (h < 3.0f/6) { r = 0; g = c; b = x; }
    else if (h < 4.0f/6) { r = 0; g = x; b = c; }
    else if (h < 5.0f/6) { r = x; g = 0; b = c; }
    else                 { r = c; g = 0; b = x; }
    r += m; g += m; b += m;
}

static void GetColor(float u, float v, float& r, float& g, float& b) {
    float normU = u / (2.0f * PI);
    switch (colorScheme) {
        case 0: {
            float sat = 0.6f + 0.3f * std::sin(v * 2.0f);
            float val = 0.75f + 0.25f * std::cos(v * 3.0f);
            HSVtoRGB(normU, sat, val, r, g, b);
            break;
        }
        case 1: {
            r = 0.1f + 0.15f * std::sin(normU * 6.28f);
            g = 0.5f + 0.3f  * std::sin(normU * 6.28f + 1.0f);
            b = 0.6f + 0.3f  * std::sin(normU * 6.28f + 2.0f);
            break;
        }
        case 2: { HSVtoRGB(0.05f + 0.12f * normU, 0.8f, 0.85f, r, g, b); break; }
        case 3: {
            float lum = 0.45f + 0.35f * std::sin(normU * 6.28f + v / (2.0f*PI) * 6.28f);
            r = g = b = lum;
            break;
        }
        case 4: {
            r = 0.1f + 0.9f * std::pow(std::sin(normU * 3.14f), 2.0f);
            g = 0.1f + 0.9f * std::pow(std::sin(normU * 3.14f + 2.09f), 2.0f);
            b = 0.1f + 0.9f * std::pow(std::sin(normU * 3.14f + 4.18f), 2.0f);
            break;
        }
    }
}

int KleinMesh_GetColorIndex() { return colorScheme; }
void KleinMesh_SetColor(int index) {
    if (index >= 0 && index < COLOR_SCHEME_COUNT) colorScheme = index;
}

void KleinMesh_SetClipPlane(float x)  { clipX = x; }
float KleinMesh_GetClipPlane()        { return clipX; }
void  KleinMesh_EnableClip(bool on)   { clipEnabled = on; }
bool  KleinMesh_IsClipEnabled()       { return clipEnabled; }

Point3D KleinMesh_Eval(float u, float v, float radius) {
    return ComputeKlein(u, v, radius);
}

void KleinMesh_ComputeNormal(float u, float v, float radius, float& nx, float& ny, float& nz) {
    const float eps = 0.001f;
    Point3D pu0 = ComputeKlein(u - eps, v, radius);
    Point3D pu1 = ComputeKlein(u + eps, v, radius);
    Point3D pv0 = ComputeKlein(u, v - eps, radius);
    Point3D pv1 = ComputeKlein(u, v + eps, radius);

    float duX = pu1.x - pu0.x, duY = pu1.y - pu0.y, duZ = pu1.z - pu0.z;
    float dvX = pv1.x - pv0.x, dvY = pv1.y - pv0.y, dvZ = pv1.z - pv0.z;

    nx = duY * dvZ - duZ * dvY;
    ny = duZ * dvX - duX * dvZ;
    nz = duX * dvY - duY * dvX;

    float len = std::sqrt(nx * nx + ny * ny + nz * nz);
    if (len > 0.0001f) { nx /= len; ny /= len; nz /= len; }
}

void KleinMesh_InitLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    GLfloat lightPos[]  = { 5.0f, 8.0f, 10.0f, 1.0f };
    GLfloat lightAmb[]  = { 0.15f, 0.15f, 0.15f, 1.0f };
    GLfloat lightDiff[] = { 0.9f, 0.85f, 0.8f, 1.0f };
    GLfloat lightSpec[] = { 0.5f, 0.5f, 0.5f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);

    GLfloat matSpec[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30.0f);
}

void KleinMesh_Render(float bottleRadius, int resU, int resV) {
    if (clipEnabled) {
        glEnable(GL_CLIP_PLANE0);
        double eqn[4] = { -1.0, 0.0, 0.0, clipX };
        glClipPlane(GL_CLIP_PLANE0, eqn);
    }

    for (int i = 0; i < resU; ++i) {
        glBegin(GL_TRIANGLE_STRIP);
        float u0 = (static_cast<float>(i) / resU) * 2.0f * PI;
        float u1 = (static_cast<float>(i + 1) / resU) * 2.0f * PI;

        for (int j = 0; j <= resV; ++j) {
            float v = (static_cast<float>(j) / resV) * 2.0f * PI;
            float nx, ny, nz;

            KleinMesh_ComputeNormal(u0, v, bottleRadius, nx, ny, nz);
            glNormal3f(nx, ny, nz);
            float r0, g0, b0;
            GetColor(u0, v, r0, g0, b0);
            glColor3f(r0, g0, b0);
            Point3D p0 = KleinMesh_Eval(u0, v, bottleRadius);
            glVertex3f(p0.x, p0.y, p0.z);

            KleinMesh_ComputeNormal(u1, v, bottleRadius, nx, ny, nz);
            glNormal3f(nx, ny, nz);
            float r1, g1, b1;
            GetColor(u1, v, r1, g1, b1);
            glColor3f(r1, g1, b1);
            Point3D p1 = KleinMesh_Eval(u1, v, bottleRadius);
            glVertex3f(p1.x, p1.y, p1.z);
        }
        glEnd();
    }

    if (clipEnabled) glDisable(GL_CLIP_PLANE0);
}

void KleinMesh_RenderNormals(float bottleRadius, int resU, int resV, float lineLen) {
    glDisable(GL_LIGHTING);
    glLineWidth(1.0f);
    glColor3f(1.0f, 1.0f, 0.0f);

    int stepU = 4, stepV = 4;
    glBegin(GL_LINES);
    for (int i = 0; i < resU; i += stepU) {
        for (int j = 0; j < resV; j += stepV) {
            float u = (static_cast<float>(i) / resU) * 2.0f * PI;
            float v = (static_cast<float>(j) / resV) * 2.0f * PI;

            Point3D p = KleinMesh_Eval(u, v, bottleRadius);
            float nx, ny, nz;
            KleinMesh_ComputeNormal(u, v, bottleRadius, nx, ny, nz);

            glVertex3f(p.x, p.y, p.z);
            glVertex3f(p.x + nx * lineLen, p.y + ny * lineLen, p.z + nz * lineLen);
        }
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void KleinMesh_ExportOBJ(const char* filename, float bottleRadius, int resU, int resV) {
    FILE* f = std::fopen(filename, "w");
    if (!f) { std::printf("Failed to open %s\n", filename); return; }

    std::fprintf(f, "# Klein Bottle - Exported from klein-bottle-viz\n");
    std::fprintf(f, "# Vertices: %d  Faces: %d\n", (resU + 1) * (resV + 1), resU * resV * 2);

    for (int i = 0; i <= resU; ++i) {
        float u = (static_cast<float>(i) / resU) * 2.0f * PI;
        for (int j = 0; j <= resV; ++j) {
            float v = (static_cast<float>(j) / resV) * 2.0f * PI;
            Point3D p = KleinMesh_Eval(u, v, bottleRadius);
            std::fprintf(f, "v %f %f %f\n", p.x, p.y, p.z);
        }
    }

    for (int i = 0; i < resU; ++i) {
        for (int j = 0; j < resV; ++j) {
            int a = i * (resV + 1) + j + 1;
            int b = a + resV + 1;
            int c = a + 1;
            int d = b + 1;
            std::fprintf(f, "f %d %d %d\n", a, b, c);
            std::fprintf(f, "f %d %d %d\n", c, b, d);
        }
    }

    std::fclose(f);
    std::printf("Exported: %s (%d vertices)\n", filename, (resU + 1) * (resV + 1));
}
