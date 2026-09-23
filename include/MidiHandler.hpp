// include/MidiHandler.hpp
#pragma once
#include <RtMidi.h>
#include <memory>
#include <functional>

class MidiHandler {
public:
    using NoteCallback = std::function<void(bool isNoteOn, int note, int vel)>;
    using ControlCallback = std::function<void(int controller, int value)>;

    MidiHandler(NoteCallback noteCb, ControlCallback ctrlCb);
    ~MidiHandler();
    bool openPort(unsigned int portNumber = 0);
    void listPorts();

private:
    static void midiCallback(double timeStamp, std::vector<unsigned char>* message, void* userData);
    std::unique_ptr<RtMidiIn> midiIn;
    NoteCallback noteCallback;
    ControlCallback controlCallback;
};