cmake_minimum_required(VERSION 3.16)
project(MidiSynth LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# テストプログラムや不要な重複ターゲットの生成をOFFにする
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(RTAUDIO_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(RTMIDI_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(RTAUDIO_API_WASAPI ON CACHE BOOL "" FORCE)
set(RTMIDI_API_WINMM ON CACHE BOOL "" FORCE)
set(RTMIDI_TARGETNAME_UNINSTALL "rtmidi-uninstall" CACHE STRING "" FORCE)

# RtAudio の取得 (WASAPI対応)
set(RTAUDIO_API_WASAPI ON CACHE BOOL "" FORCE)
FetchContent_Declare(
    rtaudio
    GIT_REPOSITORY https://github.com/thestk/rtaudio.git
    GIT_TAG        6.0.1
)

# RtMidi の取得 (Windows MM対応)
FetchContent_Declare(
    rtmidi
    GIT_REPOSITORY https://github.com/thestk/rtmidi.git
    GIT_TAG        6.0.0
)

FetchContent_MakeAvailable(rtaudio rtmidi)

include_directories(include)

add_executable(midi_synth
    src/main.cpp
    src/SynthVoice.cpp
    src/MidiHandler.cpp
    src/AudioEngine.cpp
)

target_link_libraries(midi_synth PRIVATE rtaudio rtmidi)

if (WIN32)
    target_link_libraries(midi_synth PRIVATE winmm ksuser mfplat mfuuid wmcodecdspuuid -static -static-libgcc -static-libstdc++)
endif()