#include "Renderer.h"
#include "KleinMath.h"
#include <GL/gl.h>
#include <SDL2/SDL.h>

static SDL_Window* window = nullptr;
static SDL_GLContext glContext = nullptr;
static bool running = true;

static float rotX = 20.0f;
static float rotY = 0.0f;
static int windowWidth = 800;
static int windowHeight = 600;
static const int RESOLUTION_U = 60;
static const int RESOLUTION_V = 60;
static const float BOTTLE_RADIUS = 1.0f;

static void RenderFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -7.0f);
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    const float PI = 3.14159265f;

    for (int i = 0; i < RESOLUTION_U; ++i) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j < RESOLUTION_V; ++j) {
            float u = (static_cast<float>(i) / RESOLUTION_U) * PI;
            float v = (static_cast<float>(j) / RESOLUTION_V) * 2.0f * PI;

            Point3D p = ComputeKlein(u, v, BOTTLE_RADIUS);
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();
    }
}

static void UpdateProjection(int width, int height) {
    if (height == 0) height = 1;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    glFrustum(-aspect, aspect, -1.0, 1.0, 2.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
}

void intRenderer(int argc, char** argv) {
    (void)argc;
    (void)argv;

    SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland,x11");
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow("Klein Bottle simulator",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              windowWidth, windowHeight,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window) {
        SDL_Quit();
        return;
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    glEnable(GL_DEPTH_TEST);
    UpdateProjection(windowWidth, windowHeight);
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
            }
        }

        RenderFrame();
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
