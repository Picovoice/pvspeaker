cmake_minimum_required(VERSION 3.4)
project(pv_speaker_demo VERSION 1.0.0 DESCRIPTION "Picovoice audio player library demo.")

if(NOT PV_SPEAKER_PLATFORM)
    message(FATAL_ERROR "No `PV_SPEAKER_PLATFORM` value was given. Valid platforms are: \n"
            "linux, mac-arm64, mac-x86_64, windows, raspberry-pi3-32, raspberry-pi3-64,"
            "raspberry-pi4-32, raspberry-pi4-64, raspberry-pi5-32, raspberry-pi5-64")
endif()

if (${PV_SPEAKER_PLATFORM} STREQUAL "mac-arm64")
    set(PV_SPEAKER_LIB_DIR ${CMAKE_SOURCE_DIR}/../../lib/mac/arm64)
elseif (${PV_SPEAKER_PLATFORM} STREQUAL "mac-x86_64")
    set(PV_SPEAKER_LIB_DIR ${CMAKE_SOURCE_DIR}/../../lib/mac/x86_64)
elseif (${PV_SPEAKER_PLATFORM} STREQUAL "linux")
    set(PV_SPEAKER_LIB_DIR ${CMAKE_SOURCE_DIR}/../../lib/linux/x86_64)
elseif (${PV_SPEAKER_PLATFORM} STREQUAL "raspberry-pi3")
    set(PV_SPEAKER_LIB_DIR ${CMAKE_SOURCE_DIR}/../../lib/raspberry-pi/cortex-a53)
elseif (${PV_SPEAKER_PLATFORM} STREQUAL "raspberry-pi3-64")
    set(PV_SPEAKER_LIB_DIR ${CMAKE_SOURCE_DIR}/../../lib/raspberry-pi/cortex-a53-aarch64)
elseif (${PV_SPEAKER_PLATFORM} STREQUAL "raspberry-pi4")
    set(PV_SPEAKER_LIB_DIR ${CMAKE_SOURCE_DIR}/../../lib/raspberry-pi/cortex-a72)
elseif (${PV_SPEAKER_PLATFORM} STREQUAL "raspberry-pi4-64")
    set(PV_SPEAKER_LIB_DIR ${CMAKE_SOURCE_DIR}/../../lib/raspberry-pi/cortex-a72-aarch64)
elseif (${PV_SPEAKER_PLATFORM} STREQUAL "raspberry-pi5")
    set(PV_SPEAKER_LIB_DIR ${CMAKE_SOURCE_DIR}/../../lib/raspberry-pi/cortex-a76)
elseif (${PV_SPEAKER_PLATFORM} STREQUAL "raspberry-pi5-64")
    set(PV_SPEAKER_LIB_DIR ${CMAKE_SOURCE_DIR}/../../lib/raspberry-pi/cortex-a76-aarch64)
elseif (${PV_SPEAKER_PLATFORM} STREQUAL "windows")
    set(PV_SPEAKER_LIB_DIR ${CMAKE_SOURCE_DIR}/../../lib/windows/amd64)
else ()
    message(FATAL_ERROR "Unknown platform `${PV_SPEAKER_PLATFORM}`.")
endif ()

include_directories(../../project/include)
link_directories(${PV_SPEAKER_LIB_DIR})
add_executable(pv_speaker_demo pv_speaker_demo.c)
target_link_libraries(pv_speaker_demo pv_speaker)

add_custom_command(TARGET pv_speaker_demo
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${PV_SPEAKER_LIB_DIR} ${CMAKE_BINARY_DIR}
)