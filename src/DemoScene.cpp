#include "DemoScene.h"
#include "KleinMesh.h"
#include "KleinMath.h"
#include <GL/gl.h>
#include <cmath>
#include <vector>

static bool active = false;
static bool paused = false;
static float t = 0.0f;
static float speed = 0.005f;
static bool trailVisible = true;
static const float PI = 3.14159265f;

static const int TRAIL_MAX = 512;
static std::vector<Point3D> trail;
static std::vector<Point3D> trail2;

static void DrawSphere(float radius, int slices, int stacks) {
    for (int i = 0; i < stacks; ++i) {
        float lat0 = PI * (-0.5f + static_cast<float>(i) / stacks);
        float lat1 = PI * (-0.5f + static_cast<float>(i + 1) / stacks);
        float y0 = std::sin(lat0), yr0 = std::cos(lat0);
        float y1 = std::sin(lat1), yr1 = std::cos(lat1);

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float lng = 2.0f * PI * static_cast<float>(j) / slices;
            float x = std::cos(lng), z = std::sin(lng);

            glNormal3f(x * yr0, y0, z * yr0);
            glVertex3f(radius * x * yr0, radius * y0, radius * z * yr0);

            glNormal3f(x * yr1, y1, z * yr1);
            glVertex3f(radius * x * yr1, radius * y1, radius * z * yr1);
        }
        glEnd();
    }
}

bool DemoScene_IsActive()          { return active; }
float DemoScene_GetSpeed()         { return speed; }
bool DemoScene_IsTrailVisible()    { return trailVisible; }
void  DemoScene_SetTrailVisible(bool v) { trailVisible = v; }
void  DemoScene_SetSpeed(float s)  { speed = s; }

void DemoScene_GetUV(float& u, float& v) {
    u = active ? t : 0.0f;
    v = active ? t * 3.0f : 0.0f;
}

void DemoScene_GetUV2(float& u, float& v) {
    u = active ? t + PI : 0.0f;
    v = active ? t * 2.0f : 0.0f;
}

void DemoScene_Toggle() {
    if (!active) {
        active = true;
        paused = false;
        t = 0.0f;
        trail.clear();
        trail2.clear();
    } else {
        paused = !paused;
    }
}

void DemoScene_Reset() {
    active = false;
    paused = false;
    t = 0.0f;
    trail.clear();
    trail2.clear();
}

void DemoScene_Update(float dt) {
    if (!active || paused) return;
    t += dt;
    if (t > 2.0f * PI) t -= 2.0f * PI;

    Point3D pos = KleinMesh_Eval(t, t * 3.0f, 0.7f);
    trail.push_back(pos);
    if ((int)trail.size() > TRAIL_MAX)
        trail.erase(trail.begin());

    float u2 = t + PI;
    float v2 = t * 2.0f;
    Point3D pos2 = KleinMesh_Eval(u2, v2, 0.7f);
    trail2.push_back(pos2);
    if ((int)trail2.size() > TRAIL_MAX)
        trail2.erase(trail2.begin());
}

void DemoScene_RenderTrail() {
    if (!trailVisible) return;

    glDisable(GL_LIGHTING);
    glLineWidth(2.5f);

    if (trail.size() >= 2) {
        glBegin(GL_LINE_STRIP);
        int n = (int)trail.size();
        for (int i = 0; i < n; ++i) {
            float alpha = (float)i / (float)n;
            glColor4f(1.0f, 0.4f, 0.1f, alpha * 0.8f + 0.2f);
            glVertex3f(trail[i].x, trail[i].y, trail[i].z);
        }
        glEnd();
    }

    if (trail2.size() >= 2) {
        glBegin(GL_LINE_STRIP);
        int n = (int)trail2.size();
        for (int i = 0; i < n; ++i) {
            float alpha = (float)i / (float)n;
            glColor4f(0.2f, 0.5f, 1.0f, alpha * 0.8f + 0.2f);
            glVertex3f(trail2[i].x, trail2[i].y, trail2[i].z);
        }
        glEnd();
    }

    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

static void RenderBall(float u, float v, float bottleRadius,
                        float cr, float cg, float cb) {
    Point3D pos = KleinMesh_Eval(u, v, bottleRadius);

    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);

    GLfloat matAmbDiff[] = { cr, cg, cb, 1.0f };
    GLfloat matEmit[]    = { cr * 0.8f, cg * 0.8f, cb * 0.8f, 1.0f };
    GLfloat matBlack[]   = { 0.0f, 0.0f, 0.0f, 0.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmit);
    glColor3f(cr, cg, cb);
    DrawSphere(0.12f, 20, 20);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matBlack);
    glPopMatrix();
}

void DemoScene_Render(float bottleRadius) {
    if (!active) return;
    RenderBall(t, t * 3.0f, bottleRadius, 1.0f, 0.3f, 0.1f);
}

void DemoScene_RenderSecond(float bottleRadius) {
    if (!active) return;
    RenderBall(t + PI, t * 2.0f, bottleRadius, 0.2f, 0.5f, 1.0f);
}
