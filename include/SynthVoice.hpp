// include/SynthVoice.hpp
#pragma once
#include <array>
#include <cmath>
#include <cstdint>

struct Voice {
    bool active = false;
    int noteNumber = 0;
    float velocity = 0.0f;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
    float amplitude = 0.0f;
    float decayRate = 0.99992f; // 減衰速度 (ピアノ風)
};

class SynthEngine {
public:
    static constexpr size_t MAX_VOICES = 16;
    static constexpr float SAMPLE_RATE = 44100.0f;

    SynthEngine();
    void noteOn(int note, int velocity);
    void noteOff(int note);
    float renderSample();

private:
    std::array<Voice, MAX_VOICES> voices;
    float midiNoteToFreq(int note);
};