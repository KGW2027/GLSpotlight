#include "MusicReader.h"

#include <assert.h>
#include <mfreadwrite.h>
#include <thread>
#include <vector>
#include <fftw3.h>

#include "FourierLib.h"

// Audio Play : https://stackoverflow.com/questions/65412545/how-to-increase-mp3-decoding-quality-media-foundation

MusicReader::MusicReader(const wchar_t* path) : data_(nullptr), data_len_(0), is_valid_(false)
{
    path_ = path;
    if (!is_file_exists(path_)) return;

    is_valid_ = true;

    // Read Audio Data
    read_file();

    // Experiment - Pycharm Integration
    
    
    // STFT DATA
    STFT_Setting stft;
    stft.hop_len = HOP_SIZE;
    stft.win_len = WINDOW_SIZE;
    stft.in = data_;
    stft.in_len = data_len_;

    STFT_Out out = FourierLib::stft(stft);
    result_ = out.out;
    num_chunks_ = out.size[1];
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
    UINT64 num = 1;
    for(int bits = bit_depth_-1 ; bits > 0 ; bits--)
    {
        num <<= 1;
        num++;
    }
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

    // printf("SAMPLE SIZE : %lld\n", sample_count);

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
            // ParseLogger::write_str(std::to_string(data_len_-1) + " : " + std::to_string(audio_data) + "\n");
            //
            // if(data_len_ % (sample_count / 10) == 0) printf("Progress... %lu\n", data_len_);
        }

        over += buffer_length - idx;
    }
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
    output(&shape, nullptr, nullptr);
    
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

result MusicReader::output(UINT32** length, LONGLONG* timestamp, LONGLONG* time_length)
{
    if(length != nullptr)
    {
        (*length)[0] = num_chunks_;
        (*length)[1] = WINDOW_SIZE / 2;
    }
    
    if(timestamp != nullptr)
        *timestamp = time_term_;
    
    if(time_length != nullptr)
        *time_length = length_;
    
    return result_;
}
