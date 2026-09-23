// src/MidiHandler.cpp
#include "MidiHandler.hpp"
#include <iostream>

MidiHandler::MidiHandler(NoteCallback noteCb, ControlCallback ctrlCb)
    : noteCallback(noteCb), controlCallback(ctrlCb) {
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
    int data1 = message->at(1);
    int data2 = message->at(2);

    // 1. ノートオン / ノートオフ
    if (status == 0x90 && data2 > 0) {
        if (self->noteCallback) self->noteCallback(true, data1, data2);
    } else if (status == 0x80 || (status == 0x90 && data2 == 0)) {
        if (self->noteCallback) self->noteCallback(false, data1, 0);
    }
    // 2. コントロールチェンジ（サステインペダル等: 0xB0）
    else if (status == 0xB0) {
        if (self->controlCallback) self->controlCallback(data1, data2);
    }
}