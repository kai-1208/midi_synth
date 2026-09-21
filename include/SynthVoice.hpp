// include/SynthVoice.hpp
#pragma once
#include <string>

struct tsf;

class SynthEngine {
public:
    static constexpr float SAMPLE_RATE = 44100.0f;

    SynthEngine();
    ~SynthEngine();

    bool loadSoundFont(const std::string& filename);
    void noteOn(int note, int velocity);
    void noteOff(int note);
    void renderBuffer(float* buffer, int numSamples);

private:
    tsf* soundFont = nullptr;
};