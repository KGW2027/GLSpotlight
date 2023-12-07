#include "stage/StageBuilder.h"


/*
 * TODO 1 - dB 데이터가 제대로 적용되지 않은 것으로 보임.
 *  => WAV parsing에서는 Python에서의 결과와 같은 것을 확인함.
 *  => STFT 과정에서 Python에서는 min~max가 0~359, Cpp에서는 0~730으로 큰 차이가 나는 것을 확인함. (23/11/24)
 *  => Python에서 STFT결과나 amp_to_dB의 결과를 가져온 경우 정상적으로 출력되는 것을 확인함.         (23/11/26)
 *  => Python에서는 RFFT를 사용 -> 음수 데이터를 버림. So, STFT 입력 데이터의 음수 부분을 컷해서 시도.
 *      => STFT의 문제로 추정 (Python Wav Read 데이터를 STFT한 결과와 내 Wav Read 데이터를 STFT한 결과가 다름)
 *      => Python Data는 0~229.38의 float32형, Cpp Data는 0~65535의 unsigned short형.
 *  
 */

int main(int argc, char** argv) {
    StageBuilder builder = StageBuilder(argc, argv);
    builder.start();
    
    return 0;
}
