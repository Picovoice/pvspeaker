cmake_minimum_required(VERSION 3.4)
project(pv_speaker_napi)

include(ExternalProject)

set(NODE_VERSION 18.16.1)

ExternalProject_Add(node_headers
        URL https://nodejs.org/dist/v${NODE_VERSION}/node-v${NODE_VERSION}-headers.tar.xz
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
)
ExternalProject_Get_Property(node_headers SOURCE_DIR)
set(NODE_INCLUDE_DIR ${SOURCE_DIR}/include/node/)

if (${PV_SPEAKER_PLATFORM} STREQUAL "windows")
    set(NODE_WIN_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/node_win_lib )
    ExternalProject_Add(node_win_lib
            URL https://nodejs.org/dist/v${NODE_VERSION}/win-x64/node.exe
            DOWNLOAD_NO_EXTRACT on
            DOWNLOAD_DIR ${NODE_WIN_LIB_DIR}
            CONFIGURE_COMMAND ""
            BUILD_COMMAND gendef.exe ${NODE_WIN_LIB_DIR}/node.exe
                COMMAND dlltool.exe -d node.def -y ${NODE_WIN_LIB_DIR}/node.a
            INSTALL_COMMAND ""
    )
    set(NODE_LIB ${NODE_WIN_LIB_DIR}/node.a)
endif()

add_library(pv_speaker_napi SHARED pv_speaker_napi.c $<TARGET_OBJECTS:pv_speaker_object>)
set_target_properties(pv_speaker_napi PROPERTIES
        PREFIX ""
        OUTPUT_NAME "pv_speaker"
        SUFFIX ".node"
)
add_dependencies(pv_speaker_napi node_headers)

if (${PV_SPEAKER_PLATFORM} STREQUAL "windows")
    add_dependencies(pv_speaker_napi node_win_lib)
    target_sources(pv_speaker_napi PRIVATE delayhook.c)
endif()

target_include_directories(pv_speaker_napi PUBLIC ${PROJECT_SOURCE_DIR}/../include)
target_include_directories(pv_speaker_napi PRIVATE ${NODE_INCLUDE_DIR})

target_link_libraries(pv_speaker_napi ${NODE_LIB})

if (${PV_SPEAKER_PLATFORM} STREQUAL "mac-x86_64" OR ${PV_SPEAKER_PLATFORM} STREQUAL "mac-arm64")
    target_link_options(pv_speaker_napi PRIVATE "-undefined" "dynamic_lookup")
endif()

if(DEFINED OUTPUT_DIR)
    add_custom_command(TARGET pv_speaker_napi POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${PROJECT_NAME}>
            "${CMAKE_SOURCE_DIR}/../lib/node/${OUTPUT_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
            COMMENT "Copying node lib to output directory.")
endif()