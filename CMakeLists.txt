cmake_minimum_required(VERSION 3.5)
project(ff_player)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED QUIET)

find_package(
        FFmpeg
        REQUIRED
        COMPONENTS
        avformat
        avutil
        swscale
        swresample
        avfilter
        avdevice
        avcodec)

if(${FFMPEG_FOUND})
    include_directories(${FFMPEG_INCLUDE_DIRS})

else()

    message(WARNING "ffmpeg not found")

endif(${FFMPEG_FOUND})

find_package(OpenAL)

if (${OPENAL_FOUND})
    include_directories(${OPENAL_INCLUDE_DIR})

else()

    message(WARNING "OpelAL not found !")

endif ()

set(
        FF_SRCS

        main.cpp
        openal_audio_render.h
        openal_audio_render.cpp
        opengl_video_render.h
        opengl_video_render.cpp
        sync_queue.h
        sync_queue.cpp
        video_player.h
        video_player.cpp
        input_thread.h
        input_thread.cpp
        video_player_thread.h
        video_player_thread.cpp
        pcm_transformat.h
        pcm_transformat.cpp
        image_transformat.h
        image_transformat.cpp
)

qt5_add_resources(RENDER_QRC assets/render.qrc)

add_executable(
        ff_player
        ${RENDER_QRC}
        ${FF_SRCS}
)

target_link_libraries(
        ff_player
        Qt5::Widgets
        Qt5::OpenGL
        ${FFMPEG_LIBRARIES}
        ${OPENAL_LIBRARY}
)