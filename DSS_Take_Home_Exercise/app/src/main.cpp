#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2/SOIL2.h>
#include <glm\glm.hpp>
//#include <glm\gtc\type_ptr.hpp> // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
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
    static glm::vec2 grid_offsets[4][5] =
    {
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}}
    };
    static std::atomic<bool> new_key_pressed = false;
}

static void init(GLFWwindow* window)
{
    const std::string vShaderSource =
        #include "../res/shaders/vertex_shader.glsl"
        ;
    const std::string fShaderSource =
        #include "../res/shaders/fragment_shader.glsl"
        ;
    DSS::Shader shader(vShaderSource.c_str(), fShaderSource.c_str());
    rendering_program = shader.Program;
    position_attrib_location = glGetAttribLocation(rendering_program, "pos");
    texture_attrib_location = glGetAttribLocation(rendering_program, "texCoord");
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

// Is called whenever a key is pressed/released via GLFW
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //cout << key << endl;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
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
    static glm::vec2 focused_tile_position = INIT_FOCUSED_TILE_POSITION;

    if (keys[GLFW_KEY_W])
    {
        if (focused_tile_position.x == 0)//On row 1
        {
            std::cout << "Can't move UP any further." << std::endl;
        }
        else
        {
            --focused_tile_position.x;
            renderer.process_controller_input(ControllerInput::UP, focused_tile_position);
            std::cout << "focused_tile_position (" << focused_tile_position.x << " , " << focused_tile_position.y << " ) " << std::endl;
        }
        
    }
    if (keys[GLFW_KEY_S])
    {
        if (focused_tile_position.x == 3)//On row 4
        {
            std::cout << "Can't move DOWN any further." << std::endl;
        }
        else
        {
            ++focused_tile_position.x;
            renderer.process_controller_input(ControllerInput::DOWN, focused_tile_position);
            std::cout << "focused_tile_position (" << focused_tile_position.x << " , " << focused_tile_position.y << " ) " << std::endl;
        }
        
    }
    if (keys[GLFW_KEY_A])
    {
        if (focused_tile_position.y == 0)//On column 1
        {
            std::cout << "Can't move LEFT any further." << std::endl;
        }
        else
        {
            --focused_tile_position.y;
            renderer.process_controller_input(ControllerInput::LEFT, focused_tile_position);
            std::cout << "focused_tile_position (" << focused_tile_position.x << " , " << focused_tile_position.y << " ) " << std::endl;
        }
    }
    if (keys[GLFW_KEY_D])
    {
        if (focused_tile_position.y == 4)//On column 5
        {
            std::cout << "Can't move RIGHT any further." << std::endl;
        }
        else
        {
            ++focused_tile_position.y;
            renderer.process_controller_input(ControllerInput::RIGHT, focused_tile_position);
            std::cout << "focused_tile_position (" << focused_tile_position.x << " , " << focused_tile_position.y << " ) " << std::endl;
        }
    }
}


int main(void)
{
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
    //fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

   // std::cout << glGetString(GL_VERSION) << std::endl;

    //Set up callback functions to capture and respond to events
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    init(window);

    DSS::Renderer renderer(rendering_program, position_attrib_location, texture_attrib_location);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        //Check and call events
        glfwPollEvents(); //Any pressed keys will be recorded

        //Process any controller inputs
        //TODO: Avoid doing this every frame somehow;
        if (new_key_pressed.load())
        {
            process_controller_input(renderer);
            new_key_pressed.store(false);
        }

        glClearColor(0, 0, 255, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        renderer.draw_home_page();

        //while (GLenum error = glGetError())
        //{
        //    std::cout << "[OpenGL Error] (" << error << ") " << std::endl;
        //}

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}