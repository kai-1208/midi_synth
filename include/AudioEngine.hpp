#pragma once
#include <RtAudio.h>
#include <memory>
#include <vector>
#include "SynthVoice.hpp"

class AudioEngine {
public:
    AudioEngine(SynthEngine& synth);
    ~AudioEngine();

    void listOutputDevices();
    bool start(int deviceIndex = -1); // -1 の場合は既定のデバイスを使用
    void stop();

    int getDeviceCount() const {
        return validOutputDeviceIds.size();
    }
    std::string getDeviceName(int index) const {
        if (index >= 0 && index < (int)validOutputDeviceIds.size()) {
            return dac->getDeviceInfo(validOutputDeviceIds[index]).name;
        }
        return "";
    }

private:
    static int audioCallback(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames,
                             double streamTime, RtAudioStreamStatus status, void* userData);
    std::unique_ptr<RtAudio> dac;
    SynthEngine& synthRef;
    std::vector<unsigned int> validOutputDeviceIds; // 有効な出力デバイスIDのリスト
};
