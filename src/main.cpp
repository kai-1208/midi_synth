// src/main.cpp
#include <iostream>
#include <thread>
#include <chrono>
#include "SynthVoice.hpp"
#include "MidiHandler.hpp"
#include "AudioEngine.hpp"

int main() {
    std::cout << "=== Low-Latency MIDI Synth ===" << std::endl;

    SynthEngine synth;

    // MIDIハンドラのセットアップ
    MidiHandler midi([&synth](bool isNoteOn, int note, int vel) {
        if (isNoteOn) {
            synth.noteOn(note, vel);
        } else {
            synth.noteOff(note);
        }
    });

    midi.listPorts();

    std::cout << "\nEnter MIDI Port number for P-45 (e.g., 0): ";
    unsigned int port = 0;
    std::cin >> port;

    if (!midi.openPort(port)) {
        std::cerr << "Failed to open MIDI port." << std::endl;
        return 1;
    }
    std::cout << "MIDI Port opened successfully." << std::endl;

    // オーディオエンジンの起動
    AudioEngine audio(synth);
    if (!audio.start()) {
        std::cerr << "Failed to start Audio Engine." << std::endl;
        return 1;
    }

    std::cout << "\nReady! Play your P-45 keyboard. (Press Enter to exit)\n";
    std::cin.ignore();
    std::cin.get();

    audio.stop();
    return 0;
}