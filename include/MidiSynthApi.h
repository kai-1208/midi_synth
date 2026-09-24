#pragma once

#ifdef _WIN32
  #define SYNTH_API __declspec(dllexport)
#else
  #define SYNTH_API
#endif

extern "C" {
    // ライフサイクル
    SYNTH_API bool Synth_Init();
    SYNTH_API void Synth_Shutdown();

    // 音源・設定
    SYNTH_API bool Synth_LoadSoundFont(const char* path);
    SYNTH_API void Synth_SetMasterVolume(float volume); // 0.0f ~ 1.0f
    SYNTH_API void Synth_SetPedalEnabled(bool enabled);
    SYNTH_API void Synth_AllNotesOff();

    // MIDI デバイス
    SYNTH_API int  Synth_GetMidiPortCount();
    SYNTH_API bool Synth_GetMidiPortName(int index, char* outBuffer, int bufferSize);
    SYNTH_API bool Synth_OpenMidiPort(int index);

    // オーディオ デバイス
    SYNTH_API int  Synth_GetAudioDeviceCount();
    SYNTH_API bool Synth_GetAudioDeviceName(int index, char* outBuffer, int bufferSize);
    SYNTH_API bool Synth_StartAudio(int deviceIndex);
    SYNTH_API void Synth_StopAudio();
}