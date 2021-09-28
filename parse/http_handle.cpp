#include "http_handle.h"

#include "httpparser/httprequestparser.h"
#include "httpparser/httpresponseparser.h"
#include "../log.h"

#include <fstream>
#include <iostream>

static httpparser::Response response;

std::string readHtmlFile(const char* path)
{
    std::ifstream fin(path);
    std::istreambuf_iterator<char> begin(fin);
    std::istreambuf_iterator<char> end;

    std::string content(begin, end);
    return content;
}


httpparser::Response setRespone_index()
{
    httpparser::Response res;
    const char root_path[] = "/root/cathttpserver/version_0.2.1/html/hello.html";
    
    res.versionMajor = 1;
    res.versionMinor = 1;
    res.statusCode = 200;
    res.status = "OK";
    res.keepAlive = true;

    std::string body = readHtmlFile(root_path);
    int left_count = 0; 
    /*
        这段代码主要是去除 html 中的多余空格
    */
    for(auto &p : body)
    {
        if(p == ' ' && left_count == 0)
            continue;
        else if(p == '<')
        {
            res.content.push_back(p);
            left_count++;
        }
        else if(p == '>')
        {
            res.content.push_back(p);
            left_count--;
        }
        else
            res.content.push_back(p);
    }
        

    res.headers.push_back({"Server", "catHttpServer/0.2.1"});
    res.headers.push_back({"Content-Type", "text/html"});
    res.headers.push_back({"Content-Length", std::to_string(res.content.size())});
    res.headers.push_back({"charset", "utf-8"});
    res.headers.push_back({"Connection", "keep-alive"});
    
    return res;
}


httpparser::Response setRespone_test()
{
    httpparser::Response res;
    res.versionMajor = 1;
    res.versionMinor = 1;
    res.statusCode = 200;
    res.status = "OK";
    res.keepAlive = true;

    res.headers.push_back({"Server", "catHttpServer/0.2.1"});
    res.headers.push_back({"Content-Type", "text/html"});
    res.headers.push_back({"Content-Length", std::to_string(8)});
    res.headers.push_back({"charset", "utf-8"});
    res.headers.push_back({"Connection", "keep-alive"});
    
    std::string test = "<html />";
    for(auto &p : test)
        res.content.push_back(p);

    return res;
}


std::string http_request_handle(const char *request_context, ssize_t size)
{
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
                // return setRespone_test().inspect();
                const char text[] =
                    "HTTP/1.1 200 OK\r\n"
                    "Server: nginx/1.2.1\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: 8\r\n"
                    "Connection: keep-alive\r\n"
                    "\r\n"
                    "<html />";
                return text;
                break;
            }
            if(req.uri == "/index.html")
            {
                return setRespone_index().inspect();
                break;
            }

            return "No Http";
            break;

        } while (false);

    }

    Debug("%s:%d 报文不是Http\n", __FILE__, __LINE__);
    return "No Http";
}