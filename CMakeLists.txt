cmake_minimum_required(VERSION 2.4)
project(Synth)
set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(RTAUDIO REQUIRED rtaudio)
pkg_check_modules(RTMIDI REQUIRED rtmidi)

add_executable(synth
  src/main.cpp
)

target_link_libraries(synth ${RTAUDIO_LIBRARIES} ${RTMIDI_LIBRARIES})
target_include_directories(synth PUBLIC ${RTAUDIO_INCLUDE_DIRS} ${RTMIDI_INCLUDE_DIRS})
target_compile_options(synth PUBLIC ${RTAUDIO_CFLAGS_OTHER} ${RTMIDI_CFLAGS_OTHER})
