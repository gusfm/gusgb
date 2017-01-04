#include "game_boy.h"
#include <FTGL/ftgl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "gpu.h"

#define INFO_FONT_SIZE 16
#define DEBUG_FONT_SIZE 14

typedef struct {
    int width;
    int height;
    FTGLfont *font_info;
    FTGLfont *font_debug;
    GLFWwindow *window;
    cpu_t *cpu;
    bool debug;
    char debug_flags[100];
    char debug_last_instr[100];
    char debug_next_instr[100];
    char debug_cycles[100];
} game_boy_t;

static game_boy_t game_boy;

static void gb_error_callback(int error, const char *description)
{
    (void)error;
    fputs(description, stderr);
}

static void gb_key_callback(GLFWwindow *window, int key, int scancode,
                            int action, int mods)
{
    (void)scancode;
    (void)mods;
    switch (key) {
        case GLFW_KEY_Q:
        case GLFW_KEY_ESCAPE:
            /* Quit. */
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;

        case GLFW_KEY_N:
            if (game_boy.debug == true && action == GLFW_PRESS) {
                cpu_emulate_cycle();
            }
            break;

        default:
            break;
    }
}

static void gb_resize_callback(GLFWwindow *window, int width, int height)
{
    (void)window;
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
    if (game_boy.font_info) {
        ftglDestroyFont(game_boy.font_info);
    }
    if (game_boy.window) {
        glfwDestroyWindow(game_boy.window);
        glfwTerminate();
    }
}

static void gb_gl_init(void)
{
    /* Setup our viewport to be the entire size of the window. */
    glViewport(0, 0, (GLsizei)game_boy.width, (GLsizei)game_boy.height);
    /* Change to the projection matrix, reset the matrix and set up orthagonal
     * projection. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /* Paramters: left, right, bottom, top, near, far. */
    glOrtho(0.0f, game_boy.width, game_boy.height, 0.0, -1.0f, 1.0f);
    glClearColor(0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_FLAT);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DITHER);
    glDisable(GL_BLEND);
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

int gb_init(int width, int height, const char *rom_path, bool debug,
            uint16_t breakpoint)
{
    game_boy.width = width;
    game_boy.height = height;
    game_boy.debug = breakpoint == 0 ? debug : true;
    /* Create window. */
    game_boy.window = gb_create_window("gusgb");
    if (game_boy.window == NULL) {
        fprintf(stderr, "Error: Cannot create window\n");
        return -1;
    }
    /* Create a pixmap font from a TrueType file. */
    game_boy.font_info = ftglCreatePixmapFont(
        "/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf");
    if (game_boy.font_info == NULL) {
        gb_finish();
        return -1;
    }
    game_boy.font_debug = ftglCreatePixmapFont(
        "/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf");
    if (game_boy.font_debug == NULL) {
        gb_finish();
        return -1;
    }
    /* Set the font size. */
    ftglSetFontFaceSize(game_boy.font_info, INFO_FONT_SIZE, INFO_FONT_SIZE);
    ftglSetFontFaceSize(game_boy.font_debug, DEBUG_FONT_SIZE, DEBUG_FONT_SIZE);
    /* Initialize OpenGl. */
    gb_gl_init();
    /* Initialize emulation. */
    int ret = cpu_init(rom_path);
    if (ret < 0) {
        fprintf(stderr, "Error: Cannot load rom: %s\n", rom_path);
        return -1;
    }
    gpu_set_glfw_window(game_boy.window);
    game_boy.cpu = cpu_get_instance();
    if (game_boy.debug == true) {
        /* Execute until breakpoint. */
        while (game_boy.cpu->reg.pc < breakpoint) {
            cpu_emulate_cycle();
        }
    }
    return 0;
}

void gb_main(void)
{
    while (!glfwWindowShouldClose(game_boy.window)) {
        cpu_emulate_cycle();
    }
}
