﻿#pragma once

#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mf")
#pragma comment(lib, "ole32")

typedef unsigned int uint;

struct STFT_Setting
{
    uint    hop_len;
    uint    win_len;
    uint    in_len;
    double  *in;
};

struct dB_In
{
    double  **in;
    uint    frame_count;
    uint    window_size;
    double  top_dB;
};

struct STFT_Out
{
    double  **out;
    uint    *size;
};

struct dB_Out
{
    double  **out;
    uint    *size;
};

class FourierLib
{
    static double*  padding(uint pad_l, uint pad_r, double* arr, uint offset, uint len);
    static double** frame(double* arr, uint len, uint win_len, uint hop_len, uint* shape);
    static void     w_hann(double* out, uint window_length);
    static void     w_hamming(double* out, uint window_length);
    
public:

    static STFT_Out stft(STFT_Setting params);
    static dB_Out   amp_to_dB(dB_In params);
};
