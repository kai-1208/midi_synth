#include "AudioEngine.hpp"
#include <iostream>

AudioEngine::AudioEngine(SynthEngine& synth) : synthRef(synth) {
    dac = std::make_unique<RtAudio>(RtAudio::WINDOWS_WASAPI);
}

AudioEngine::~AudioEngine() {
    stop();
}

void AudioEngine::listOutputDevices() {
    validOutputDeviceIds.clear();
    std::vector<unsigned int> deviceIds = dac->getDeviceIds();

    std::cout << "\n[Audio Output Devices]" << std::endl;
    unsigned int defaultOut = dac->getDefaultOutputDevice();

    int index = 0;
    for (unsigned int id : deviceIds) {
        RtAudio::DeviceInfo info = dac->getDeviceInfo(id);
        // 出力チャンネルを持っているデバイスのみを表示
        if (info.outputChannels > 0) {
            validOutputDeviceIds.push_back(id);
            std::cout << "  [" << index << "] " << info.name;
            if (id == defaultOut) {
                std::cout << " (Default)";
            }
            std::cout << std::endl;
            index++;
        }
    }
}

int AudioEngine::audioCallback(void* outputBuffer, void* /*inputBuffer*/, unsigned int nBufferFrames,
                               double /*streamTime*/, RtAudioStreamStatus /*status*/, void* userData) {
    float* buffer = static_cast<float*>(outputBuffer);
    auto* self = static_cast<AudioEngine*>(userData);

    self->synthRef.renderBuffer(buffer, static_cast<int>(nBufferFrames));
    return 0;
}

bool AudioEngine::start(int deviceIndex) {
    if (validOutputDeviceIds.empty()) {
        // まだリスト化されていない場合は内部で取得
        listOutputDevices();
    }

    if (validOutputDeviceIds.empty()) {
        std::cerr << "[Audio Error] No audio output devices found!\n";
        return false;
    }

    unsigned int targetDeviceId = dac->getDefaultOutputDevice();
    if (deviceIndex >= 0 && static_cast<size_t>(deviceIndex) < validOutputDeviceIds.size()) {
        targetDeviceId = validOutputDeviceIds[deviceIndex];
    }

    RtAudio::DeviceInfo info = dac->getDeviceInfo(targetDeviceId);
    std::cout << "[Audio] Using output device: " << info.name << std::endl;

    RtAudio::StreamParameters parameters;
    parameters.deviceId = targetDeviceId;
    parameters.nChannels = 2; // ステレオ
    parameters.firstChannel = 0;

    unsigned int sampleRate = static_cast<unsigned int>(SynthEngine::SAMPLE_RATE);
    unsigned int bufferFrames = 128; // 低遅延（約2.9ms）

    try {
        dac->openStream(&parameters, nullptr, RTAUDIO_FLOAT32,
                        sampleRate, &bufferFrames, &AudioEngine::audioCallback, this);
        dac->startStream();
        std::cout << "[Audio] Audio stream started (Buffer: " << bufferFrames << " frames).\n";
    } catch (RtAudioErrorType& e) {
        std::cerr << "[Audio Error] Failed to open audio stream: " << e << std::endl;
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