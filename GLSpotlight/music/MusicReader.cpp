#include "MusicReader.h"

#include <assert.h>
#include <mfreadwrite.h>
#include <thread>
#include <vector>
#include <fftw3.h>
#include <fstream>

#include "FourierLib.h"
#include "WaveReader.h"

// Audio Play : https://stackoverflow.com/questions/65412545/how-to-increase-mp3-decoding-quality-media-foundation

MusicReader::MusicReader(const wchar_t* path) : is_valid_(false)
{
    is_ready_      = true;
    is_terminated_ = false;
    set_path(path);
}

void MusicReader::play_music()
{
    is_playing_    = true;
    is_terminated_ = false;
    
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

    auto now_time  = std::chrono::high_resolution_clock::now();
    writer->BeginWriting();
    
    // Source에서 샘플 읽기
    while (!is_terminated_)
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
            printf("NOTIFY END OF SEGMENT\n");
            writer->NotifyEndOfSegment(0);
            break;
        }

        // Sample이 유효하지 않다면 스킵한다.
        if (!sample) 
            continue;
        
        // Stream에 Sample을 입력한다. (재생)
        writer->WriteSample(0, sample);

        now_time += std::chrono::nanoseconds(timestamp / 100);
        std::this_thread::sleep_until(now_time);
    }
    
    writer->Release();
    writer->Finalize();

    MFShutdown();
    
    is_playing_ = false;
}

void MusicReader::terminate()
{
    is_ready_ = false;
    
    if(!is_terminated_ && wav_reader_ != nullptr) // terminate의 중복 호출 예방
    {
        for(size_t idx = 0 ; idx < processor_.time_len ; idx++)
            delete[] processor_.data[idx];
        delete[] processor_.data;
        
        processor_.debug.clear();
    }
    
    is_terminated_ = true;
}

void MusicReader::set_path(const wchar_t* new_path)
{
    path_ = new_path;
    if (!is_file_exists(path_)) return;
    
    wav_reader_ = new WaveReader(const_cast<wchar_t*>(path_));
    processor_  = wav_reader_->make_processor();
    
    // out의 memory free는 Wave Reader.clear에서 진행
    STFT_Out out = wav_reader_->get_stft_result();
    
    dB_In db_cvt;
    db_cvt.frame_count = out.size[1];
    db_cvt.window_size = out.size[0];
    db_cvt.top_dB = 80.;
    db_cvt.in = out.out;
    dB_Out db_out = FourierLib::amp_to_dB(db_cvt);

    processor_.win_len  = db_out.size[0];
    processor_.data     = db_out.out;
    processor_.time_len = max(processor_.time_len, db_out.size[1]);
    processor_.debug    = wav_reader_->print_info();
    wav_reader_->clear();

    is_valid_ = true;
    is_terminated_ = false;
    is_ready_ = true;
}

bool MusicReader::is_ready()
{
    return is_ready_ && !is_playing_;
}
