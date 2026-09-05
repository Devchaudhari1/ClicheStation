#include <vector>
#include<atomic>
#include "AudioEffect.h"
class Delay : public AudioEffect
{
private:
    std::vector<float> leftBuffer;
    std::vector<float> rightBuffer;

    std::size_t writePosition = 0;

    double sampleRate = 44100.0;

    std::atomic<float> delayMs { 300.0f };
    std::atomic<float> feedback { 0.4f };
    std::atomic<float> mix { 0.3f };
};