#include "Overlay.h"
#include "FontRenderer.h"
#include "Stats.h"
#include "KleinMesh.h"
#include "DemoScene.h"
#include <cstdio>
#include <GL/gl.h>

static const char* COLOR_NAMES[] = {
    "Rainbow", "Ocean", "Sunset", "Mono", "Neon"
};

static void BeginOverlay(int winW, int winH) {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, winW, 0, winH, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

static void EndOverlay() {
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void Overlay_Render(float fps, int winW, int winH) {
    BeginOverlay(winW, winH);

    char buf[256];

    int ci = KleinMesh_GetColorIndex();
    std::snprintf(buf, sizeof(buf), "FPS: %.0f  CPU: %.1f%%  RAM: %.0fMB (%d%%)  Color: %s",
                  fps, Stats_GetCPU(), Stats_GetRAM(), Stats_GetRAMPercent(),
                  (ci >= 0 && ci < 5) ? COLOR_NAMES[ci] : "?");
    FontRenderer_DrawString(buf, 10.0f, static_cast<float>(winH - 20));

    if (DemoScene_IsActive()) {
        float u1, v1, u2, v2;
        DemoScene_GetUV(u1, v1);
        DemoScene_GetUV2(u2, v2);
        std::snprintf(buf, sizeof(buf),
            "Ball1: u=%.2f v=%.2f  Ball2: u=%.2f v=%.2f",
            u1, v1, u2, v2);
        FontRenderer_DrawString(buf, 10.0f, static_cast<float>(winH - 38));
    }

    if (KleinMesh_IsClipEnabled()) {
        std::snprintf(buf, sizeof(buf), "Clip plane: x = %.2f", KleinMesh_GetClipPlane());
        FontRenderer_DrawString(buf, 10.0f, static_cast<float>(winH - 56));
    }

    FontRenderer_DrawString(
        "[LMB] Rotate  [Scroll] Zoom  [Space] Play/Pause  [T] Trail  [W] Wire  [N] Normals",
        10.0f, 38.0f);
    FontRenderer_DrawString(
        "[A] AutoRot  [F11] Fullscreen  [+/-] Speed  [1-5] Color  [Q/E] Clip  [B] BG  [O] OBJ  [V] View  [R] Reset",
        10.0f, 22.0f);
    FontRenderer_DrawString(
        "[P] Screenshot",
        10.0f, 6.0f);

    EndOverlay();
}
