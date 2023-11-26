#pragma once

#include <functional>
#include <iostream>
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <complex>

#include "FourierLib.h"

typedef double* read_data;
typedef double** result;
typedef const wchar_t* file_path;

#define WINDOW_SIZE 2048
#define HOP_SIZE 512
#define AMIN 1e-8

class MusicReader
{
    file_path   path_;
    read_data   data_;
    result      result_;
    DWORD       data_len_;
    DWORD       sample_rate_;
    LONGLONG    time_term_;
    LONGLONG    length_;
    UINT32      bit_depth_;
    UINT32      num_channels_;
    UINT32      num_chunks_;
    bool        is_valid_;
    
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
    
    double combine_audio_data(BYTE* array, DWORD* idx);
    void normalize(double* data);
    
    void read_file();
public:

    MusicReader(){}
    
    explicit MusicReader(file_path path);

    void play_music();
    
    result output(UINT32** length, LONGLONG* timestamp, LONGLONG* time_length);
};
