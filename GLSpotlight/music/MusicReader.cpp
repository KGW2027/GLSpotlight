#include "MusicReader.h"

#include <mfreadwrite.h>
#include <thread>

// Audio Play : https://stackoverflow.com/questions/65412545/how-to-increase-mp3-decoding-quality-media-foundation

MusicReader::MusicReader(const wchar_t* path) : is_valid_(false)
{
    path_ = path;
    if(!is_file_exists(path_)) return;
    
    is_valid_ = true;

    // Read Audio Data
    read_file();

    // STFT DATA
    stft();
}

void MusicReader::play_music(void(* callback)(float** array, int len))
{
    play_music_internal(callback);
}

double MusicReader::combine_float64(BYTE* array, DWORD* idx)
{
    uint32_t combine = 0;
    for(int shift = 64-8 ; shift >= 0 ; shift -= 8)
        combine |= static_cast<uint32_t>(array[(*idx)++]) << shift;
    float result;
    std::memcpy(&result, &combine, sizeof(float));
    return result;
}

float MusicReader::combine_float32(BYTE* array, DWORD* idx)
{
    uint32_t combine = 0;
    for(int shift = 32-8 ; shift >= 0 ; shift -= 8)
        combine |= static_cast<uint32_t>(array[(*idx)++]) << shift;
    float result;
    std::memcpy(&result, &combine, sizeof(float));
    return result;
}

DWORD MusicReader::combine_int32(BYTE* array, DWORD* idx)
{
    uint32_t combine = 0;
    for(int shift = 32-8 ; shift >= 0 ; shift -= 8)
        combine |= static_cast<uint32_t>(array[(*idx)++]) << shift;
    DWORD result;
    std::memcpy(&result, &combine, sizeof(DWORD));
    return result;
}

WORD MusicReader::combine_int16(BYTE* array, DWORD* idx)
{
    uint32_t combine = 0;
    for(int shift = 16-8 ; shift >= 0 ; shift -= 8)
        combine |= static_cast<uint16_t>(array[(*idx)++]) << shift;
    WORD result;
    std::memcpy(&result, &combine, sizeof(WORD));
    return result;
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
        PROPVARIANT duration;
        source_reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &duration);
        // LONGLONG quadpart = duration.hVal.QuadPart;
        sample_count = static_cast<DWORD>(duration.hVal.QuadPart * sample_rate_ / 10'000'000);
    }

    // printf("SAMPLE SIZE : %lld\n", sample_count);

    data_ = new float[sample_count]{0.f};
    data_len_ = 0;
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

        DWORD idx = 0;
        while(data_len_ < sample_count && idx < buffer_length)
            data_[data_len_++] = static_cast<float>(combine_int16(real_buffer, &idx)) / static_cast<float>(SHRT_MAX) - 1;
    }
}

void MusicReader::play_music_internal(void (*callback)(float** array, int len))
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
    int now_idx = 85;
    int* shape = new int[2];
    LONGLONG bef_ts = 0;
    output(&shape);
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
        
        if(now_idx < shape[0])
            callback(&result_[now_idx++], shape[1]);
        std::this_thread::sleep_for(std::chrono::milliseconds((timestamp-bef_ts) / 10'000)*0.9);
        
        // Stream에 Sample을 입력한다. (재생)
        writer->WriteSample(0, sample);
        bef_ts = timestamp;
    }
    writer->Finalize();
}

void MusicReader::stft()
{
    // Initialize FFTW plan and allocate memory
    fftw_plan plan;
    fftw_complex *in, *out;
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * WINDOW_SIZE);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * WINDOW_SIZE);
    plan = fftw_plan_dft_1d(WINDOW_SIZE, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    result_ = new float*[(data_len_ - WINDOW_SIZE) / HOP_SIZE + 5];
    float hamming[WINDOW_SIZE];
    for(int i = 0 ; i < WINDOW_SIZE ; i++)
        hamming[i] = 0.54f - 0.46f * cos(2 * 3.141592 * (i / ((WINDOW_SIZE - 1) * 1.0)));
    int num_chunks = 0, chunk_pos = 0;
    bool bIsBreak = false;

    while(chunk_pos < data_len_ && !bIsBreak)
    {
        // Windoing
        for(int i = 0 ; i < WINDOW_SIZE ; i++)
        {
            int read = chunk_pos + i;
            in[i][0] = 0.0;
            in[i][1] = 0.0;
            
            if(read < data_len_)
            {
                in[i][0] = data_[read] * hamming[i];
                continue;
            }
            
            bIsBreak = true;
        }

        // FFT
        fftw_execute(plan);

        // Apply
        float* chunks = new float[WINDOW_SIZE / 2];
        for(int i = 0 ; i < WINDOW_SIZE/2 ; i++)
            chunks[i] = 5 * (log10(max(AMIN, out[i][0] * out[i][0])) - log10(AMIN));
        result_[num_chunks] = chunks;

        // Next
        chunk_pos += HOP_SIZE;
        ++num_chunks;
    }

    printf("NUM CHUNKS : %d\n", num_chunks);

    // Clean up FFTW resources
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
}

result MusicReader::output(int** length)
{
    (*length)[0] = (static_cast<long long>(data_len_) - WINDOW_SIZE) / HOP_SIZE + 5;
    (*length)[1] = WINDOW_SIZE / 2;
    return result_;
}
