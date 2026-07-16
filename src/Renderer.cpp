#include "Renderer.h"
#include "KleinMesh.h"
#include "DemoScene.h"
#include "Overlay.h"
#include "FontRenderer.h"
#include "Stats.h"
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

static SDL_Window* window = nullptr;
static SDL_GLContext glContext = nullptr;
static bool running = true;

static float rotX = 20.0f;
static float rotY = 0.0f;
static float zoom  = -5.0f;
static int windowWidth = 800;
static int windowHeight = 600;
static bool fullscreen = false;

static const int RESOLUTION_U = 80;
static const int RESOLUTION_V = 40;
static const float BOTTLE_RADIUS = 0.7f;

static bool dragging = false;
static int lastMouseX = 0;
static int lastMouseY = 0;

static bool autoRotate = false;
static bool wireframe  = false;
static bool showNormals = false;
static bool darkBG = true;

static int frameCount = 0;
static Uint32 fpsTimer = 0;
static float currentFPS = 0.0f;
static Uint32 statsTimer = 0;

static void UpdateProjection(int width, int height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    glFrustum(-aspect, aspect, -1.0, 1.0, 2.0, 50.0);
    glMatrixMode(GL_MODELVIEW);
}

static void ToggleFullscreen() {
    fullscreen = !fullscreen;
    if (fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(window, 0);
        SDL_SetWindowSize(window, windowWidth, windowHeight);
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
}

static void ToggleBackground() {
    darkBG = !darkBG;
    if (darkBG)
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    else
        glClearColor(0.92f, 0.92f, 0.95f, 1.0f);
}

static void SaveScreenshot() {
    char filename[64];
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    std::snprintf(filename, sizeof(filename), "screenshot_%04d%02d%02d_%02d%02d%02d.bmp",
                  t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                  t->tm_hour, t->tm_min, t->tm_sec);

    int w, h;
    SDL_GL_GetDrawableSize(window, &w, &h);

    int rowBytes = w * 4;
    SDL_Surface* surf = SDL_CreateRGBSurface(0, w, h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!surf) return;

    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);

    unsigned char* data = (unsigned char*)surf->pixels;
    unsigned char* rowBuf = (unsigned char*)std::malloc(rowBytes);
    if (rowBuf) {
        for (int y = 0; y < h / 2; ++y) {
            unsigned char* top = data + y * surf->pitch;
            unsigned char* bot = data + (h - 1 - y) * surf->pitch;
            std::memcpy(rowBuf, top, rowBytes);
            std::memcpy(top, bot, rowBytes);
            std::memcpy(bot, rowBuf, rowBytes);
        }
        std::free(rowBuf);
    }

    SDL_SaveBMP(surf, filename);
    SDL_FreeSurface(surf);
    std::printf("Saved: %s\n", filename);
}

static void RenderFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, zoom);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else           glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    KleinMesh_Render(BOTTLE_RADIUS, RESOLUTION_U, RESOLUTION_V);

    if (showNormals)
        KleinMesh_RenderNormals(BOTTLE_RADIUS, RESOLUTION_U, RESOLUTION_V, 0.3f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    DemoScene_RenderTrail();
    DemoScene_Render(BOTTLE_RADIUS);
    DemoScene_RenderSecond(BOTTLE_RADIUS);

    Overlay_Render(currentFPS, windowWidth, windowHeight);
}

void intRenderer(int argc, char** argv) {
    FontRenderer_Init();
    Stats_Init();

    SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland,x11");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow("Klein Bottle simulator",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              windowWidth, windowHeight,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) { SDL_Quit(); return; }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) { SDL_DestroyWindow(window); SDL_Quit(); return; }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    KleinMesh_InitLighting();
    UpdateProjection(windowWidth, windowHeight);

    fpsTimer = SDL_GetTicks();
    statsTimer = fpsTimer;
}

void runRenderer() {
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    windowWidth = event.window.data1;
                    windowHeight = event.window.data2;
                    UpdateProjection(windowWidth, windowHeight);
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    dragging = true;
                    lastMouseX = event.button.x;
                    lastMouseY = event.button.y;
                }
            } else if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    dragging = false;
                }
            } else if (event.type == SDL_MOUSEWHEEL) {
                zoom += event.wheel.y * 0.5f;
                if (zoom > -1.5f)  zoom = -1.5f;
                if (zoom < -20.0f) zoom = -20.0f;
            } else if (event.type == SDL_MOUSEMOTION) {
                if (dragging) {
                    rotY += (event.motion.x - lastMouseX) * 0.5f;
                    rotX += (event.motion.y - lastMouseY) * 0.5f;
                    lastMouseX = event.motion.x;
                    lastMouseY = event.motion.y;
                }
            } else if (event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;
                if (key == SDLK_SPACE)       DemoScene_Toggle();
                else if (key == SDLK_r) {
                    DemoScene_Reset();
                    rotX = 20.0f; rotY = 0.0f; zoom = -5.0f;
                    KleinMesh_EnableClip(false);
                }
                else if (key == SDLK_t)
                    DemoScene_SetTrailVisible(!DemoScene_IsTrailVisible());
                else if (key == SDLK_w)
                    wireframe = !wireframe;
                else if (key == SDLK_a)
                    autoRotate = !autoRotate;
                else if (key == SDLK_n)
                    showNormals = !showNormals;
                else if (key == SDLK_b)
                    ToggleBackground();
                else if (key == SDLK_F11)
                    ToggleFullscreen();
                else if (key == SDLK_p)
                    SaveScreenshot();
                else if (key == SDLK_v) {
                    rotX = 20.0f; rotY = 0.0f; zoom = -5.0f;
                }
                else if (key == SDLK_EQUALS || key == SDLK_PLUS) {
                    float s = DemoScene_GetSpeed() + 0.001f;
                    if (s > 0.03f) s = 0.03f;
                    DemoScene_SetSpeed(s);
                }
                else if (key == SDLK_MINUS || key == SDLK_UNDERSCORE) {
                    float s = DemoScene_GetSpeed() - 0.001f;
                    if (s < 0.001f) s = 0.001f;
                    DemoScene_SetSpeed(s);
                }
                else if (key >= SDLK_1 && key <= SDLK_5) {
                    KleinMesh_SetColor(key - SDLK_1);
                }
                else if (key == SDLK_q) {
                    KleinMesh_EnableClip(true);
                    float cx = KleinMesh_GetClipPlane() - 0.1f;
                    if (cx < -4.0f) cx = -4.0f;
                    KleinMesh_SetClipPlane(cx);
                }
                else if (key == SDLK_e) {
                    KleinMesh_EnableClip(true);
                    float cx = KleinMesh_GetClipPlane() + 0.1f;
                    if (cx > 4.0f) cx = 4.0f;
                    KleinMesh_SetClipPlane(cx);
                }
                else if (key == SDLK_o) {
                    KleinMesh_ExportOBJ("klein_bottle.obj", BOTTLE_RADIUS, RESOLUTION_U, RESOLUTION_V);
                }
            }
        }

        if (autoRotate) rotY += 0.3f;

        DemoScene_Update(DemoScene_GetSpeed());

        frameCount++;
        Uint32 now = SDL_GetTicks();
        if (now - fpsTimer >= 200) {
            currentFPS = frameCount * 1000.0f / static_cast<float>(now - fpsTimer);
            frameCount = 0;
            fpsTimer = now;
        }
        if (now - statsTimer >= 500) {
            Stats_Update();
            statsTimer = now;
        }

        RenderFrame();
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
