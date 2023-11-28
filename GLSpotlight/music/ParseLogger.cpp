#include "ParseLogger.h"

#include <chrono>
#include <vector>

ParseLogger::ParseLogger()
{
    auto now_time = std::chrono::system_clock::now();
    auto now = now_time.time_since_epoch().count();
    printf("Try to load logger\n");
    
    if(!fopen_s(&LOG_FILE, ("../" + std::to_string(now) + ".txt").c_str(), "w+"))
    {
        printf("Log File Successfully Loaded!\n");
    }
}

void ParseLogger::write_flush()
{
    fflush(getInstance().LOG_FILE);
}

void ParseLogger::write_stft_result(double** data, const char* file_name)
{ 
}

void ParseLogger::write_audio_file(double* data, const char* file_name)
{
    FILE* expt_file;
    char path[100];
    strcpy_s(path, "../");
    strcat_s(path, file_name);
    strcat_s(path, ".txt");
    if(!fopen_s(&expt_file, path, "w+"))
    {
        
    }
    fflush(expt_file);
    fclose(expt_file);
}

double* ParseLogger::read_audio_export(const char* file_path, uint* len)
{
    FILE                    *file;
    size_t                  buffer_size = (1<<12)+1;
    char                    *buffer     = new char[buffer_size];
    size_t                  length;

    std::vector<double>     audio_datas;
    double                  read_cache  = 0.0;
    bool                    floating    = false;
    int                     floating_i  = 0;

    if(fopen_s(&file, file_path, "r"))
    {
        printf("Not found file :: %s\n", file_path);
        return nullptr;
    }

    while((length = fread_s(buffer, buffer_size, sizeof(char), (buffer_size - 1) / sizeof(char), file)) > 0)
    {
        for(uint idx = 0 ; idx < length ; idx++)
        {
            switch(const char c = buffer[idx])
            {
            case ',':
            case '\0':
            case '\n':
                audio_datas.push_back(read_cache);
                read_cache = 0.0;
                floating = false;
                continue;
            case '.':
                floating = true;
                floating_i = 1;
            case '\r':
                continue;
            default:
                if(floating)
                {
                    read_cache += (c & 0xF) * pow(10, floating_i--);
                }
                else
                {
                    read_cache *= 10;
                    read_cache += c & 0xF;
                }
            }
        }
    }
    
    *len = static_cast<uint>(audio_datas.size());

    double* retv = new double[audio_datas.size()];
    memcpy(retv, audio_datas.data(), sizeof(double) * audio_datas.size());
    return retv;
}

void ParseLogger::check_min_max(double* arr, uint len, double* min, double* max)
{
    for(uint idx = 0 ; idx < len ; idx++)
    {
        *min = std::min(*min, arr[idx]);
        *max = std::max(*max, arr[idx]);
    }
}


void ParseLogger::write_wchr(PWCHAR context)
{
    fwrite(context, sizeof(PWCHAR), wcslen(context), getInstance().LOG_FILE);
}

void ParseLogger::write_chr(PCHAR context)
{
    fwrite(context, sizeof(PCHAR), strlen(context), getInstance().LOG_FILE);
}

void ParseLogger::write_str(std::string context)
{
    fwrite(context.c_str(), sizeof(char), context.length(), getInstance().LOG_FILE);
}
