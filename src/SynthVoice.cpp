// src/SynthVoice.cpp
#include "SynthVoice.hpp"

SynthEngine::SynthEngine() {
    for (auto& v : voices) {
        v.active = false;
    }
}

float SynthEngine::midiNoteToFreq(int note) {
    // A4 (note 69) = 440 Hz
    return 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
}

void SynthEngine::noteOn(int note, int velocity) {
    if (velocity == 0) {
        noteOff(note);
        return;
    }

    // 空いているボイス、または最も音量が小さいボイスを割り当て
    int targetIdx = 0;
    float minAmp = 100.0f;
    for (size_t i = 0; i < MAX_VOICES; ++i) {
        if (!voices[i].active) {
            targetIdx = i;
            break;
        }
        if (voices[i].amplitude < minAmp) {
            minAmp = voices[i].amplitude;
            targetIdx = i;
        }
    }

    auto& v = voices[targetIdx];
    v.active = true;
    v.noteNumber = note;
    v.velocity = velocity / 127.0f;
    v.amplitude = v.velocity;
    v.phase = 0.0f;
    float freq = midiNoteToFreq(note);
    v.phaseIncrement = (2.0f * 3.14159265358979323846f * freq) / SAMPLE_RATE;
}

void SynthEngine::noteOff(int note) {
    for (auto& v : voices) {
        if (v.active && v.noteNumber == note) {
            v.decayRate = 0.999f; // キーを離したら素早く減衰
        }
    }
}

float SynthEngine::renderSample() {
    float mixed = 0.0f;
    for (auto& v : voices) {
        if (!v.active) continue;

        // 簡易倍音（基音 + 第2倍音）によるピアノ風の厚み付け
        float sample = std::sin(v.phase) * 0.7f + std::sin(v.phase * 2.0f) * 0.3f;
        mixed += sample * v.amplitude;

        // 位相とエンベロープ更新
        v.phase += v.phaseIncrement;
        if (v.phase >= 2.0f * 3.14159265358979323846f) {
            v.phase -= 2.0f * 3.14159265358979323846f;
        }

        v.amplitude *= v.decayRate;
        if (v.amplitude < 0.0001f) {
            v.active = false;
        }
    }
    return mixed * 0.2f; // 全体マスター音量
}