﻿#define _CRT_SECURE_NO_WARNINGS

#include "WaveReader.h"

#include <algorithm>
#include <assert.h>
#include <string>


void WaveReader::fill_buffer()
{
    if(buffer_ == nullptr)
        buffer_ = new uint8[BUFFER_SIZE];
    buffer_length_ = fread(buffer_, sizeof(uint8), BUFFER_SIZE, target_file_);
}

uint8 WaveReader::read()
{
    if(buffer_ == nullptr || file_ptr_ >= buffer_length_)
    {
        fill_buffer();
        file_ptr_ = 0;
    }
    
    if(buffer_length_ == 0) return 0;
    
    return buffer_[file_ptr_++];
}

void WaveReader::reads(uint8* arr, size_t len)
{
    for(size_t idx = 0 ; idx < len ; idx++)
        arr[idx] = read();
}

void WaveReader::read_header()
{
    header_ = WAVE_FMT();

    reads(header_.riff, 4);
    header_.file_size        = endian32();
    reads(header_.wave, 4);
    reads(header_.fmt, 4);
    header_.len1             = endian32();
    header_.format           = endian16();
    header_.num_channels     = endian16();
    header_.sample_rate      = endian32();
    header_.bit_rate         = endian32();
    header_.type             = endian16();
    header_.bit_per_sample   = endian16();

    // Check List Chunk
    uint8 list_test[4];
    reads(list_test, 4);
    if(!is_data(list_test))
    {
        uint32 list_length = endian32();
        file_ptr_ += list_length;
        reads(header_.data, 4);
    }
    else
    {
        for(size_t copy = 0 ; copy < 4 ; copy++)
            header_.data[copy] = list_test[copy];    
    }
    header_.data_size        = endian32();
}

uint16 WaveReader::endian16()
{
    uint8  sbuffer[2];
    reads(sbuffer, 2);
    
    uint16 d = 0;
    for(int shift = 0 ; shift < 2 ; shift++)
        d += static_cast<uint16>(sbuffer[shift]) << shift * 8;

    return d;
}

uint32 WaveReader::endian32()
{
    uint8  sbuffer[4];
    reads(sbuffer, 4);
    
    uint32 d = 0;
    for(int shift = 0 ; shift < 4 ; shift++)
        d += static_cast<uint32>(sbuffer[shift]) << shift * 8;

    return d;
}

uint64 WaveReader::endian64()
{
    uint8  sbuffer[8];
    reads(sbuffer, 8);
    
    uint64 d = 0;
    for(int shift = 0 ; shift < 8 ; shift++)
        d += static_cast<uint64>(sbuffer[shift]) << shift * 8;

    return d;
}

size_t WaveReader::get_data_count()
{
    return static_cast<size_t>(header_.data_size) / static_cast<size_t>(header_.bit_per_sample * header_.num_channels / 8);
}

WAVE_DATA WaveReader::parse_data_8bit()
{
    WAVE_DATA wave_data;
    wave_data.data_16 = nullptr;
    
    wave_data.data_8 = new int8*[header_.num_channels];
    for(size_t channel = 0 ; channel < header_.num_channels ; channel++)
    {
        wave_data.data_8[channel] = new int8[get_data_count()];
        for(size_t idx = 0 ; idx < get_data_count() ; idx++)
        {
            uint8 udata = read();
            int8  sdata;
            memcpy(&sdata, &udata, sizeof(int8));
            wave_data.data_8[channel][idx] = sdata;
        }
    }

    return wave_data;
}

WAVE_DATA WaveReader::parse_data_16bit()
{
    WAVE_DATA wave_data;
    wave_data.data_8 = nullptr;

    wave_data.data_16 = new int16*[header_.num_channels];
    for(size_t channel = 0 ; channel < header_.num_channels ; channel++)
    {
        wave_data.data_16[channel] = new int16[get_data_count()];
        for(size_t idx = 0 ; idx < get_data_count() ; idx++)
        {
            uint16 udata = endian16();
            int16  sdata;
            memcpy(&sdata, &udata, sizeof(int16));
            wave_data.data_16[channel][idx] = sdata;
        }
    }
    
    return wave_data;
    
}

double** WaveReader::normalize(WAVE_DATA data)
{
    double** norm = new double*[header_.num_channels];
    double int8_max = 1<<7, int16_max = 1<<15; 
    for(size_t channel = 0 ; channel < header_.num_channels ; channel++)
    {
        norm[channel] = new double[get_data_count()];
        for(size_t idx = 0 ; idx < get_data_count() ; idx++)
        {
            norm[channel][idx] = header_.bit_per_sample == 8
                               ? static_cast<double>(data.data_8[channel][idx] ) / int8_max
                               : static_cast<double>(data.data_16[channel][idx]) / int16_max; 
        }
    }

    return norm;
}

bool WaveReader::is_data(unsigned char* test_arr)
{
    return test_arr[0] == 'd' && test_arr[1] == 'a' && test_arr[2] == 't' && test_arr[3] == 'a';
}

void WaveReader::export_data(STFT_Out ext)
{
    FILE* exportFile;
    exportFile = fopen("../export_stft.txt", "w+");

    for(size_t widx = 0 ; widx < ext.size[0] ; widx++)
    {
        std::string str = std::to_string(ext.out[0][widx]);
        for(size_t sidx = 1 ; sidx < ext.size[1] ; sidx++)
        {
            str += "," + std::to_string(ext.out[sidx][widx]);  
        }
        str += "\n";
        fwrite(str.c_str(), sizeof(char), str.length(), exportFile);
        
        if(widx % 25 == 0)
            printf("Export Data (%llu / %d) ··· %.2f%%\n", widx, ext.size[0], static_cast<double>(widx * 100) / static_cast<double>(ext.size[0]));
    }
    fflush(exportFile);
    fclose(exportFile);
}

void WaveReader::concat()
{
    STFT_Out base = out_[0];

    // Compress Base
    for(size_t idx = 0 ; idx < base.size[1] / 2 ; idx++)
        for(size_t fidx = 0 ; fidx < base.size[0] ; fidx++)
            base.out[idx][fidx] = (base.out[idx*2][fidx] + base.out[idx*2+1][fidx]) / 2.0;

    STFT_Out next = out_[1];
    size_t offset = base.size[1] / 2;
    for(size_t idx = 0 ; idx < next.size[1] / 2 ; idx++)
        for(size_t fidx = 0 ; fidx < next.size[0] ; fidx++)
            base.out[idx + offset][fidx] = (next.out[idx*2][fidx] + next.out[idx*2+1][fidx]) / 2.0;
    
}

WaveReader::WaveReader(wchar_t* file_path)
{
    file_path_ = file_path;
    target_file_ = _wfopen(file_path, L"rb");
    assert(target_file_ != nullptr);

    file_ptr_ = 0;
    read_header();

    WAVE_DATA parse_data = header_.bit_per_sample == 8 ? parse_data_8bit() : parse_data_16bit();
    double**  norm_data  = normalize(parse_data);
    free_wave_data(parse_data);

    for(size_t channel = 0 ; channel < header_.num_channels ; channel++)
    {
        STFT_Setting stft_settings;
        stft_settings.in       = norm_data[channel];
        stft_settings.in_len   = get_data_count();
        stft_settings.hop_len  = 512;
        stft_settings.win_len  = 2048;
        STFT_Out out = FourierLib::stft(stft_settings);
        // FourierLib::amp_to_mel(out, header_.sample_rate);
        out_.push_back(out);
    }
    concat();
    // export_data(out_[0]);
    free(norm_data);

    print_info();
}

void WaveReader::print_info()
{
    printf("===== ===== [WAVE FILE INFO] ===== =====\n");
    printf("File Dir : %ls\n", file_path_);
    printf("Num Of Channels : %hd\n", header_.num_channels);
    printf("Sample Rate : %d\n", header_.sample_rate);
    printf("Bit Per Sample : %hd\n", header_.bit_per_sample);
    printf("Data Size : %d\n", header_.data_size);
    printf("Music Length : %.2lf secs\n", get_music_length());
    printf("===== ===== ===== == == ===== ===== =====\n");
}

MUSIC_PROCESSOR WaveReader::make_processor()
{
    uint32 estimate_time_len = static_cast<uint32>(ceil((get_data_count() - 2048) / 512 + 1));
    
    MUSIC_PROCESSOR processor{
        0,
        1024,
        estimate_time_len,
        static_cast<uint64>(get_music_length() * 10'000'000),
        nullptr
    };

    return processor;
}

void WaveReader::clear()
{
    if(target_file_ != nullptr)
        fclose(target_file_);
    free(buffer_);
    // free(file_path_);
}

void WaveReader::free_wave_data(WAVE_DATA data)
{
    for(size_t idx = 0 ; idx < header_.num_channels ; idx++)
    {
        if(data.data_8  != nullptr) free(data.data_8 [idx]);
        if(data.data_16 != nullptr) free(data.data_16[idx]);
    }

    if(data.data_8  != nullptr) free(data.data_8 );
    if(data.data_16 != nullptr) free(data.data_16);
}
