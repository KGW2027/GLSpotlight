#pragma once

#include <functional>
#include <iostream>
#include <windows.h>

#include <mfapi.h>
#include <mfidl.h>

#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mf")
#pragma comment(lib, "ole32")

#include <fftw3.h>
#include <thread>
#include <vector>

typedef float* read_data;
typedef float** result;
typedef const wchar_t* file_path;

#define WINDOW_SIZE 2048
#define HOP_SIZE 512
#define AMIN 1e-8

class MusicReader
{
    file_path   path_;
    read_data   data_;
    result      result_;
    DWORD       data_len_, sample_rate_;
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

    double combine_float64(BYTE* array, DWORD* idx);
    float combine_float32(BYTE* array, DWORD* idx);
    DWORD combine_int32(BYTE* array, DWORD* idx);
    WORD combine_int16(BYTE* array, DWORD* idx);

    bool check_header(BYTE one, BYTE two)
    {
        return one == 0xFF && ((two & 0xF0) == 0xF0 || (two & 0xE0) == 0xE0);
    }
    
    void read_file();

    void stft();

    void play_music_internal(void (*callback)(float** array, int len));
public:
    explicit MusicReader(file_path path);

    void play_music(void (*callback)(float** array, int len));
    
    result output(int** length);
};
