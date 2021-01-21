#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL2/SOIL2.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp> // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "renderer.h"

int main(void)
{
    DSS::Renderer r;
    //download_file_to_disk(DSS::DSS_HOME_JSON_URL, "home.json");
    //auto rj_doc_opt = DSS::get_json_doc_from_file("app/res/home.json");
    //if (!rj_doc_opt.has_value())
    //{
    //    std::cout << "Failed to create json document from file 'res/home.json'" << std::endl;
    //}
    //auto rj_doc = std::move(rj_doc_opt.value());
    //for (auto itr = rj_doc.MemberBegin(); itr != rj_doc.MemberEnd(); ++itr)
    //{
    //    std::cout << itr->name.GetString() << std::endl;
    //}
    //std::cin.get();
    //GLFWwindow* window;

    ///* Initialize the library */
    //if (!glfwInit())
    //    return -1;

    ///* Create a windowed mode window and its OpenGL context */
    //window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    //if (!window)
    //{
    //    glfwTerminate();
    //    return -1;
    //}

    ///* Make the window's context current */
    //glfwMakeContextCurrent(window);

    ///* Initialize glew*/
    //GLenum err = glewInit();
    //if (GLEW_OK != err)
    //{
    //    /* Problem: glewInit failed, something is seriously wrong. */
    //    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    //}
    //fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    //std::cout << glGetString(GL_VERSION) << std::endl;

    ///* Loop until the user closes the window */
    //while (!glfwWindowShouldClose(window))
    //{
    //    /* Render here */
    //    glClear(GL_COLOR_BUFFER_BIT);

    //    /* Swap front and back buffers */
    //    glfwSwapBuffers(window);

    //    /* Poll for and process events */
    //    glfwPollEvents();
    //}

    //glfwTerminate();
    std::cin.get();
    return 0;
}