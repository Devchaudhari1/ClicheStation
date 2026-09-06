cmake_minimum_required(VERSION 3.16)

project(ClicheStation
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 CONFIG REQUIRED COMPONENTS Core Gui Widgets Multimedia)
find_package(Threads REQUIRED)

qt_standard_project_setup()

# adds an executable named ClicheStation which is created from the src mentioned
qt_add_executable(ClicheStation
    # add playlist 
    gui/playlist/clip.cpp
    gui/playlist/clip.h
    gui/playlist/ClipArea.cpp
    gui/playlist/ClipArea.h
    gui/playlist/layer.cpp
    gui/playlist/layer.h
    gui/playlist/playlist.cpp
    gui/playlist/playlist.h
    gui/playlist/sequence.cpp
    gui/playlist/sequence.h
    gui/playlist/TimelineRow.cpp
    gui/playlist/TimelineRow.h
    gui/playlist/TimelineWidget.cpp
    gui/playlist/TimelineWidget.h
    gui/playlist/TimeRuler.cpp
    gui/playlist/TimeRuler.h


    #main gui
    gui/base.cpp
    gui/base.h
    gui/drumPad.cpp
    gui/drumPad.h
    gui/instrument_settings.cpp
    gui/instrument_settings.h
    gui/main.cpp
    gui/monitorWindow.cpp
    gui/monitorWindow.h
    gui/piano.cpp
    gui/piano.h
    gui/pianoKeyboard.cpp
    gui/pianoKeyboard.h
    gui/pianoRoll.cpp
    gui/pianoRoll.h
    gui/track_list.cpp
    gui/track_list.h
    gui/voice_track.cpp
    gui/voice_track.h

    # Midi source
    midi/RtMidi.cpp
    midi/RtMidi.h
    midi/MidiInput.cpp
    midi/MidiInput.h

    # Synth source
    synth/ADSREnvelope.cpp
    synth/ADSREnvelope.h
    synth/InstrumentParameters.h
    synth/Oscillator.cpp
    synth/Oscillator.h
    synth/SynthEngine.cpp
    synth/SynthEngine.h
    synth/Voice.cpp
    synth/Voice.h

    # Audio Engine
    audio/AudioEngine.cpp
    audio/AudioEngine.h
    audio/TrackProcessor.cpp
    audio/TrackProcessor.h
    
    # Audio Effects
    audio/effects/AudioEffect.h
    audio/effects/Clipping.cpp
    audio/effects/Delay.cpp
    audio/effects/Distortion.cpp
    audio/effects/Distortion.h
    audio/effects/EffectChain.cpp
    audio/effects/EFfectChain.h
    audio/effects/Saturation.cpp
)

# links against -lQt6Widgets 
target_link_libraries(ClicheStation
    PRIVATE
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Multimedia
        Threads::Threads
)

# add WIN32_EXECUTABLE to properties for GUI build
set_target_properties(ClicheStation PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)


# ====== PLATFORM-SPECIFIC RTMIDI CONFIGURATION ======

# Windows 
#WIN32 is built-in cmake variable which automatically set to true for windows platform
if(WIN32)
    # adds a preprocessor macro equivalent to #define __WINDOWS_MM__
    target_compile_definitions(ClicheStation PRIVATE __WINDOWS_MM__)
    # links the target with WIndows MultiMedia Library
    target_link_libraries(ClicheStation PRIVATE winmm)
    message(STATUS "RtMidi: Configured for Windows MM API")
endif()

# macOS
#APPLE is built-in cmake variable which automatically set to true for apple platform

if(APPLE)
    # adds a preprocessor macro equivalent to #define __MACOSX_CORE__
    target_compile_definitions(ClicheStation PRIVATE __MACOSX_CORE__)
    target_link_libraries(ClicheStation PRIVATE 
        "-framework CoreMIDI"
        "-framework CoreAudio"
        "-framework CoreFoundation"
        "-framework CoreServices"
    )
    message(STATUS "RtMidi: Configured for macOS CoreMIDI API")
endif()


# Linux - check for ALSA or JACK
# UNIX automatically sets to true for LINUX
if(UNIX AND NOT APPLE)
    # Try ALSA first
    find_package(ALSA QUIET)
    if(ALSA_FOUND)
        target_compile_definitions(ClicheStation PRIVATE __LINUX_ALSA__)
        target_link_libraries(ClicheStation PRIVATE ALSA::ALSA)
        message(STATUS "RtMidi: Configured for Linux ALSA API")
    else()
        # Fall back to JACK
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(JACK QUIET jack)
            if(JACK_FOUND)
                target_compile_definitions(ClicheStation PRIVATE __UNIX_JACK__)
                target_link_libraries(ClicheStation PRIVATE ${JACK_LIBRARIES})
                target_include_directories(ClicheStation PRIVATE ${JACK_INCLUDE_DIRS})
                message(STATUS "RtMidi: Configured for JACK API")
            else()
                message(WARNING "RtMidi: No MIDI API found (ALSA or JACK)")
            endif()
        endif()
    endif()
endif()

# Debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(ClicheStation PRIVATE __RTMIDI_DEBUG__)
endif()

message(STATUS "Build configuration complete")

