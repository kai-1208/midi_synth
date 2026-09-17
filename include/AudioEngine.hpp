// include/AudioEngine.hpp
#pragma once
#include <RtAudio.h>
#include <memory>
#include "SynthVoice.hpp"

class AudioEngine {
public:
    AudioEngine(SynthEngine& synth);
    ~AudioEngine();
    bool start();
    void stop();

private:
    static int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
                             double streamTime, RtAudioStreamStatus status, void* userData);
    std::unique_ptr<RtAudio> dac;
    SynthEngine& synthRef;
};