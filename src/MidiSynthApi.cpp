#include "MidiSynthApi.h"
#include "SynthVoice.hpp"
#include "MidiHandler.hpp"
#include "AudioEngine.hpp"
#include <memory>
#include <cstring>
#include <vector>

static std::unique_ptr<SynthEngine> g_synth;
static std::unique_ptr<MidiHandler> g_midi;
static std::unique_ptr<AudioEngine> g_audio;

extern "C" {

SYNTH_API bool Synth_Init() {
    g_synth = std::make_unique<SynthEngine>();
    
    g_midi = std::make_unique<MidiHandler>(
        [](bool isNoteOn, int note, int vel) {
            if (g_synth) {
                if (isNoteOn) g_synth->noteOn(note, vel);
                else g_synth->noteOff(note);
            }
        },
        [](int controller, int value) {
            if (g_synth) g_synth->controlChange(controller, value);
        }
    );

    g_audio = std::make_unique<AudioEngine>(*g_synth);
    return true;
}

SYNTH_API void Synth_Shutdown() {
    if (g_audio) g_audio->stop();
    g_audio.reset();
    g_midi.reset();
    g_synth.reset();
}

SYNTH_API bool Synth_LoadSoundFont(const char* path) {
    if (!g_synth || !path) return false;
    return g_synth->loadSoundFont(path);
}

SYNTH_API void Synth_SetMasterVolume(float volume) {
    if (g_synth) g_synth->setVolume(volume);
}

SYNTH_API void Synth_SetPedalEnabled(bool enabled) {
    if (g_synth) g_synth->setPedalEnabled(enabled);
}

SYNTH_API void Synth_AllNotesOff() {
    if (g_synth) g_synth->allNotesOff();
}

SYNTH_API int Synth_GetMidiPortCount() {
    return g_midi ? g_midi->getPortCount() : 0;
}

SYNTH_API bool Synth_GetMidiPortName(int index, char* outBuffer, int bufferSize) {
    if (!g_midi || !outBuffer) return false;
    std::string name = g_midi->getPortName(index);
    if (name.empty()) return false;
    strncpy(outBuffer, name.c_str(), bufferSize - 1);
    outBuffer[bufferSize - 1] = '\0';
    return true;
}

SYNTH_API bool Synth_OpenMidiPort(int index) {
    return g_midi ? g_midi->openPort(index) : false;
}

SYNTH_API int Synth_GetAudioDeviceCount() {
    return g_audio ? g_audio->getDeviceCount() : 0;
}

SYNTH_API bool Synth_GetAudioDeviceName(int index, char* outBuffer, int bufferSize) {
    if (!g_audio || !outBuffer) return false;
    std::string name = g_audio->getDeviceName(index);
    if (name.empty()) return false;
    strncpy(outBuffer, name.c_str(), bufferSize - 1);
    outBuffer[bufferSize - 1] = '\0';
    return true;
}

SYNTH_API bool Synth_StartAudio(int deviceIndex) {
    return g_audio ? g_audio->start(deviceIndex) : false;
}

SYNTH_API void Synth_StopAudio() {
    if (g_audio) g_audio->stop();
}

}