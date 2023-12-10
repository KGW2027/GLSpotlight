#pragma once
#define _ITERATOR_DEBUG_LEVEL 2

#include <functional>
#include <iostream>
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <complex>

#include "MusicStructs.h"

class WaveReader;
typedef const wchar_t* file_path;

class MusicReader
{
    file_path   path_           ;
    WaveReader  *wav_reader_    ;
    
    bool        is_terminated_  ;
    bool        is_ready_       ;
    bool        is_playing_     ;
    
        // HRESULT의 결과가 실패(<0)이면 오류 메세지 및 콜백 함수 실행
    void check(HRESULT result, const wchar_t* error_msg, void (*callback)() = {})
    {
        if(FAILED(result))
        {
            std::wcerr << "[Error code : " << result << " ] " << error_msg << std::endl;
            MFShutdown();
            callback();
            exit(result);
        }
    }
    
    bool is_file_exists(const wchar_t* filePath) {
        const DWORD file_attributes = GetFileAttributes(filePath);
        return file_attributes != INVALID_FILE_ATTRIBUTES && !(file_attributes & FILE_ATTRIBUTE_DIRECTORY);
    }
    
public:
    MUSIC_PROCESSOR processor_  ;
    
    MusicReader();

    void   play_music();
    void   terminate();
    void   set_path(const wchar_t* new_path);

    bool   is_ready();

};
