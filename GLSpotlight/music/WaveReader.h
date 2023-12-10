#pragma once
#include <cstdio>
#include <string>
#include <vector>

#include "FourierLib.h"
#include "MusicStructs.h"

#define BUFFER_SIZE 2048
typedef std::vector<STFT_Out> Out;

class WaveReader
{
    static bool debug_lock;
    
    wchar_t*      file_path_    ;
    FILE*         target_file_  ;
    uint8         *buffer_      ;
    size_t        buffer_length_;
    size_t        file_ptr_     ;
    
    WAVE_FMT        header_     ;
    Out             out_        ;

    void   fill_buffer();
    uint8  read();
    void   reads(uint8* arr, size_t len);
    void   read_header();

    uint16 endian16();
    uint32 endian32();
    uint64 endian64();

    size_t get_data_count();
    WAVE_DATA   parse_data_8bit ();
    WAVE_DATA   parse_data_16bit();

    double** normalize(WAVE_DATA data);
    bool     is_data(unsigned char* test_arr);
    void     concat();
    
public:

    WaveReader(wchar_t* file_path);

    std::vector<std::string> print_info();
    double  get_music_length() { return static_cast<double>(get_data_count()) / header_.sample_rate; }
    
    STFT_Out        get_stft_result()  { return out_[0]; }
    MUSIC_PROCESSOR make_processor();

    void    clear();
    void    free_wave_data(WAVE_DATA data);
    
};
