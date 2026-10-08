#include "AudioEngine.hpp"
#include <iostream>

AudioEngine::AudioEngine(SynthEngine& synth) : synthRef(synth) {
    listOutputDevices();
}

AudioEngine::~AudioEngine() {
    stop();
}

void AudioEngine::listOutputDevices() {
    availableDevices.clear();
    std::cout << "\n[Audio Output Devices (ASIO & Windows Standard)]" << std::endl;

    int displayIndex = 0;

    // 1. ASIO デバイスのスキャン
    try {
        RtAudio asioDac(RtAudio::WINDOWS_ASIO);
        std::vector<unsigned int> asioIds = asioDac.getDeviceIds();
        for (unsigned int id : asioIds) {
            RtAudio::DeviceInfo info = asioDac.getDeviceInfo(id);
            if (info.outputChannels > 0) {
                std::string name = "[ASIO] " + info.name;
                availableDevices.push_back({RtAudio::WINDOWS_ASIO, id, name});
                std::cout << "  [" << displayIndex++ << "] " << name << std::endl;
            }
        }
    } catch (...) {
        // ASIOが環境にない場合はスキップ
    }

    // 2. WASAPI（Windows標準）デバイスのスキャン
    try {
        RtAudio wasapiDac(RtAudio::WINDOWS_WASAPI);
        std::vector<unsigned int> wasapiIds = wasapiDac.getDeviceIds();
        unsigned int defaultOut = wasapiDac.getDefaultOutputDevice();

        for (unsigned int id : wasapiIds) {
            RtAudio::DeviceInfo info = wasapiDac.getDeviceInfo(id);
            if (info.outputChannels > 0) {
                std::string name = "[WASAPI] " + info.name;
                if (id == defaultOut) {
                    name += " (Default)";
                }
                availableDevices.push_back({RtAudio::WINDOWS_WASAPI, id, name});
                std::cout << "  [" << displayIndex++ << "] " << name << std::endl;
            }
        }
    } catch (...) {
        // WASAPIエラー時はスキップ
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
    if (availableDevices.empty()) {
        listOutputDevices();
    }

    if (availableDevices.empty()) {
        std::cerr << "[Audio Error] No audio devices found!\n";
        return false;
    }

    // デバイスの決定（指定がなければ先頭または既定）
    int selectedIdx = 0;
    if (deviceIndex >= 0 && static_cast<size_t>(deviceIndex) < availableDevices.size()) {
        selectedIdx = deviceIndex;
    }

    const auto& chosen = availableDevices[selectedIdx];
    std::cout << "[Audio] Opening: " << chosen.displayName << std::endl;

    // 選択されたAPIでRtAudioを生成
    try {
        activeDac = std::make_unique<RtAudio>(chosen.api);
    } catch (RtAudioErrorType& e) {
        std::cerr << "[Audio Error] Failed to create driver: " << e << std::endl;
        return false;
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = chosen.deviceId;
    parameters.nChannels = 2; // ステレオ
    parameters.firstChannel = 0;

    unsigned int sampleRate = static_cast<unsigned int>(SynthEngine::SAMPLE_RATE);
    // ASIO/WASAPI共通で安定かつ低遅延な256サンプル（約5.3ms）
    unsigned int bufferFrames = 256; 

    try {
        activeDac->openStream(&parameters, nullptr, RTAUDIO_FLOAT32,
                             sampleRate, &bufferFrames, &AudioEngine::audioCallback, this);
        activeDac->startStream();
        std::cout << "[Audio] Stream started successfully (Buffer: " << bufferFrames << " frames).\n";
    } catch (RtAudioErrorType& e) {
        std::cerr << "[Audio Error] Stream failed: " << e << std::endl;
        return false;
    }

    return true;
}

void AudioEngine::stop() {
    if (activeDac && activeDac->isStreamOpen()) {
        activeDac->stopStream();
        activeDac->closeStream();
    }
    activeDac.reset();
}