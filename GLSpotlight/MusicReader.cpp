#include "MusicReader.h"

#include <vector>
#include <mfreadwrite.h>


MusicReader::MusicReader(const wchar_t* path) : is_valid(false)
{
    PATH = path;
    if(!is_file_exists(PATH)) return;
    
    is_valid = true;

    // Read Audio Data
    read_file();
    printf("LEN : %d\n", data_len);

    // STFT DATA
    stft(2048, 512);
}

#pragma region DEPRECATED-1
// double* MusicReader::byte_array_to_double(BYTE* arr, DWORD len)
// {
//     double* double_arr = new double[len];
//     for(DWORD i = 0 ; i < len ; i++) double_arr[i] = static_cast<double>(arr[i]);
//     return double_arr;
// }
//
// bool MusicReader::ends_with(const wchar_t* str, const wchar_t* suffix)
// {
//     size_t strLen = std::wcslen(str);
//     size_t suffixLen = std::wcslen(suffix);
//
//     // Check if the string is shorter than the suffix
//     if (strLen < suffixLen) {
//         return false;
//     }
//
//     // Compare the end of the string with the suffix
//     return std::wcscmp(str + (strLen - suffixLen), suffix) == 0;
// }
//
// double* MusicReader::fft_audio_data(IMFSample* sample, int* len)
// {
//     // Sample Buffer를 연속하게 가져온다.
//     IMFMediaBuffer* media_buffer;
//     sample->ConvertToContiguousBuffer(&media_buffer);
//
//     // Sample Buffer 내부 값을 접근 가능하게 만든다.
//     BYTE* sample_start_address;
//     DWORD max_length, current_length;
//     media_buffer->Lock(&sample_start_address, &max_length, &current_length);
//     const int loops = static_cast<int>(current_length);
//
//     // FFTW에 입력 가능하도록 BYTE*를 double*로 변환
//     std::vector<double> in(loops);
//     for(int idx = 0 ; idx < loops ; idx++) in[idx] = (static_cast<double>(sample_start_address[idx]) - 128.0) / 128.0;
//
//     // FFT 수행
//     fftw_complex* out = fftw_alloc_complex(loops);
//     fftw_plan plan = fftw_plan_dft_r2c_1d(loops, in.data(), out, FFTW_ESTIMATE);
//     fftw_execute(plan);
//
//     const int new_length = loops / 2 + 1;
//     
//     // Magnitude Spectrum 계산
//     std::vector<double> magnitude(new_length);
//     for(int idx = 0 ; idx < new_length ; idx++) magnitude[idx] = sqrt(out[idx][0] * out[idx][0] + out[idx][1] * out[idx][1]);
//
//     // Magnitude to dB 계산
//     double* db = new double[new_length]{0};
//     double epsilon = 1e-9;
//     for(int idx = 0 ; idx < new_length ; idx++)
//     {
//         db[idx] = 20.0 * std::log10(max(magnitude[idx], epsilon));
//         // printf("[Idx %d] Value : %lf\n", idx, db[idx]);
//     }
//
//     // Clean
//     fftw_destroy_plan(plan);
//     fftw_free(out);
//     *len = new_length;
//     
//     return db;
// }
//
// int MusicReader::compress_idx(int idx, int section_gap = 7)
// {
//     int section_id = max(static_cast<int>(log2(idx)-6.0), 0);
//     if(section_id == 0) return idx;
//     
//     int section_term = static_cast<int>(pow(2, section_gap));
//     int range = max(1, pow(2, max(0, section_id-1)));
//     int apply_range = (idx - pow(2, floor(log2(idx)))) / range;
//         
//     return section_term * section_id + apply_range;
// }
//
// void MusicReader::compress_dB(double* data, int len, double** out, int* new_len)
// {
//     int comp_range = 7;
//     int comp_len = static_cast<int>(pow(2, comp_range) * (ceil(log2(len))-6.0));
//     double* comp_data = new double[comp_len]{0};
//
//     // 데이터 압축 합계
//     for(int idx = 0 ; idx < len ; idx++)
//         comp_data[compress_idx(idx, comp_range)] += data[idx];
//
//     // 데이터 평균 내기
//     for(int idx = 0 ; idx < comp_len ; idx++)
//         comp_data[idx] /= pow(2, max((idx / 128)-1, 0));
//
//     // 반환
//     *out = comp_data;
//     *new_len = comp_len;
// }
#pragma endregion

float MusicReader::combine_float(BYTE* array, int idx)
{
    uint32_t combine = 0;
    for(int shift = 32-8 ; shift >= 0 ; shift -= 8)
        combine |= static_cast<uint32_t>(array[idx++]) << shift;
    float result;
    std::memcpy(&result, &combine, sizeof(float));
    return result;
}

double MusicReader::combine_double(BYTE* array, int idx)
{
    uint64_t combine = 0;
    for(int shift = 64-8 ; shift >= 0 ; shift -= 8)
        combine |= static_cast<uint64_t>(array[idx++]) << shift;
    double result;
    std::memcpy(&result, &combine, sizeof(double));
    return result;
}

void MusicReader::read_file()
{
    HRESULT hr = S_OK;
    
    hr = MFStartup(MF_VERSION);
    check(hr, L"MF Initiate Exception");

    IMFSourceReader* source_reader = nullptr;
    hr = MFCreateSourceReaderFromURL(PATH, nullptr, &source_reader);
    check(hr, L"Create Source Reader Exception");

    IMFMediaType* media_type = nullptr;
    hr = source_reader->GetNativeMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &media_type);
    check(hr, L"Get Native Media Type Exception");

    GUID major_type;
    UINT32 num_channels;
    DWORD sample_size;
    hr = media_type->GetGUID(MF_MT_MAJOR_TYPE, &major_type);
    if (SUCCEEDED(hr) && major_type == MFMediaType_Audio)
    {
        sample_rate = MFGetAttributeUINT32(media_type, MF_MT_AUDIO_SAMPLES_PER_SECOND, 0);
        num_channels = MFGetAttributeUINT32(media_type, MF_MT_AUDIO_NUM_CHANNELS, 0);
        std::cout << "Sampling Rate : " << sample_rate << " Hz\nNum Channels : " << num_channels  << std::endl;

        PROPVARIANT duration;
        source_reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &duration);
        sample_size = static_cast<DWORD>(duration.hVal.QuadPart * sample_rate / 10'000'000);
    }

    float* samples = new float[sample_size];
    data_len = 0;
    int frame_header_count = 0;
    while(true)
    {
        IMFSample* sample = nullptr;
        DWORD stream_idx, flags;
        LONGLONG timestamp;
    
        hr = source_reader->ReadSample(
            MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            0,
            &stream_idx,
            &flags,
            &timestamp,
            &sample
            );
    
        if (FAILED(hr) || flags & MF_SOURCE_READERF_ENDOFSTREAM) break;

        DWORD buffer_length, buffer_max;
        IMFMediaBuffer* media_buffer;
        BYTE* real_buffer;
        sample->ConvertToContiguousBuffer(&media_buffer);
        media_buffer->Lock(&real_buffer, &buffer_max, &buffer_length);
        for(int idx = 0 ; idx < buffer_length ; idx++)
        {
            // printf("[%d] %lf\n", data_len, combine_double(real_buffer, idx));
            if(idx+1 < buffer_length && check_header(real_buffer[idx], real_buffer[idx+1])) frame_header_count++;
            samples[data_len++] = static_cast<float>(real_buffer[idx]) / 255;
        }
        
        media_buffer->Release();
        sample->Release();
    }
    printf("FRAME HEADER COUNT : %d\n", frame_header_count);
    DATA = samples;
}

void MusicReader::stft(int window_size, int hop_size)
{
    // Initialize FFTW plan and allocate memory
    fftw_plan plan;
    fftw_complex* in, * out;
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);
    plan = fftw_plan_dft_1d(window_size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    RESULT = new float*[(data_len - window_size) / hop_size + 1];
    int key = 0;
    // Iterate through the signal with the specified hop size
    for (int i = 0; i < data_len - window_size; i += hop_size) {
        // Fill the input array with the current window
        for (int j = 0; j < window_size; ++j) {
            in[j][0] = DATA[i + j];
            in[j][1] = 0.0;
        }

        // Execute FFT
        fftw_execute(plan);

        float* db = new float[window_size];
        // Process the results (for demonstration purposes, you can customize this part)
        // Magnitude spectrum
        for (int j = 0; j < window_size; ++j)
        {
            db[j] = 20.0f * log10(max(1e-10, out[j][0]));
            // printf("[Time %d] %.2f Hz | %.4f dB\n", i, static_cast<float>(j) * sample_rate / window_size, db[j]);
        }
        RESULT[key++] = db;
        break;
    }

    // Clean up FFTW resources
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
}
