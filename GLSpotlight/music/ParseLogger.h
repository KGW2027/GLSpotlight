#pragma once
#include <cstdio>
#include <string>

typedef wchar_t* PWCHAR;
typedef char* PCHAR;
typedef std::string STR;


class ParseLogger
{

    FILE        *LOG_FILE;

    
    ParseLogger();
    ~ParseLogger()
    {
        fclose(LOG_FILE);
    }

    ParseLogger(const ParseLogger&) = delete;
    ParseLogger& operator=(const ParseLogger&) = delete;

    
public:

    static ParseLogger& getInstance()
    {
        static ParseLogger instance_;
        return instance_;
    }

    static void write_wchr(PWCHAR context);
    static void write_chr(PCHAR context);
    static void write_str(STR context);
    static void write_flush();
    
};
