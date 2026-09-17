// src/AudioEngine.cpp
#include "AudioEngine.hpp"
#include <iostream>

AudioEngine::AudioEngine(SynthEngine& synth) : synthRef(synth) {
    // WindowsではWASAPI APIを明示的に使用
    dac = std::make_unique<RtAudio>(RtAudio::WINDOWS_WASAPI);
}

AudioEngine::~AudioEngine() {
    stop();
}

int AudioEngine::audioCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames,
                               double /*streamTime*/, RtAudioStreamStatus /*status*/, void* userData) {
    float* buffer = static_cast<float*>(outputBuffer);
    auto* self = static_cast<AudioEngine*>(userData);

    for (unsigned int i = 0; i < nBufferFrames; ++i) {
        float sample = self->synthRef.renderSample();
        *buffer++ = sample; // Left
        *buffer++ = sample; // Right
    }
    return 0;
}

bool AudioEngine::start() {
    if (dac->getDeviceCount() < 1) {
        std::cerr << "No audio devices found!\n";
        return false;
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac->getDefaultOutputDevice();
    parameters.nChannels = 2;
    parameters.firstChannel = 0;

    unsigned int sampleRate = static_cast<unsigned int>(SynthEngine::SAMPLE_RATE);
    unsigned int bufferFrames = 128; // 低遅延（約2.9ms）

    try {
        dac->openStream(&parameters, nullptr, RTAUDIO_FLOAT32,
                        sampleRate, &bufferFrames, &AudioEngine::audioCallback, this);
        dac->startStream();
        std::cout << "Audio Stream started with buffer size: " << bufferFrames << " frames." << std::endl;
    } catch (RtAudioErrorType& e) {
        std::cerr << "RtAudio error occurred: " << e << std::endl;
        return false;
    }
    return true;
}

void AudioEngine::stop() {
    if (dac && dac->isStreamOpen()) {
        dac->stopStream();
        dac->closeStream();
    }
}