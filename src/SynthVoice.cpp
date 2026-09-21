// src/SynthVoice.cpp
#include "SynthVoice.hpp"
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
    // 出力設定 (インターリーブステレオ, 44.1kHz)
    tsf_set_output(soundFont, TSF_STEREO_INTERLEAVED, static_cast<int>(SAMPLE_RATE), 0.0f);
    std::cout << "[Synth] SoundFont loaded successfully: " << filename << std::endl;
    return true;
}

void SynthEngine::noteOn(int note, int velocity) {
    if (!soundFont) return;
    float vel = velocity / 127.0f;
    tsf_note_on(soundFont, 0, note, vel);
}

void SynthEngine::noteOff(int note) {
    if (!soundFont) return;
    tsf_note_off(soundFont, 0, note);
}

void SynthEngine::renderBuffer(float* buffer, int numSamples) {
    if (!soundFont) {
        for (int i = 0; i < numSamples * 2; ++i) {
            buffer[i] = 0.0f;
        }
        return;
    }
    // 指定サンプル数分を一括でステレオ描画 (gain: 1.0f)
    tsf_render_float(soundFont, buffer, numSamples, 0);
}