#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <curl/curl.h>
#include <SOIL2/SOIL2.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp> // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "utils.h"
#include "curl_utils.h"
#include "constants.h"

int main(void)
{
    auto file_memory = download_file_to_memory(DSS::DSS_HOME_JSON_URL);
    std::cout << std::string(file_memory->memory) << std::endl;
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

/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2018, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
 /* <DESC>
  * Download a given URL into a local file named page.out.
  * </DESC>
  */

//#include <stdio.h>
//#include <stdlib.h>
//#include <curl/curl.h>
//
//static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
//{
//    size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
//    return written;
//}
//
//int main(int argc, char* argv[])
//{
//    CURL* curl_handle;
//    static const char* pagefilename = "page.out";
//    FILE* pagefile;
//
//    curl_global_init(CURL_GLOBAL_ALL);
//
//    /* init the curl session */
//    curl_handle = curl_easy_init();
//
//    /* set URL to get here */
//    curl_easy_setopt(curl_handle, CURLOPT_URL, "https://cd-static.bamgrid.com/dp-117731241344/home.json");//argv[1]);
//
//    /* Switch on full protocol/debug output while testing */
//    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
//
//    /* disable progress meter, set to 0L to enable and disable debug output */
//    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
//
//    /* send all data to this function  */
//    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
//
//    /* open the file */
//    pagefile = fopen(pagefilename, "wb");
//    if (pagefile) {
//
//        /* write the page body to this file handle */
//        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
//
//        /* get it! */
//        curl_easy_perform(curl_handle);
//
//        /* close the header file */
//        fclose(pagefile);
//    }
//
//    /* cleanup curl stuff */
//    curl_easy_cleanup(curl_handle);
//
//    curl_global_cleanup();
//
//    return 0;
//}