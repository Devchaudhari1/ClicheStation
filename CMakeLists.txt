cmake_minimum_required(VERSION 3.16)

project(ClicheStation
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 CONFIG REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

# adds an executable named CicheStation which is created from the src mentioned
qt_add_executable(ClicheStation
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MidiInput.cpp
    MidiInput.h

    # RtMidi source
    RtMidi.cpp
    RtMidi.h
)

# links against -lQt6Widgets 
target_link_libraries(ClicheStation
    PRIVATE
        Qt6::Widgets
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

