#ifndef UTIL
#define UTIL
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

using std::string;
using std::cout;
using std::cin;
using std::endl;

const int MAXREADTIME = 5;

void handle_for_sigpipe();
ssize_t readn(int fd, void *buff, size_t n);
ssize_t writen(int fd, const void *buff, size_t n);
ssize_t readET(int fd, string& content);
#endif