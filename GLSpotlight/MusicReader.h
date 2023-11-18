#pragma once

#include <functional>
#include <iostream>
#include <windows.h>
#include <mfapi.h>
#include <mfidl.h>

#include <fftw3.h>

#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mf")
#pragma comment(lib, "ole32")

typedef float* read_data;
typedef float** result;
typedef const wchar_t* file_path;

class MusicReader
{
    file_path PATH;
    read_data DATA;
    result RESULT;
    UINT32 data_len, sample_rate;
    bool is_valid;
    
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

    // double* byte_array_to_double(BYTE* arr, DWORD len);
    //
    // bool ends_with(const wchar_t* str, const wchar_t* suffix);

    // // DEPRECATE
    // double* fft_audio_data(IMFSample* sample, int* len);
    //
    // // DEPRECATE
    // int compress_idx(int idx, int section_gap);
    //
    // // DEPRECATE
    // void compress_dB(double* data, int len, double** out, int* new_len);

    float combine_float(BYTE* array, int idx);

    bool check_header(BYTE one, BYTE two)
    {
        return one == 0xFF && ((two & 0xF0) == 0xF0 || (two & 0xE0) == 0xE0);
    }

    double combine_double(BYTE* array, int idx);
    
    void read_file();

    void stft(int window_size, int hop_size);

public:
    explicit MusicReader(file_path path);
    
};
