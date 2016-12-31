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

static void gb_update_debugs(void)
{
    cpu_debug_flags(game_boy.debug_flags, sizeof(game_boy.debug_flags));
    cpu_debug_last_instr(game_boy.debug_last_instr,
                    sizeof(game_boy.debug_last_instr));
    cpu_debug_next_instr(game_boy.debug_next_instr,
                    sizeof(game_boy.debug_next_instr));
    cpu_debug_cycles(game_boy.debug_cycles, sizeof(game_boy.debug_cycles));
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
                gb_update_debugs();
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

#if 0
static void gb_render_help(void)
{
    glRasterPos2f(0, INFO_FONT_SIZE);
    ftglRenderFont(game_boy.font_info, "[Esc]Quit", FTGL_RENDER_ALL);
}

static void gb_render_debug(void)
{
    char str[100];
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(500, INFO_FONT_SIZE);
    ftglRenderFont(game_boy.font_info, "Debug info", FTGL_RENDER_ALL);
    GLfloat debug_y = 30.0;
    const GLfloat debug_x = 500.0;
    /* PC and SP. */
    glRasterPos2f(debug_x, debug_y);
    snprintf(str, sizeof(str), "PC:0x%04x SP:0x%04x", game_boy.cpu->reg.pc,
             game_boy.cpu->reg.sp);
    ftglRenderFont(game_boy.font_debug, str, FTGL_RENDER_ALL);
    debug_y += DEBUG_FONT_SIZE;
    /* Registers. */
    glRasterPos2f(debug_x, debug_y);
    snprintf(str, sizeof(str), "AF:0x%04x BC:0x%04x DE:0x%04x HL:0x%04x",
             game_boy.cpu->reg.af, game_boy.cpu->reg.bc, game_boy.cpu->reg.de,
             game_boy.cpu->reg.hl);
    ftglRenderFont(game_boy.font_debug, str, FTGL_RENDER_ALL);
    debug_y += DEBUG_FONT_SIZE;
    /* Flags. */
    glRasterPos2f(debug_x, debug_y);
    ftglRenderFont(game_boy.font_debug, game_boy.debug_flags, FTGL_RENDER_ALL);
    debug_y += DEBUG_FONT_SIZE;
    /* Current instruction. */
    glRasterPos2f(debug_x, debug_y);
    ftglRenderFont(game_boy.font_debug, game_boy.debug_last_instr,
                   FTGL_RENDER_ALL);
    debug_y += DEBUG_FONT_SIZE;
    /* Next instruction. */
    glRasterPos2f(debug_x, debug_y);
    ftglRenderFont(game_boy.font_debug, game_boy.debug_next_instr,
                   FTGL_RENDER_ALL);
    debug_y += DEBUG_FONT_SIZE;
    /* Print cycle count. */
    glRasterPos2f(debug_x, debug_y);
    ftglRenderFont(game_boy.font_debug, game_boy.debug_cycles, FTGL_RENDER_ALL);
}

static void gb_render_info(void)
{
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 0.0f);
    gb_render_help();
    if (game_boy.debug == true) {
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
    // cpu_render(game_boy.cpu, game_boy.width, game_boy.height);
    /* Do other glfw things. */
    glfwSwapBuffers(game_boy.window);
    glfwPollEvents();
}
#endif

static void gb_gl_init(void)
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
        gb_update_debugs();
    }
    return 0;
}

void gb_main(void)
{
    while (!glfwWindowShouldClose(game_boy.window)) {
        cpu_emulate_cycle();
    }
}
