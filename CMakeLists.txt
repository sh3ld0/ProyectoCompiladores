cmake_minimum_required(VERSION 3.11)
project(music)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
 src/Binasc.cpp
 src/MidiEvent.cpp
 src/MidiEventList.cpp
 src/MidiFile.cpp
 src/MidiMessage.cpp
 main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include) 
