#ifndef HTTP_HANDLE
#define HTTP_HANDLE

#include "httpparser/response.h"
#include <string>

std::string http_request_handle(const char *request_context, ssize_t size);
httpparser::Response setRespone_test();
httpparser::Response setRespone_index();

#endif