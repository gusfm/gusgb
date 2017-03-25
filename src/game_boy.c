#include "game_boy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpu.h"
#include "keys.h"

game_boy_t GB;

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

        /* Game keys. */
        case GLFW_KEY_A:
            if (action == GLFW_PRESS)
                key_press(KEY_A);
            else if (action == GLFW_RELEASE)
                key_release(KEY_A);
            break;
        case GLFW_KEY_S:
            if (action == GLFW_PRESS)
                key_press(KEY_B);
            else if (action == GLFW_RELEASE)
                key_release(KEY_B);
            break;
        case GLFW_KEY_ENTER:
            if (action == GLFW_PRESS)
                key_press(KEY_START);
            else if (action == GLFW_RELEASE)
                key_release(KEY_START);
            break;
        case GLFW_KEY_SPACE:
            if (action == GLFW_PRESS)
                key_press(KEY_SELECT);
            else if (action == GLFW_RELEASE)
                key_release(KEY_SELECT);
            break;
        case GLFW_KEY_UP:
            if (action == GLFW_PRESS)
                key_press(KEY_UP);
            else if (action == GLFW_RELEASE)
                key_release(KEY_UP);
            break;
        case GLFW_KEY_DOWN:
            if (action == GLFW_PRESS)
                key_press(KEY_DOWN);
            else if (action == GLFW_RELEASE)
                key_release(KEY_DOWN);
            break;
        case GLFW_KEY_LEFT:
            if (action == GLFW_PRESS)
                key_press(KEY_LEFT);
            else if (action == GLFW_RELEASE)
                key_release(KEY_LEFT);
            break;
        case GLFW_KEY_RIGHT:
            if (action == GLFW_PRESS)
                key_press(KEY_RIGHT);
            else if (action == GLFW_RELEASE)
                key_release(KEY_RIGHT);
            break;

        default:
            break;
    }
}

static void gb_resize_callback(GLFWwindow *window, int width, int height)
{
    (void)window;
    GB.width = width;
    GB.height = height;
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
    if (GB.window) {
        glfwDestroyWindow(GB.window);
        glfwTerminate();
    }
}

static void gb_gl_init(void)
{
    /* Setup our viewport to be the entire size of the window. */
    glViewport(0, 0, (GLsizei)GB.width, (GLsizei)GB.height);
    /* Change to the projection matrix, reset the matrix and set up orthagonal
     * projection. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /* Paramters: left, right, bottom, top, near, far. */
    glOrtho(0.0f, GB.width, GB.height, 0.0, -1.0f, 1.0f);
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
    window = glfwCreateWindow(GB.width, GB.height, name, NULL, NULL);
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

int gb_init(int width, int height, float window_zoom, const char *rom_path)
{
    GB.width = width;
    GB.height = height;
    /* Create window. */
    GB.window = gb_create_window("gusgb");
    if (GB.window == NULL) {
        fprintf(stderr, "Error: Cannot create window\n");
        return -1;
    }
    /* Initialize OpenGl. */
    gb_gl_init();
    /* Initialize emulation. */
    int ret = cpu_init(rom_path, window_zoom);
    if (ret < 0) {
        fprintf(stderr, "Error: Cannot load rom: %s\n", rom_path);
        return -1;
    }
    gpu_set_glfw_window(GB.window);
    return 0;
}

void gb_main(void)
{
    while (!glfwWindowShouldClose(GB.window)) {
        cpu_emulate_cycle();
    }
}
