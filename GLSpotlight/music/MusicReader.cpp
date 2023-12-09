#include "MusicReader.h"

#include <assert.h>
#include <mfreadwrite.h>
#include <thread>
#include <vector>
#include <fftw3.h>
#include <fstream>
#include <fstream>

#include "FourierLib.h"
#include "ParseLogger.h"

#define P_Integ_L 17292
#define P_Integ_S 1025

// Audio Play : https://stackoverflow.com/questions/65412545/how-to-increase-mp3-decoding-quality-media-foundation

MusicReader::MusicReader(const wchar_t* path) : data_(nullptr), data_len_(0), is_valid_(false)
{
    path_ = path;
    if (!is_file_exists(path_)) return;

    is_valid_ = true;

    // Read Audio Data
    read_file();
    
    // Experiment - Pycharm STFT Result export Integration [ wav5 - (20854), wav4 - (25423), wav6 - (17292) ]
    // const char    *file_path   = "F:/P_Programming/P_python/economyPract/export-stft-wav6.txt";
    //       double  **parse_arr  = new double*[0];
    //
    // FILE*   file;
    // if(!fopen_s(&file, file_path, "r"))
    //     parse_arr = experiment_read_line(file);
    //
    // dB_In db_cvt;
    // db_cvt.frame_count = P_Integ_L;
    // db_cvt.window_size = P_Integ_S;
    // db_cvt.top_dB      = 80.;
    // db_cvt.in          = parse_arr;
    // dB_Out db_out      = FourierLib::amp_to_dB(db_cvt);
    // result_     = db_out.out;
    // num_chunks_ = db_out.size[1];

    // Experiment - Pycharm Music Parse Integration
    // const char  *file_path  = "F:/P_Programming/P_python/economyPract/export-wav4.txt";
    // uint        parse_len   = 0;
    // double      *data       = ParseLogger::read_audio_export(file_path, &parse_len);
    
    //
    // STFT DATA
    STFT_Setting stft;
    stft.hop_len = HOP_SIZE;
    stft.win_len = WINDOW_SIZE;
    stft.in = data_;
    stft.in_len = data_len_;
    STFT_Out out = FourierLib::stft(stft);
    
    dB_In db_cvt;
    db_cvt.frame_count = out.size[1];
    db_cvt.window_size = out.size[0];
    db_cvt.top_dB = 80.;
    db_cvt.in = out.out;
    dB_Out db_out = FourierLib::amp_to_dB(db_cvt);
    result_     = db_out.out;
    num_chunks_ = db_out.size[1];

    // dB Parse : -80, -57.2536
    // norm parse : -80, -41.6194
    // de-norm parse : -80, -41.6194
    // my data : -39.2190, -39.9385
    // norm my data : -39.2190, -39.9385
}

double** MusicReader::experiment_read_line(FILE* file)
{
    size_t                      buffer_size = (1<<12)+1;
    char                        *buffer     = new char[buffer_size];
    size_t                      length;

    double                      read_cache  = 0.0;
    uint                        integ_key   = 0;
    uint                        inner_key   = 0;
    uint                        max_inner   = 0;
    double                      **integrated= new double*[P_Integ_L];
    bool                        floating    = false;
    int                         floating_i  = 0;
    
    for(uint idx = 0 ; idx < P_Integ_L ; idx++ )
        integrated[idx] = new double[P_Integ_S];
    
    while((length = fread_s(buffer, buffer_size, sizeof(char), (buffer_size - 1) / sizeof(char), file)) > 0)
    {
        for(uint idx = 0 ; idx < length ; idx++)
        {
            char    c   = buffer[idx];
            
            if(c == '\r') continue;

            if(c == ',' || c == '\0' || c == '\n')
            {
                integrated[inner_key++][integ_key] = read_cache;
                read_cache = 0.0;
                floating = false;

                if(c != ',')
                {
                    integ_key++;
                    if(c == '\0') break;
                    if(integ_key % 102 == 0) printf("%d / %d\n", integ_key, 1025);
                    max_inner = max(max_inner, inner_key);
                    inner_key = 0;
                }
                continue;
            }

            if(c == '.')
            {
                floating = true;
                floating_i = -1;
                continue;
            }

            if(floating)
            {
                read_cache += (c & 0xF) * pow(10, floating_i--);
            }
            else
            {
                read_cache *= 10;
                read_cache += c & 0xF;
            }
        }
    }

    printf("Shape : (%d, %d)\n", integ_key, max_inner);
    return integrated;
}

double MusicReader::combine_audio_data(BYTE* array, DWORD* idx)
{
    uint64_t combine = 0;
    double sum = 0.;

    for(UINT32 channel = 0 ; channel < num_channels_ ; channel++)
    {
        for(int shift = bit_depth_-8 ; shift >= 0 ; shift -= 8)
            combine |= static_cast<uint64_t>(array[(*idx)++]) << shift;
        sum += combine;
    }

    return sum / num_channels_;
}

void MusicReader::normalize(double* data)
{
    UINT64 num = (1 << bit_depth_) - 1;
    *data /= static_cast<double>(num);
}

void MusicReader::read_file()
{
    HRESULT hr = S_OK;
    
    hr = MFStartup(MF_VERSION);
    check(hr, L"MF Initiate Exception");

    IMFSourceReader* source_reader = nullptr;
    hr = MFCreateSourceReaderFromURL(path_, nullptr, &source_reader);
    check(hr, L"Create Source Reader Exception");
    source_reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, false);
    source_reader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, true);
    
    IMFMediaType* media_type = nullptr;
    hr = source_reader->GetNativeMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &media_type);
    check(hr, L"Get Native Media Type Exception");
    
    GUID major_type;
    DWORD sample_count;
    hr = media_type->GetGUID(MF_MT_MAJOR_TYPE, &major_type);
    if (SUCCEEDED(hr) && major_type == MFMediaType_Audio)
    {
        source_reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, media_type);
        sample_rate_ = MFGetAttributeUINT32(media_type, MF_MT_AUDIO_SAMPLES_PER_SECOND, 0);
        media_type->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &bit_depth_);
        media_type->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &num_channels_);
        PROPVARIANT duration;
        source_reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &duration);
        length_ = duration.hVal.QuadPart; // 100 ns 단위로 재생 시간 계산 (1,000,000,000 ns = 1 s, 1,000,000 ns = 1 ms)
        sample_count = static_cast<DWORD>(length_ * sample_rate_ / 10'000'000);
    }

    data_ = new double[sample_count]{0.f};
    data_len_ = 0;
    time_term_ = 0;
    LONGLONG bef_ts = 0;
    LONGLONG over = 0;
    // Source에서 샘플 읽기
    while (true)
    {
        
        IMFSample* sample;
        DWORD streamFlags;
        MFTIME timestamp;
        source_reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            0,
            nullptr,
            &streamFlags,
            &timestamp,
            &sample);

        // Sample이 EOS라면 종료한다.
        if (!sample || streamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
            break;
        
        DWORD buffer_length, buffer_max;
        IMFMediaBuffer* media_buffer;
        BYTE* real_buffer;
        sample->ConvertToContiguousBuffer(&media_buffer);
        media_buffer->Lock(&real_buffer, &buffer_max, &buffer_length);

        // 샘플 간격 설정
        if(time_term_ == 0 && bef_ts > 0)
            time_term_ = timestamp - bef_ts;
        bef_ts = timestamp;
        
        DWORD idx = 0;
        while(data_len_ < sample_count && idx < buffer_length)
        {
            double audio_data = combine_audio_data(real_buffer, &idx);
            normalize(&audio_data);
            data_[data_len_++] = audio_data;
        }

        over += buffer_length - idx;
    }
}

std::vector<std::string> MusicReader::split(std::string input, char delimiter)
{
    std::vector<std::string> answer;
    std::stringstream ss(input);
    std::string temp;
 
    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }
 
    return answer;

}

void MusicReader::play_music()
{
    HRESULT hr = MFStartup(MF_VERSION);
    check(hr, L"MF Initiate Exception");

    IMFSourceReader* source_reader = nullptr;
    hr = MFCreateSourceReaderFromURL(path_, nullptr, &source_reader);
    check(hr, L"Create Source Reader Exception");
    source_reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, false);
    source_reader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, true);
    
    IMFMediaSink* media_sink;
    IMFStreamSink* stream_sink;
    IMFMediaTypeHandler* typeHandler;
    IMFMediaType* input_type = nullptr;
    DWORD dwCount = 0;
    MFCreateAudioRenderer(nullptr, &media_sink);
    media_sink->GetStreamSinkByIndex(0, &stream_sink);
    stream_sink->GetMediaTypeHandler(&typeHandler);
    typeHandler->GetMediaTypeCount(&dwCount);
    
    // Stream_Sink가 Input_Type 미디어 타입을 지원하는지 검사
    for (INT i = 0; i < static_cast<int>(dwCount); i++)
    {
        typeHandler->GetMediaTypeByIndex(i, &input_type);
        if (SUCCEEDED(typeHandler->IsMediaTypeSupported(input_type, NULL)))
            break;
    }
    source_reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, input_type);
    typeHandler->SetCurrentMediaType(input_type);
    
    IMFSinkWriter* writer;
    MFCreateSinkWriterFromMediaSink(media_sink, nullptr, &writer);
    writer->SetInputMediaType(0, input_type, nullptr);

    writer->BeginWriting();
    UINT32* shape = new UINT32[2];
    output(shape, nullptr, nullptr);
    
    // Source에서 샘플 읽기
    while (true)
    {
        IMFSample* sample;
        DWORD streamFlags;
        MFTIME timestamp;
        source_reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            0,
            nullptr,
            &streamFlags,
            &timestamp,
            &sample);

        // Sample이 EOS라면 종료한다.
        if (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
        {
            writer->NotifyEndOfSegment(0);
            break;
        }

        // Sample이 유효하지 않다면 스킵한다.
        if (!sample) 
            continue;
        
        // Stream에 Sample을 입력한다. (재생)
        writer->WriteSample(0, sample);
    }
    writer->Finalize();
}

result MusicReader::output(UINT32* length, LONGLONG* timestamp, LONGLONG* time_length)
{
    if(length != nullptr)
    {
        assert(this != NULL);
        
        length[0] = num_chunks_;
        length[1] = WINDOW_SIZE / 2;
    }
    
    if(timestamp != nullptr)
        *timestamp = time_term_;
    
    if(time_length != nullptr)
        *time_length = length_;
    
    return result_;
}
