cmake_minimum_required(VERSION 3.16)
project(MidiSynthCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# テストターゲット等の重複生成を防止
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(RTAUDIO_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(RTMIDI_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(RTMIDI_TARGETNAME_UNINSTALL "rtmidi-uninstall" CACHE STRING "" FORCE)

set(RTAUDIO_API_WASAPI ON CACHE BOOL "" FORCE)
set(RTMIDI_API_WINMM ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    rtaudio
    GIT_REPOSITORY https://github.com/thestk/rtaudio.git
    GIT_TAG        6.0.1
)
FetchContent_Declare(
    rtmidi
    GIT_REPOSITORY https://github.com/thestk/rtmidi.git
    GIT_TAG        6.0.0
)

FetchContent_MakeAvailable(rtaudio rtmidi)

include_directories(include)

# 1. DLL（共有ライブラリ）のビルド
add_library(midi_synth_core SHARED
    src/MidiSynthApi.cpp
    src/SynthVoice.cpp
    src/MidiHandler.cpp
    src/AudioEngine.cpp
)

set_target_properties(midi_synth_core PROPERTIES 
    PREFIX ""
    DEBUG_POSTFIX ""
)

target_link_libraries(midi_synth_core PRIVATE rtaudio rtmidi)

if (WIN32)
    target_link_libraries(midi_synth_core PRIVATE winmm ole32 uuid ksuser mfplat mfuuid wmcodecdspuuid)
endif()

# 2. テスト用 EXE のビルド
add_executable(midi_synth_test
    src/main.cpp
)

target_link_libraries(midi_synth_test PRIVATE midi_synth_core)

add_custom_command(TARGET midi_synth_core POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:rtaudio>
            $<TARGET_FILE_DIR:midi_synth_core>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:rtmidi>
            $<TARGET_FILE_DIR:midi_synth_core>
)

# MinGW 環境ですべての依存ライブラリを静的リンクして単体起動可能にする
if (WIN32 AND MINGW)
    target_link_options(midi_synth_core PRIVATE -static -static-libgcc -static-libstdc++)
    target_link_options(midi_synth_test PRIVATE -static -static-libgcc -static-libstdc++)
endif()