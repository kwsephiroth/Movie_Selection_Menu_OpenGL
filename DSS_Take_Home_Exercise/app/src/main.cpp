#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "renderer.h"
#include "shader.h"
#include <atomic>

namespace
{
    static unsigned int rendering_program;
    static int width, height;
    static float aspect;
    static glm::mat4 pMat;
    static bool keys[1024];
    static unsigned int position_attrib_location;
    static unsigned int texture_attrib_location;
    static std::atomic<bool> new_key_pressed = false;
}

static void init_shader_program(GLFWwindow* window)
{
    const std::string vShaderSource =
        #include "../res/shaders/vertex_shader.glsl"
        ;
    const std::string fShaderSource =
        #include "../res/shaders/fragment_shader.glsl"
        ;
    DSS::Shader shader(vShaderSource.c_str(), fShaderSource.c_str());
    rendering_program = shader.Program;
    position_attrib_location = glGetAttribLocation(rendering_program, "tilePos");
    texture_attrib_location = glGetAttribLocation(rendering_program, "texCoord");
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(1920), 0.0f, static_cast<float>(1080));
    shader.Use();
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUseProgram(0);
}

// Is called whenever a key is pressed/released via GLFW
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //cout << key << endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            keys[key] = true;
            new_key_pressed.store(true);
        }
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
        }
    }
}

static void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
    aspect = (float)newWidth / (float)newHeight;
    glViewport(0, 0, newWidth, newHeight);
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

static void resize_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void process_controller_input(DSS::Renderer& renderer)
{
    using namespace DSS;

    if (keys[GLFW_KEY_UP])
    {
        renderer.process_controller_input(ControllerInput::UP);
    }
    if (keys[GLFW_KEY_DOWN])
    {
        renderer.process_controller_input(ControllerInput::DOWN);
    }
    if (keys[GLFW_KEY_LEFT])
    {
        renderer.process_controller_input(ControllerInput::LEFT);
    }
    if (keys[GLFW_KEY_RIGHT])
    {
        renderer.process_controller_input(ControllerInput::RIGHT);
    }
}

int main(void)
{
    #ifdef WIN32
        #if not defined(_DEBUG)
            FreeConsole();
        #endif
    #endif

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "Disney+", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize glew*/
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    //Set up callback functions to capture and respond to events
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    init_shader_program(window);

    DSS::Renderer renderer(rendering_program, position_attrib_location, texture_attrib_location);

    std::cout << "Rendering home page ... " << std::endl;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        //Check and call events
        glfwPollEvents(); //Any pressed keys will be recorded

        //Process any controller inputs
        if (new_key_pressed.load())
        {
            process_controller_input(renderer);
            new_key_pressed.store(false);
        }

        glClearColor(0, 0, 255, 1);//Background color
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        renderer.draw_home_page();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}