#include<atomic>
#include<algorithm>
#include "AudioEffect.h"

class Clipper : public AudioEffect
{
public:
    void prepare(
        double,
        std::size_t,
        std::size_t) override
    {
    }

    void reset() override
    {
    }

    void setThreshold(float value)
    {
        threshold.store(value);
    }

    void process(
        float* left,
        float* right,
        std::size_t count) override
    {
        const float t =
            threshold.load();

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            left[i] =
                std::clamp(
                    left[i],
                    -t,
                    t);

            right[i] =
                std::clamp(
                    right[i],
                    -t,
                    t);
        }
    }

private:
    std::atomic<float> threshold { 0.8f };
};