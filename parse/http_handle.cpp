#include "http_handle.h"

#include "httpparser/httprequestparser.h"
#include "httpparser/httpresponseparser.h"
#include "../log.h"

#include <fstream>
#include <iostream>

std::string readHtmlFile(const char* path)
{
    std::ifstream fin(path);
    std::istreambuf_iterator<char> begin(fin);
    std::istreambuf_iterator<char> end;

    std::string content(begin, end);
    return content;
}


std::string http_request_handle(const char *request_context, ssize_t size)
{
    const char root_path[] = "/root/cathttpserver/version_2.1/html/hello_world.html";

    std::string http_context;
    httpparser::Request req;
    httpparser::HttpRequestParser parser;
    httpparser::HttpRequestParser::ParseResult status = parser.parse(req, request_context, request_context + size);
    if(status == httpparser::HttpRequestParser::ParsingCompleted)
    {
        do
        {
            if(req.uri == "/")
            {
                httpparser::Response response;
                
                char _header[] =
                        "HTTP/1.1 200 OK\r\n"
                        "Server: catHttpServer/0.2.1\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: %d\r\n"
                        "charset: utf-8"
                        "Connection: keep-alive\r\n"
                        "\r\n";
                std::string body = readHtmlFile(root_path);
                Debug("%s:%d 网页长度 = %d\n", __FILE__, __LINE__, (int)body.size());
                char header[1024];
                sprintf(header, _header, body.size());
                std::cout << header << std::endl;
                http_context += header;
                http_context += body;
                return http_context;
                break;
            }
            if(req.uri == "/index.html")
            {
                return "No Http";
                break;
            }
            break;
        } while (false);

    }

    Debug("%s:%d 报文不是Http\n", __FILE__, __LINE__);
    return std::string("No Http");
}