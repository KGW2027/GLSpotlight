#include "MusicReader.h"

void callback(float** array, int window)
{
    printf("Playing now %d >> %f\n", window, (*array)[0]);
}

int main(int argc, char** argv) {
    const wchar_t* filename = L"../test.wav";
    
    MusicReader reader = MusicReader(filename);
    int* out_length = new int[2];
    float** out = reader.output(&out_length);

    printf("out shape : (%d, %d)\n", out_length[0], out_length[1]);
    reader.play_music(callback);
    
    return 0;
}