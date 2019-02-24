//
// Created by tanwei on 19-2-23.
//
#include <iostream>
#include "Download.h"
static mutex writeMutex;
Download::Download() {
    curlHandle = curl_easy_init();
}

Download::~Download() {
    cout<<"~Download"<< endl;
    curl_easy_cleanup(curlHandle);
}

bool Download::multiThreadDownload() {}

long long Download::getFileSize() {
    double FileLength = 0;
    curl_easy_setopt (curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt (curlHandle, CURLOPT_HEADER, 1);
    curl_easy_setopt (curlHandle, CURLOPT_NOBODY, 1);
    if (curl_easy_perform (curlHandle) == CURLE_OK)
    {
        curl_easy_getinfo (curlHandle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &FileLength);
    }
    else
    {
        FileLength = -1;
    }
    return static_cast<long long>(FileLength);
}


size_t Download::writeData(void *ptr, size_t size, size_t count, void *threadData)
{
    auto *data = (ThreadData *) threadData;
    size_t written = 0;
    writeMutex.lock();
    if (data->startPos + size * count <= data->endPos)
    {
        fseek (data->fileHandle, data->startPos, SEEK_SET);
        written = fwrite (ptr, size, count, data->fileHandle);
        data->startPos += size * count;
    }
    else
    {
        fseek (data->fileHandle, data->startPos, SEEK_SET);
        written = fwrite (ptr, 1, data->endPos - data->startPos + 1, data->fileHandle);
        data->startPos = data->endPos;
    }
    writeMutex.unlock();
    return written;
}

void Download::executeThread(void *arg) {
    auto *threadData = (ThreadData *) arg;

    curl_easy_perform (threadData->curlHandle);
    curl_easy_cleanup (threadData->curlHandle);
    delete threadData;


}

