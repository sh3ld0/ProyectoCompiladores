cmake_minimum_required(VERSION 3.11)
project(music)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
 midifile/src/Binasc.cpp
 midifile/src/MidiEvent.cpp
 midifile/src/MidiEventList.cpp
 midifile/src/MidiFile.cpp
 midifile/src/MidiMessage.cpp
 src/Lexer.cpp
 src/Music.cpp
 src/Parser.cpp
 src/Checker.cpp
 main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include/ midifile/include/) 
