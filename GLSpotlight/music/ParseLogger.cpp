#include "ParseLogger.h"

#include <chrono>

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
