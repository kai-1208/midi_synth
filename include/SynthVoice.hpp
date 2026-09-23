// include/SynthVoice.hpp
#pragma once
#include <string>

struct tsf;

class SynthEngine {
public:
    static constexpr float SAMPLE_RATE = 48000.0f; // 48kHz

    SynthEngine();
    ~SynthEngine();

    bool loadSoundFont(const std::string& filename);
    void noteOn(int note, int velocity);
    void noteOff(int note);
    void controlChange(int controller, int value); // ペダルなどの制御信号
    void renderBuffer(float* buffer, int numSamples);

private:
    tsf* soundFont = nullptr;
};