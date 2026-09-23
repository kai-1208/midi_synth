#define TSF_IMPLEMENTATION
#include "tsf.h"
#include "SynthVoice.hpp"
#include <iostream>

SynthEngine::SynthEngine() {}

SynthEngine::~SynthEngine() {
    if (soundFont) {
        tsf_close(soundFont);
    }
}

bool SynthEngine::loadSoundFont(const std::string& filename) {
    if (soundFont) {
        tsf_close(soundFont);
    }
    soundFont = tsf_load_filename(filename.c_str());
    if (!soundFont) {
        std::cerr << "[Error] Failed to load SoundFont: " << filename << std::endl;
        return false;
    }
    // 出力設定 (ステレオ, 48.0kHz)
    tsf_set_output(soundFont, TSF_STEREO_INTERLEAVED, static_cast<int>(SAMPLE_RATE), 0.0f);
    
    // チャンネル0にピアノプリセット（0番）を明示的に割り当て
    tsf_channel_set_presetindex(soundFont, 0, 0);

    std::cout << "[Synth] SoundFont loaded: " << filename << std::endl;
    return true;
}

void SynthEngine::noteOn(int note, int velocity) {
    if (!soundFont) return;
    float vel = velocity / 127.0f;
    // チャンネル0でノートオン
    tsf_channel_note_on(soundFont, 0, note, vel);
}

void SynthEngine::noteOff(int note) {
    if (!soundFont) return;
    // チャンネル0でノートオフ
    tsf_channel_note_off(soundFont, 0, note);
}

// サステインペダル（CC #64）の処理
void SynthEngine::controlChange(int controller, int value) {
    if (!soundFont) return;

    if (controller == 64) { // サステインペダル (Sustain / Damper Pedal)
        int sustainState = (value >= 64) ? 1 : 0;
        tsf_channel_set_sustain(soundFont, 0, sustainState);
        
        // 動作確認用ログ
        std::cout << "\r[Pedal] " << (sustainState ? "ON " : "OFF") << " (val: " << value << ")   " << std::flush;
    } else {
        // その他のコントロールチェンジ
        tsf_channel_midi_control(soundFont, 0, controller, value);
    }
}

void SynthEngine::renderBuffer(float* buffer, int numSamples) {
    if (!soundFont) {
        for (int i = 0; i < numSamples * 2; ++i) buffer[i] = 0.0f;
        return;
    }
    tsf_render_float(soundFont, buffer, numSamples, 0);
}