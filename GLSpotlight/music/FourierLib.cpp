#include "FourierLib.h"

#include <cassert>
#include <complex>
#include <fftw3.h>
#include <functional>
#include <iostream>
#include <windows.h>
#include <complex>


#define PI 3.141592

void FourierLib::w_hann(double* out, uint window_length)
{
    for (uint idx = 0 ; idx < window_length ; idx++)
        out[idx] = 0.5 * (1.0 - std::cos(2.0 * PI * idx / (window_length - 1)));
}

void FourierLib::w_hamming(double* out, uint window_length)
{
    for(uint idx = 0 ; idx < window_length ; idx++)
        out[idx] = 0.54 - 0.46 * std::cos(2 * PI * idx / (window_length - 1));
}

double FourierLib::mel_to_hz(double mel)
{
    return 700. * pow(10.0, mel / 2595.) - 1.0;
}

double FourierLib::hz_to_mel(double hz)
{
    return 2595. * log10(1.0 + hz / 700.);
}

filter_bank FourierLib::make_mel_fb(uint num_filters, uint fft_size, uint sr)
{
    filter_bank filter_bank;

    double ms = hz_to_mel(0.0);
    double me = hz_to_mel(static_cast<double>(sr) / 2.0);

    num_filters++;
    for(int i = 0 ; i <= num_filters ; i++)
    {
        double mel = ms + i * (me - ms) / num_filters;
        filter_bank.push_back(mel_to_hz(mel));
    }

    return filter_bank;
}

dB_Out FourierLib::amp_to_dB(dB_In params)
{
    double  **out   = new double*[params.frame_count];
    double  ref     = -999, amin    = 1e-10,    norm_max    = -999;

    // Get Maximum Value (Ref)
    for(uint idx = 0 ; idx < params.frame_count ; idx++)
    {
        out[idx]    = new double[params.window_size];
        for(uint widx = 0 ; widx < params.window_size ; widx++)
        {
            if(params.in[idx][widx] < 0) params.in[idx][widx] = 0;
            
            ref            = max(ref, params.in[idx][widx]);
            out[idx][widx] = 10 * log10(max(amin, pow(params.in[idx][widx], 2)));
        }
    }

    // Normalized
    ref     = 10. * log10(max(amin, pow(ref, 2)));
    
    for(uint idx = 0 ; idx < params.frame_count ; idx++)
    {
        for(uint widx = 0 ; widx < params.window_size ; widx++)
        {
            out[idx][widx] -= ref;
            norm_max = max(norm_max, out[idx][widx]);
        }
    }

    // Low Cut
    for(uint idx = 0 ; idx < params.frame_count ; idx++)
        for(uint widx = 0 ; widx < params.window_size ; widx++)
            out[idx][widx]  = max(out[idx][widx], norm_max - params.top_dB);

    // Return
    dB_Out result;
    result.out = out;
    result.size = new uint[]{params.window_size, params.frame_count};
    return result;
}

void FourierLib::amp_to_mel(STFT_Out stft, uint sr)
{
    const uint num_filters = 20;
    const uint fft_size = stft.size[0];

    filter_bank fb = make_mel_fb(num_filters, fft_size, sr);

    for(size_t tidx = 0 ; tidx < stft.size[1] ; tidx++)
    {
        filter_bank result(num_filters, 0.0);

        for(uint i = 1 ; i < num_filters ; i++)
        {
            for(uint j = 0 ; j < fft_size ; j++)
            {
                double hz = static_cast<double>(j) / (fft_size / 2) * (sr / 2);
                if (hz >= fb[i - 1] && hz <= fb[i + 1]) {
                    double weight = (hz - fb[i - 1]) / (fb[i] - fb[i - 1]);
                    result[i-1] += weight * stft.out[tidx][j];
                } else if (hz >= fb[i] && hz <= fb[i + 2]) {
                    double weight = (fb[i + 2] - hz) / (fb[i + 2] - fb[i]);
                    result[i-1] += weight * stft.out[tidx][j];
                }
            }
        }
    }
}

double* FourierLib::padding(uint pad_l, uint pad_r, double* arr, uint offset, uint len)
{
    uint tlen = pad_l + pad_r + len;
    double* pad = new double[tlen];
    for(uint idx = 0 ; idx < tlen ; idx++)
        pad[idx] = pad_l <= idx && idx < pad_l + len? arr[offset+idx-pad_l] : 0;
    
    return pad;
}

double** FourierLib::frame(double* arr, uint len, uint win_len, uint hop_len, uint* shape)
{
    assert(len > win_len);
    uint        frame_size = (len - win_len) / hop_len + 1;

    assert(frame_size > 0);
    double      **frames   = new double*[frame_size];
    
    for(uint idx = 0 ; idx < frame_size ; idx++)
    {
        frames[idx] = new double[win_len];
        uint offset = hop_len * idx;

        for(uint sidx = 0 ; sidx < win_len ; sidx++)
        {
            frames[idx][sidx] = arr[offset + sidx];
        }
    }

    shape[0] = win_len;
    shape[1] = frame_size;

    return frames;
}

STFT_Out FourierLib::stft(STFT_Setting params)
{
    // Init Window
    double* fft_window = new double[params.win_len];
    w_hann(fft_window, params.win_len);

    // Padding
    uint pad_l, pad_r, forward_k, backward_k, offset;
    pad_l       = params.win_len / 2;
    pad_r       = 0;
    forward_k   = static_cast<uint>(ceil(params.win_len / 2.0 / params.hop_len));
    backward_k  = (params.in_len + params.win_len/2 - params.win_len) / params.hop_len + 1;

    double *y_pre, *y_post, **y_frame_pre, **y_frame_post;
    uint   pad_len, extra;
    uint   *pre_frame_size = (uint*) malloc(sizeof(uint) * 2);
    uint   *post_frame_size = (uint*) malloc(sizeof(uint) * 2);
    pad_len     = (forward_k - 1) * params.hop_len - params.win_len / 2 + params.win_len + 1;
    y_pre       = padding(pad_l, pad_r, params.in, 0, pad_len);
    y_frame_pre = frame(y_pre, pad_l + pad_r + pad_len, params.win_len, params.hop_len, pre_frame_size);
    
    if (backward_k * params.hop_len - params.win_len / 2 + params.win_len <= params.in_len * params.win_len / 2)
    {
        pad_l        = 0;
        pad_r        = params.win_len / 2;
        offset       = backward_k * params.hop_len - params.win_len / 2;
        y_post       = padding(pad_l, pad_r, params.in, offset, params.in_len - offset);
        y_frame_post = frame(y_post, pad_l + pad_r + params.in_len - offset, params.win_len, params.hop_len, post_frame_size);
    }
    else
    {
        y_frame_post    = new double*[0];
        y_post          = new double[0];
        post_frame_size = pre_frame_size;
    }
    extra = pre_frame_size[1] + post_frame_size[1];
    
    // Fourier Transform Init
    double       **result;
    uint         *out_frame_size  = (uint*) malloc(sizeof(uint) * 2);
    double       **audio_frames   = frame(params.in, params.in_len, params.win_len, params.hop_len, out_frame_size);
    double       *in              = (double*)       fftw_malloc(sizeof(double) * params.win_len);
    fftw_complex *out             = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * params.win_len);

    out_frame_size[0] = 1 + params.win_len / 2;
    out_frame_size[1] += extra;

    // Init Matrix
    result = new double*[out_frame_size[1]];
    for(uint idx = 0 ; idx < out_frame_size[1] ; idx++)
        result[idx] = new double[out_frame_size[0]];

    fftw_plan plan = fftw_plan_dft_r2c_1d(static_cast<int>(params.win_len), in, out, FFTW_ESTIMATE); 

    // Warm-up [FFT y_pre]
    for(uint frame_idx = 0 ; frame_idx < pre_frame_size[1] ; frame_idx++)
    {
        for(uint in_idx = 0 ; in_idx < params.win_len ; in_idx++)
            in[in_idx] = fft_window[in_idx] * y_frame_pre[frame_idx][in_idx];
        
        fftw_execute(plan);
    
        for(uint in_idx = 0 ; in_idx < out_frame_size[0] ; in_idx++)
            result[frame_idx][in_idx] = sqrt(out[in_idx][0] * out[in_idx][0] + out[in_idx][1] * out[in_idx][1]);
    }
    
    // Warm-up [FFT y_post]
    if(post_frame_size != nullptr && post_frame_size[1] > 0)
    {
        for(uint frame_idx = 0 ; frame_idx < post_frame_size[1] ; frame_idx++)
        {
            uint real_frame_idx = out_frame_size[1] - (post_frame_size[1] - frame_idx);
            
            for(uint in_idx = 0 ; in_idx < params.win_len ; in_idx++)
                in[in_idx] = fft_window[in_idx] * y_frame_post[frame_idx][in_idx];
        
            fftw_execute(plan);

            for(uint in_idx = 0 ; in_idx < out_frame_size[0] ; in_idx++)
                result[real_frame_idx][in_idx] = sqrt(out[in_idx][0] * out[in_idx][0] + out[in_idx][1] * out[in_idx][1]);
        }
    }

    double mmin = DBL_MAX, mmax = -DBL_MAX;
    
    // FFT Body
    for(uint frame_idx = 0 ; frame_idx < out_frame_size[1] - extra ; frame_idx++)
    {
        for(uint in_idx = 0 ; in_idx < params.win_len ; in_idx++)
            in[in_idx] = fft_window[in_idx] * audio_frames[frame_idx][in_idx];

        fftw_execute(plan);
        for(uint in_idx = 0 ; in_idx < out_frame_size[0] ; in_idx++)
        {
            result[pre_frame_size[1] + frame_idx][in_idx] = sqrt(out[in_idx][0] * out[in_idx][0] + out[in_idx][1] * out[in_idx][1]);
            mmin = min(mmin, result[pre_frame_size[1] + frame_idx][in_idx]);
            mmax = max(mmax, result[pre_frame_size[1] + frame_idx][in_idx]);
        }
    }


    STFT_Out output;
    output.out = result;
    output.size = new uint[]{out_frame_size[0], out_frame_size[1]};
    
    // Memory Clean
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
    
    delete[] fft_window;
    delete[] y_pre;
    delete[] y_frame_pre;
    delete[] y_post;
    delete[] y_frame_post;
    delete[] audio_frames;
    free(pre_frame_size);
    free(post_frame_size);
    free(out_frame_size);
    
    return output;
}
