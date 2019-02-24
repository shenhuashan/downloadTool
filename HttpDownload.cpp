//
// Created by tanwei on 19-2-23.
//
#include <fstream>
#include <cstdio>
#include <iostream>
#include <zconf.h>
#include <thread>
#include <cstring>
#include "HttpDownload.h"
HttpDownload::HttpDownload(const string& url, const string& downloadPath, const unsigned int& threadNum) {
    this->url = url;
    this->threadNums = threadNum;
    curl_easy_setopt(curlHandle, CURLOPT_URL, this->url.c_str());
    initFileName();
    string realPath = downloadPath + fileName;
    FILE * temp = fopen(realPath.c_str(), "wb");
    if (temp)
    {
        downloadFileHandle = temp;
    }
}

bool HttpDownload::multiThreadDownload() {
    thread threads[threadNums];
    fileSize = getFileSize();

    if (fileSize <= 0)
    {
        cout << "get the file length error..." << endl;
        return false;
    }
    cout << "fileSize:"<<fileSize<<endl;

    long partSize = fileSize / threadNums;

    for (int i = 0; i <= threadNums; i++)
    {
        auto *threadData = new ThreadData ();

        if (i < threadNums)
        {
            threadData->startPos = i * partSize;
            threadData->endPos = (i + 1) * partSize - 1;
        }
        else
        {
            if (fileSize % threadNums != 0)
            {
                threadData->startPos = i * partSize;
                threadData->endPos = fileSize - 1;
            }
            else
                break;
        }

        CURL *curl = curl_easy_init ();

        threadData->curlHandle = curl;
        threadData->fileHandle = downloadFileHandle;

        char range[64] = { 0 };
        snprintf (range, sizeof (range), "%lld-%lld", threadData->startPos, threadData->endPos);

        curl_easy_setopt (curl, CURLOPT_URL, url.c_str ());
        curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, writeData);
        curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) threadData);
        curl_easy_setopt (curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt (curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
        curl_easy_setopt (curl, CURLOPT_LOW_SPEED_TIME, 5L);
        curl_easy_setopt (curl, CURLOPT_RANGE, range);

        threads[i] = thread(executeThread, threadData);
    }

    for (int i = 0; i < threadNums; ++i) {
        threads[i].join();
    }

    fclose (downloadFileHandle);

    cout << "download succeed!" << endl;
    return true;
}

HttpDownload::~HttpDownload() {
    if (downloadFileHandle) {
        fclose(downloadFileHandle);
    }
}


void HttpDownload::initFileName() {
    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_HEADERFUNCTION, parseHeader);
    curl_easy_setopt(curlHandle, CURLOPT_HEADERDATA, &fileName);

    curl_easy_perform(curlHandle);

    if (fileName.empty())
    {
        getNameFromUrl();
    }

}


size_t HttpDownload::parseHeader(void *ptr, size_t size, size_t nmemb, void *stream) {
    const size_t  cb = size * nmemb;
    const char *hdr_str= (char*)ptr;
    char const * const tag = "Content-Disposition:";

    /* Example:
     * ...
     * Content-Type: text/html
     * Content-Disposition: filename=name1367; charset=funny; option=strange
     */
    if (strstr(hdr_str, "Content-Disposition:")) {
        cout << "has c-d: "<< hdr_str << endl;
    }

    if (!strncasecmp(hdr_str, tag, strlen(tag))) {
        int ret = getNameFromTag(hdr_str+strlen(tag), (char *)stream);
        if (ret) {
            cout << "ERR: bad remote name" << endl;
        }
    }

    return cb;
}

string HttpDownload::getNameFromUrl() {
    string tempUrl = url;
    unsigned long lastPosition = tempUrl.find_last_of("/");
    fileName = tempUrl.substr(lastPosition+1);
}


size_t HttpDownload::getNameFromTag(const char * cd, char *name) {
    const char * key = "filename=";
    size_t ret = 0;
    char *val = NULL;
    /* Example Content-Disposition: filename=name1367; charset=funny; option=strange */

    val = const_cast<char *>(strcasestr(cd, key));
    if (!val) {
        return ret;
    }

    val += strlen(key);

    while (*val != '\0' && *val != ';') {
        *name++ = *val++;
    }
    *name = '\0';
    return ret;
}