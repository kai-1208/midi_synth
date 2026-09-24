#include <iostream>
#include <string>
#include <vector>
#include "MidiSynthApi.h"

int main() {
    std::cout << "=== DLL API Functional Test CLI ===" << std::endl;

    // 1. エンジンの初期化
    if (!Synth_Init()) {
        std::cerr << "Failed to init Synth Engine.\n";
        return 1;
    }

    // 2. SoundFontの読み込み
    std::string sfPath = "piano.sf2";
    if (!Synth_LoadSoundFont(sfPath.c_str())) {
        std::cerr << "Failed to load '" << sfPath << "'.\n";
        Synth_Shutdown();
        return 1;
    }

    // 3. MIDI ポートの選択
    int midiCount = Synth_GetMidiPortCount();
    std::cout << "\n[MIDI Ports]" << std::endl;
    char nameBuf[256];
    for (int i = 0; i < midiCount; ++i) {
        Synth_GetMidiPortName(i, nameBuf, sizeof(nameBuf));
        std::cout << "  [" << i << "] " << nameBuf << std::endl;
    }

    std::cout << "Enter MIDI Port number (e.g., 0): ";
    int midiPort = 0;
    std::cin >> midiPort;
    if (!Synth_OpenMidiPort(midiPort)) {
        std::cerr << "Failed to open MIDI port.\n";
        Synth_Shutdown();
        return 1;
    }

    // 4. オーディオ出力デバイスの選択
    int audioCount = Synth_GetAudioDeviceCount();
    std::cout << "\n[Audio Devices]" << std::endl;
    for (int i = 0; i < audioCount; ++i) {
        Synth_GetAudioDeviceName(i, nameBuf, sizeof(nameBuf));
        std::cout << "  [" << i << "] " << nameBuf << std::endl;
    }

    std::cout << "Enter Audio Device number (e.g., 0 for VoiceMeeter/Headphone): ";
    int audioDevice = 0;
    std::cin >> audioDevice;
    if (!Synth_StartAudio(audioDevice)) {
        std::cerr << "Failed to start Audio.\n";
        Synth_Shutdown();
        return 1;
    }

    std::cout << "\n============================================\n";
    std::cout << "  Ready! You can play your keyboard now.\n";
    std::cout << "  [Commands to test Core Features]:\n";
    std::cout << "    v <0.0 - 1.0> : Change Volume (e.g., 'v 0.3')\n";
    std::cout << "    p             : Toggle Pedal ON/OFF\n";
    std::cout << "    panic         : Force stop all notes (Panic)\n";
    std::cout << "    q             : Quit\n";
    std::cout << "============================================\n\n";

    std::string cmd;
    bool pedalState = true;
    while (std::cin) {
        std::cout << "> ";
        if (!(std::cin >> cmd)) {
            break;
        }
        if (cmd == "q") {
            break;
        } else if (cmd == "v") {
            float vol = 1.0f;
            std::cin >> vol;
            Synth_SetMasterVolume(vol);
            std::cout << "[Test] Volume set to: " << vol << std::endl;
        } else if (cmd == "p") {
            pedalState = !pedalState;
            Synth_SetPedalEnabled(pedalState);
            std::cout << "[Test] Pedal is now: " << (pedalState ? "ENABLED" : "DISABLED") << std::endl;
        } else if (cmd == "panic") {
            Synth_AllNotesOff();
            std::cout << "[Test] All notes stopped.\n";
        }
    }

    Synth_Shutdown();
    std::cout << "Engine shutdown cleanly.\n";
    return 0;
}