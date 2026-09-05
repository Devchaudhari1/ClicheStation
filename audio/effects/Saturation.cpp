#include <cmath>

float saturate(float x, float drive)
{
    const float y =
        x * drive;

    return y /
        (1.0f + std::abs(y));
}