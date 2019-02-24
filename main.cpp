#include <iostream>
#include "Download.h"
#include "HttpDownload.h"
int main() {
    Download *download = new HttpDownload("http://localhost/test/curl-7.30.0.tar.gz", "/home/tanwei/", 5);
//    Download *download = new HttpDownload("http://localhost/test/curl-7.30.0.tar.gz", "/home/tanwei/", 5);
    delete download;
}