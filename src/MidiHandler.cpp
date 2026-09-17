// src/MidiHandler.cpp
#include "MidiHandler.hpp"
#include <iostream>

MidiHandler::MidiHandler(NoteCallback cb) : callback(cb) {
    try {
        midiIn = std::make_unique<RtMidiIn>();
        midiIn->setCallback(&MidiHandler::midiCallback, this);
        midiIn->ignoreTypes(false, false, false);
    } catch (RtMidiError& error) {
        error.printMessage();
    }
}

MidiHandler::~MidiHandler() {
    if (midiIn && midiIn->isPortOpen()) {
        midiIn->closePort();
    }
}

void MidiHandler::listPorts() {
    unsigned int nPorts = midiIn->getPortCount();
    std::cout << "[MIDI Ports]" << std::endl;
    for (unsigned int i = 0; i < nPorts; ++i) {
        std::cout << "  [" << i << "] " << midiIn->getPortName(i) << std::endl;
    }
}

bool MidiHandler::openPort(unsigned int portNumber) {
    if (portNumber >= midiIn->getPortCount()) return false;
    midiIn->openPort(portNumber);
    return true;
}

void MidiHandler::midiCallback(double /*timeStamp*/, std::vector<unsigned char>* message, void* userData) {
    auto* self = static_cast<MidiHandler*>(userData);
    if (!message || message->size() < 3) return;

    unsigned char status = message->at(0) & 0xF0;
    int note = message->at(1);
    int vel  = message->at(2);

    if (status == 0x90 && vel > 0) {
        self->callback(true, note, vel);  // Note On
    } else if (status == 0x80 || (status == 0x90 && vel == 0)) {
        self->callback(false, note, 0);   // Note Off
    }
}