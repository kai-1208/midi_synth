// include/SynthVoice.hpp
#pragma once
#include <string>
#include <atomic>
#include <mutex>

struct tsf;

class SynthEngine {
public:
    static constexpr float SAMPLE_RATE = 48000.0f;

    SynthEngine();
    ~SynthEngine();

    bool loadSoundFont(const std::string& filename);
    void noteOn(int note, int velocity);
    void noteOff(int note);
    void controlChange(int controller, int value);
    void renderBuffer(float* buffer, int numSamples);

    void setVolume(float vol) { volume.store(vol); }
    void setPedalEnabled(bool enabled);
    void allNotesOff();

private:
    tsf* soundFont = nullptr;
    std::mutex soundFontMutex;             // ロードと描画の競合を防ぐ
    std::atomic<float> volume{1.0f};       // 音量 (0.0 ~ 1.0)
    std::atomic<bool> pedalEnabled{true};  // ペダル有効フラグ
};