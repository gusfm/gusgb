#include "game_boy.h"
#include <FTGL/ftgl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"

#define INFO_FONT_SIZE 16

typedef struct {
    int width;
    int height;
    FTGLfont *font;
    GLFWwindow *window;
    cpu_t *cpu;
    bool debug;
} game_boy_t;

static game_boy_t game_boy;

static void gb_error_callback(int error, const char *description)
{
    fputs(description, stderr);
}

static void gb_key_callback(GLFWwindow *window, int key, int scancode,
                            int action, int mods)
{
    switch (key) {
        case GLFW_KEY_ESCAPE:
            /* Quit. */
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;

        default:
            break;
    }
}

static void gb_resize_callback(GLFWwindow *window, int width, int height)
{
    game_boy.width = width;
    game_boy.height = height;
    /* Setup our viewport to be the entire size of the window. */
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    /* Select the projection matrix. */
    glMatrixMode(GL_PROJECTION);
    /* Reset the projection matrix. */
    glLoadIdentity();
    glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
}

void gb_finish(void)
{
    if (game_boy.font) {
        ftglDestroyFont(game_boy.font);
    }
    if (game_boy.window) {
        glfwDestroyWindow(game_boy.window);
        glfwTerminate();
    }
}

static void gb_render_help()
{
    glRasterPos2f(0, INFO_FONT_SIZE);
    ftglRenderFont(game_boy.font, "[Esc]Quit", FTGL_RENDER_ALL);
}

void gb_render_debug(void)
{
}

static void gb_render_info()
{
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 0.0f);
    gb_render_help();
    if (game_boy.debug) {
        gb_render_debug();
    }
    glPopMatrix();
}

static void gb_render(void)
{
    /* Clear the screen. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* Reset the matrix. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /* Render info before scale and translate. */
    gb_render_info();
    /* Draw stuff. */
    //cpu_render(game_boy.cpu, game_boy.width, game_boy.height);
    /* Do other glfw things. */
    glfwSwapBuffers(game_boy.window);
    glfwPollEvents();
}

static void gb_gl_init()
{
    /* Setup our viewport to be the entire size of the window. */
    glViewport(0, 0, (GLsizei)game_boy.width, (GLsizei)game_boy.height);
    /* Change to the projection matrix, reset the matrix and set up orthagonal
     * projection. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /* Paramters: left, right, bottom, top, near, far. */
    glOrtho(0.0f, game_boy.width, game_boy.height, 0.0, -1.0f, 1.0f);
    /* ----- OpenGL settings ----- */
    /* Enable (gouraud) shading. */
    glEnable(GL_SMOOTH);
    /* Disable depth testing. */
    glDisable(GL_DEPTH_TEST);
}

static GLFWwindow *gb_create_window(const char *name)
{
    GLFWwindow *window;
    /* Set an error callback. */
    glfwSetErrorCallback(gb_error_callback);
    /* Initialize window. */
    if (!glfwInit())
        return NULL;
    /* Creating a window and context. */
    window =
        glfwCreateWindow(game_boy.width, game_boy.height, name, NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return NULL;
    }
    /* Make the OpenGL context current. */
    glfwMakeContextCurrent(window);
    /* Set a key input callback. */
    glfwSetKeyCallback(window, gb_key_callback);
    glfwSetFramebufferSizeCallback(window, gb_resize_callback);
    return window;
}

int gb_init(const char *rom_path, int width, int height, bool debug)
{
    game_boy.width = width;
    game_boy.height = height;
    game_boy.debug = debug;
    /* Create window. */
    game_boy.window = gb_create_window("gusgb");
    if (game_boy.window == NULL) {
        fprintf(stderr, "Error: Cannot create window\n");
        return -1;
    }
    /* Create a pixmap font from a TrueType file. */
    game_boy.font = ftglCreatePixmapFont(
        "/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf");
    if (game_boy.font == NULL) {
        gb_finish();
        return -1;
    }
    /* Set the font size. */
    ftglSetFontFaceSize(game_boy.font, INFO_FONT_SIZE, INFO_FONT_SIZE);
    /* Initialize OpenGl. */
    gb_gl_init();
    /* Initialize emulation. */
    int ret = cpu_init(rom_path);
    if (ret < 0) {
        fprintf(stderr, "Error: Cannot load rom: %s\n", rom_path);
        return -1;
    }
    return 0;
}

static void gb_process(void)
{
    cpu_emulate_cycle();
}

void gb_main(void)
{
    while (!glfwWindowShouldClose(game_boy.window)) {
        gb_process();
        gb_render();
    }
}
