#include "log.h"
#include <stdio.h>
#include <string>
using std::string;

int LOG::RANK = 1;
bool LOG::isWrite = false;
char LOG::type_name[][10] = {"Info.", "Warn.", "Error."};

void LOG::show(const char *cpp, const int line,  int type, const char* msg)
{
    
    printf("%s:%d [%s] %s\n", cpp, line, LOG::type_name[type], msg);
     
    if(LOG::isWrite)
        writeToLogFile();
}


void LOG::Info(const char *cpp, const int line, const char* msg)
{
    if(RANK >= 1)
        show(cpp, line, 0, msg);
}


void LOG::Info(const char *cpp, const int line, const char* msg, int param1)
{
    if(RANK >= 1)
    {
        char new_msg[64];
        sprintf(new_msg, msg, param1);
        show(cpp, line, 0, new_msg);
    }
        
}


void LOG::Warn(const char *cpp, const int line, const char* msg)
{
    if(RANK >= 2)
        show(cpp, line, 1, msg);
}


void LOG::Warn(const char *cpp, const int line, const char* msg, int param1)
{
    if(RANK >= 2)
    {
        char new_msg[64];
        sprintf(new_msg, msg, param1);
        show(cpp, line, 1, new_msg);
    }
        
}

void LOG::writeToLogFile()
{

}