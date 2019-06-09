#include "chrono.h"

FChrono UChronoAPI::GetNowChrono() {
    FChrono c;
    
    c.tp = std::chrono::system_clock::now();
    
    return c;
}

float UChronoAPI::GetDiffMicrosecons(const FChrono& c1, const FChrono& c2) {
    return static_cast<float>(std::chrono::duration_cast<std::chrono::microseconds>(c2.tp - c1.tp).count());
}
