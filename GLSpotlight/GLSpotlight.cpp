#include "stage/StageBuilder.h"

int main(int argc, char** argv) {
    StageBuilder builder = StageBuilder(argc, argv);
    builder.start();
    
    return 0;
}
