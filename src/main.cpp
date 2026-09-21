// src/main.cpp
#include <iostream>
#include <thread>
#include <chrono>
#include "SynthVoice.hpp"
#include "MidiHandler.hpp"
#include "AudioEngine.hpp"

int main() {
    std::cout << "=== Low-Latency SoundFont Piano Synth ===" << std::endl;

    SynthEngine synth;

    // 1. SoundFont の読み込み
    std::string sfPath = "piano.sf2";
    if (!synth.loadSoundFont(sfPath)) {
        std::cerr << "Please ensure '" << sfPath << "' exists in the working directory.\n";
        return 1;
    }

    // 2. MIDIハンドラのセットアップ
    MidiHandler midi([&synth](bool isNoteOn, int note, int vel) {
        if (isNoteOn) {
            synth.noteOn(note, vel);
        } else {
            synth.noteOff(note);
        }
    });

    midi.listPorts();

    std::cout << "\nEnter MIDI Port number (e.g., 0): ";
    unsigned int port = 0;
    std::cin >> port;

    if (!midi.openPort(port)) {
        std::cerr << "Failed to open MIDI port." << std::endl;
        return 1;
    }
    std::cout << "MIDI Port opened successfully." << std::endl;

    // 3. オーディオエンジンの起動
    AudioEngine audio(synth);
    if (!audio.start()) {
        std::cerr << "Failed to start Audio Engine." << std::endl;
        return 1;
    }

    std::cout << "\n[Ready] Play your P-45! (Press Enter to exit)\n";
    std::cin.ignore();
    std::cin.get();

    audio.stop();
    return 0;
}