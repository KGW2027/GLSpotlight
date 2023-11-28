#pragma once
#include <cstdio>
#include <string>

typedef wchar_t* PWCHAR;
typedef char* PCHAR;
typedef std::string STR;
typedef unsigned int uint;

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
    static void write_stft_result(double** data, const char* file_name);
    static void write_audio_file(double* data, const char* file_name);

    static double* read_audio_export(const char* file_path, uint* length);

    static void check_min_max(double* arr, uint len, double* min, double* max);
    
};
