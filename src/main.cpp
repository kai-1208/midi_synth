#include <iostream>
#include <string>
#include <sstream>
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

    // 2. MIDI 入力デバイスの選択
    MidiHandler midi([&synth](bool isNoteOn, int note, int vel) {
        if (isNoteOn) {
            synth.noteOn(note, vel);
        } else {
            synth.noteOff(note);
        }
    });

    midi.listPorts();
    std::cout << "\nEnter MIDI Port number for P-45 (e.g., 0): ";
    unsigned int midiPort = 0;
    std::cin >> midiPort;

    if (!midi.openPort(midiPort)) {
        std::cerr << "Failed to open MIDI port." << std::endl;
        return 1;
    }
    std::cout << "MIDI Port opened successfully.\n";

    // 3. オーディオ出力デバイスの選択
    AudioEngine audio(synth);
    audio.listOutputDevices();

    std::cout << "\nEnter Audio Device number (or press Enter for default): ";
    std::string line;
    std::cin.ignore(); // 直前の改行をクリア
    std::getline(std::cin, line);

    int audioIndex = -1;
    if (!line.empty()) {
        std::stringstream ss(line);
        ss >> audioIndex;
    }

    // 4. オーディオエンジンの起動
    if (!audio.start(audioIndex)) {
        std::cerr << "Failed to start Audio Engine." << std::endl;
        return 1;
    }

    std::cout << "\n[Ready] Play your P-45! (Press Enter to exit)\n";
    std::cin.get();

    audio.stop();
    return 0;
}