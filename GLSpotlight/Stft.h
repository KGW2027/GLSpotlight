#pragma once

#include <vector>


class stft
{
    static void hamming(int windowLength, float *buffer);
public:
    static void execute_stft(std::vector<float> *signal, int signalLength, int windowSize, int hopSize);
    
};
