#define TSF_IMPLEMENTATION
#include "tsf.h"
#include "SynthVoice.hpp"
#include <iostream>

SynthEngine::SynthEngine() {}

SynthEngine::~SynthEngine() {
    std::lock_guard<std::mutex> lock(soundFontMutex);
    if (soundFont) tsf_close(soundFont);
}

bool SynthEngine::loadSoundFont(const std::string& filename) {
    std::lock_guard<std::mutex> lock(soundFontMutex);
    
    tsf* newSf = tsf_load_filename(filename.c_str());
    if (!newSf) {
        std::cerr << "[Error] Failed to load SoundFont: " << filename << std::endl;
        return false;
    }

    tsf_set_output(newSf, TSF_STEREO_INTERLEAVED, static_cast<int>(SAMPLE_RATE), 0.0f);
    tsf_channel_set_presetindex(newSf, 0, 0);

    if (soundFont) tsf_close(soundFont);
    soundFont = newSf;
    std::cout << "[Synth] SoundFont loaded: " << filename << std::endl;
    return true;
}

void SynthEngine::noteOn(int note, int velocity) {
    std::lock_guard<std::mutex> lock(soundFontMutex);
    if (!soundFont) return;
    tsf_channel_note_on(soundFont, 0, note, velocity / 127.0f);
}

void SynthEngine::noteOff(int note) {
    std::lock_guard<std::mutex> lock(soundFontMutex);
    if (!soundFont) return;
    tsf_channel_note_off(soundFont, 0, note);
}

void SynthEngine::controlChange(int controller, int value) {
    std::lock_guard<std::mutex> lock(soundFontMutex);
    if (!soundFont) return;

    if (controller == 64) { // サステインペダル
        if (pedalEnabled.load()) {
            tsf_channel_set_sustain(soundFont, 0, (value >= 64) ? 1 : 0);
        }
    } else {
        tsf_channel_midi_control(soundFont, 0, controller, value);
    }
}

// ペダル機能の有効・無効切り替え
void SynthEngine::setPedalEnabled(bool enabled) {
    pedalEnabled.store(enabled);
    
    // 無効化された場合、現在残っているサステイン（ペダル踏み状態）を即座に解除
    if (!enabled) {
        std::lock_guard<std::mutex> lock(soundFontMutex);
        if (soundFont) {
            tsf_channel_set_sustain(soundFont, 0, 0);
        }
    }
}

// パニック機能（全音強制消音）
void SynthEngine::allNotesOff() {
    std::lock_guard<std::mutex> lock(soundFontMutex);
    if (!soundFont) return;

    // 1. サステインペダル状態を強制リセット
    tsf_channel_set_sustain(soundFont, 0, 0);

    // 2. CC 120 (All Sound Off: リリース時間も待たずに即座に完全消音)
    tsf_channel_midi_control(soundFont, 0, 120, 0);

    // 3. CC 123 (All Notes Off)
    tsf_channel_midi_control(soundFont, 0, 123, 0);

    // 念のため全ノートオフ
    for (int i = 0; i < 128; ++i) {
        tsf_channel_note_off(soundFont, 0, i);
    }
}

void SynthEngine::renderBuffer(float* buffer, int numSamples) {
    std::lock_guard<std::mutex> lock(soundFontMutex);
    if (!soundFont) {
        for (int i = 0; i < numSamples * 2; ++i) buffer[i] = 0.0f;
        return;
    }

    tsf_render_float(soundFont, buffer, numSamples, 0);

    float vol = volume.load();
    if (vol != 1.0f) {
        int total = numSamples * 2;
        for (int i = 0; i < total; ++i) {
            buffer[i] *= vol;
        }
    }
}