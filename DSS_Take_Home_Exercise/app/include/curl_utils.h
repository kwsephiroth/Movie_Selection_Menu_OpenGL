/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2020, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
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
  * Shows how the write callback function can be used to download data into a
  * chunk of memory instead of storing it in a file.
  * </DESC>
  */
#ifndef CURL_UTILS_H_
#define CURL_UTILS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <memory>

namespace curl_utils
{
    struct MemoryStruct {
        char* memory;
        size_t size;
    };

    static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
    {
        size_t realsize = size * nmemb;
        struct MemoryStruct* mem = (struct MemoryStruct*)userp;

        char* ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
        if (ptr == NULL) {
            /* out of memory! */
            printf("not enough memory (realloc returned NULL)\n");
            return 0;
        }

        mem->memory = ptr;
        memcpy(&(mem->memory[mem->size]), contents, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;

        return realsize;
    }

    static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream)
    {
        size_t written = fwrite(ptr, size, nmemb, (FILE*)stream);
        return written;
    }

    static std::unique_ptr<MemoryStruct> download_file_to_memory(const char* file_url)
    {
        CURL* curl_handle;
        CURLcode res;

        //struct MemoryStruct chunk;
        auto chunk = std::make_unique<MemoryStruct>();

        chunk->memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
        chunk->size = 0;    /* no data at this point */

        curl_global_init(CURL_GLOBAL_ALL);

        /* init the curl session */
        curl_handle = curl_easy_init();

        /* specify URL to get */
        curl_easy_setopt(curl_handle, CURLOPT_URL, file_url);

        /* send all data to this function  */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)chunk.get());

        /* some servers don't like requests that are made without a user-agent
           field, so we provide one */
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        /* get it! */
        res = curl_easy_perform(curl_handle);

        /* check for errors */
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
            return nullptr;
        }
        //else {
        //    /*
        //     * Now, our chunk.memory points to a memory block that is chunk.size
        //     * bytes big and contains the remote file.
        //     *
        //     * Do something nice with it!
        //     */

        //    printf("%lu bytes retrieved\n", (unsigned long)chunk->size);
        //}

        /* cleanup curl stuff */
        curl_easy_cleanup(curl_handle);

        /* we're done with libcurl, so clean it up */
        curl_global_cleanup();

        return chunk;
    }

    static void download_file_to_disk(const char* file_url, const char* out_file_name)
    {
        CURL* curl_handle;
        const char* pagefilename = out_file_name;
        FILE* pagefile;

        curl_global_init(CURL_GLOBAL_ALL);

        /* init the curl session */
        curl_handle = curl_easy_init();

        /* set URL to get here */
        curl_easy_setopt(curl_handle, CURLOPT_URL, file_url);

        /* Switch on full protocol/debug output while testing */
        curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

        /* disable progress meter, set to 0L to enable it */
        curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

        /* send all data to this function  */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

        /* open the file */
        pagefile = fopen(pagefilename, "wb");
        if (pagefile) {

            /* write the page body to this file handle */
            curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);

            /* get it! */
            curl_easy_perform(curl_handle);

            /* close the header file */
            fclose(pagefile);
        }

        /* cleanup curl stuff */
        curl_easy_cleanup(curl_handle);

        curl_global_cleanup();
    }
}
#endif