#pragma once

class ADSREnvelope
{
public:

    enum class Stage
    {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };

    void prepare(double sampleRate);

    void reset();

    void noteOn();

    void noteOff();

    float processSample();

    bool isActive() const;

    void setAttack(float seconds);
    void setDecay(float seconds);
    void setSustain(float level);
    void setRelease(float seconds);
    void setPeakGain(float level);
private:

    double m_sampleRate = 44100.0;

    Stage m_stage =
        Stage::Idle;

    float m_level = 0.0f;

    float m_attack = 0.01f;
    float m_decay = 0.1f;
    float m_sustain = 0.8f;
    float m_release = 0.2f;
    float m_peakGain = 1.0f;
};