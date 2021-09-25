#ifndef __LOG_H__
#define __LOG_H__

#define __DEBUG__
#ifdef __DEBUG__
#define Debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define Debug(fmt, ...) 
#endif

class LOG
{
private:
    static char type_name[][10];
    static void show(const char *cpp, const int line,  int type, const char* msg);
    static void writeToLogFile();
public:
    /*
    RANK = 0: No Info
    RANK = 1: Info
    RANK = 2: Info + Warn
    */
    static int RANK; 
    static bool isWrite;
    static void Info(const char *cpp, const int line, const char* msg);
    static void Info(const char *cpp, const int line, const char* msg, int param1);
    static void Warn(const char *cpp, const int line, const char* msg);
    static void Warn(const char *cpp, const int line, const char* msg, int param1);
};





#endif
