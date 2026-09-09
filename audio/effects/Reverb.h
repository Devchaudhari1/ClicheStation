
#include "AudioEffect.h"
#include <QString>

class Reverb : public AudioEffect
{
public:
    void prepare(double sampleRate,
                 std::size_t maximumBlockSize,
                 std::size_t channels) override;

    void process(float *left,
                 float *right,
                 std::size_t numSamples) override;

    void reset() override;
    
    void setRoomSize(float roomSize);
    void setDamping(float damping);
    void setWet(float wet);
    void setDecay(float decay);
    AudioEffectType effectType() const override;
    
    QString effectName() const override;

private:
    // Preset parameters for the first implementation.
    float m_roomSize = 0.7f;
    float m_damping = 0.4f;
    float m_wet = 0.25f;
    float m_decay = 0.6f;

    // Internal state.
    std::vector<float> m_leftBuffer;
    std::vector<float> m_rightBuffer;

    std::size_t m_writePosition = 0;
    double m_sampleRate = 48000.0;
};