// src/AudioEngine.cpp
#include "AudioEngine.hpp"
#include <iostream>

AudioEngine::AudioEngine(SynthEngine& synth) : synthRef(synth) {
    dac = std::make_unique<RtAudio>(RtAudio::WINDOWS_WASAPI);
}

AudioEngine::~AudioEngine() {
    stop();
}

int AudioEngine::audioCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames,
                               double /*streamTime*/, RtAudioStreamStatus /*status*/, void* userData) {
    float* buffer = static_cast<float*>(outputBuffer);
    auto* self = static_cast<AudioEngine*>(userData);

    // バッファサイズ分（128サンプル）を一括展開
    self->synthRef.renderBuffer(buffer, static_cast<int>(nBufferFrames));
    return 0;
}

bool AudioEngine::start() {
    if (dac->getDeviceCount() < 1) {
        std::cerr << "[Audio] No audio devices found!\n";
        return false;
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac->getDefaultOutputDevice();
    parameters.nChannels = 2; // ステレオ
    parameters.firstChannel = 0;

    unsigned int sampleRate = static_cast<unsigned int>(SynthEngine::SAMPLE_RATE);
    unsigned int bufferFrames = 128; // 超低遅延（約2.9ms）

    try {
        dac->openStream(&parameters, nullptr, RTAUDIO_FLOAT32,
                        sampleRate, &bufferFrames, &AudioEngine::audioCallback, this);
        dac->startStream();
        std::cout << "[Audio] Audio stream started with buffer size: " << bufferFrames << " frames.\n";
    } catch (RtAudioErrorType& e) {
        std::cerr << "[Audio] RtAudio error: " << e << std::endl;
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