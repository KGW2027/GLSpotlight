#pragma once

typedef unsigned char        uint8 ;
typedef unsigned short       uint16;
typedef unsigned int         uint32;
typedef unsigned long long   uint64;
typedef char                 int8  ;
typedef short                int16 ;



struct MUSIC_PROCESSOR
{
    uint32     index    ;
    uint32     win_len  ;
    uint32     time_len ;
    uint64     length   ;
    double**   data     ;
};

struct WAVE_DATA
{
    // [Discrete Time][Channels], S = Bit per Sample
    
    int8**  data_8;
    int16** data_16;
};

struct WAVE_FMT
{
    uint8   riff[4];
    uint32  file_size;
    uint8   wave[4];
    uint8   fmt [4];
    
    uint32  len1;
    uint16  format;
    uint16  num_channels;
    uint32  sample_rate;
    uint32  bit_rate;
    uint16  type;
    uint16  bit_per_sample;

    uint8   data[4];
    uint32  data_size;
};