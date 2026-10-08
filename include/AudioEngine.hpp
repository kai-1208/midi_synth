#pragma once
#include <RtAudio.h>
#include <memory>
#include <vector>
#include <string>
#include "SynthVoice.hpp"

struct AvailableDevice {
    RtAudio::Api api;
    unsigned int deviceId;
    std::string displayName;
};

class AudioEngine {
public:
    AudioEngine(SynthEngine& synth);
    ~AudioEngine();

    void listOutputDevices();
    bool start(int deviceIndex = -1);
    void stop();

    // C# 連携・情報取得メソッド
    int getDeviceCount() const { 
        return static_cast<int>(availableDevices.size()); 
    }
    std::string getDeviceName(int index) const {
        if (index >= 0 && index < static_cast<int>(availableDevices.size())) {
            return availableDevices[index].displayName;
        }
        return "";
    }

private:
    static int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
                             double streamTime, RtAudioStreamStatus status, void* userData);

    std::unique_ptr<RtAudio> activeDac;
    SynthEngine& synthRef;
    std::vector<AvailableDevice> availableDevices;
};