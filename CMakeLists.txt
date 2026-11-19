cmake_minimum_required(VERSION 3.22)
project(personaclient C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(REPO_ROOT "/home/dirkjan/Documents/projects/personaclient")
set(DONKEY_ROOT "/home/dirkjan/Documents/projects/donkey-ai-cloning")
set(ANDROID_PREFIX "${REPO_ROOT}/third_party/android/arm64-v8a")

set(CLIENT_SOURCES
    "${REPO_ROOT}/src/main.c"
    "${REPO_ROOT}/src/log.c"
    "${REPO_ROOT}/src/linkedlist.c"
    "${REPO_ROOT}/src/buspackage.c"
    "${REPO_ROOT}/src/websocketclient.c"
    "${REPO_ROOT}/src/app.c"
    "${REPO_ROOT}/src/cubesplash.c"
    "${REPO_ROOT}/src/statelogo.c"
    "${REPO_ROOT}/src/statetycho.c"
    "${REPO_ROOT}/src/statemanager.c"
    "${REPO_ROOT}/src/ringbuffer.c"
    "${REPO_ROOT}/src/voweldetector.c"
    "${REPO_ROOT}/src/fftprocessor.c"
    "${REPO_ROOT}/src/audiogain.c"
    "${REPO_ROOT}/src/packagering.c"
    "${REPO_ROOT}/src/clonesanta.c"
    "${REPO_ROOT}/src/subtitles.c"
)

if(ANDROID)
    list(APPEND CLIENT_SOURCES
        "${DONKEY_ROOT}/src/cJSON.c"
        "${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c"
    )

    set(PLATFORM "Android" CACHE STRING "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
    add_subdirectory("${REPO_ROOT}/raylib" "${CMAKE_CURRENT_BINARY_DIR}/raylib")

    add_library(personaclient SHARED ${CLIENT_SOURCES})

    target_include_directories(personaclient PRIVATE
        "${REPO_ROOT}/include"
        "${REPO_ROOT}/src"
        "${REPO_ROOT}/raylib/src"
        "${REPO_ROOT}/raylib/src/external"
        "${DONKEY_ROOT}/src"
        "${ANDROID_PREFIX}/include"
        "${ANDROID_NDK}/sources/android/native_app_glue"
    )

    target_link_options(personaclient PRIVATE
        "-Wl,-u,ANativeActivity_onCreate"
    )

    target_link_libraries(personaclient PRIVATE
        raylib
        "${ANDROID_PREFIX}/lib/libcurl.a"
        "${ANDROID_PREFIX}/lib/libwebsockets.a"
        android
        log
        GLESv2
        EGL
        m
    )
else()
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(LWS libwebsockets)
    endif()

    add_executable(personaclient ${CLIENT_SOURCES})

    target_include_directories(personaclient PRIVATE
        "${REPO_ROOT}/include"
        "${REPO_ROOT}/src"
        "${REPO_ROOT}/raylib/src"
        "${REPO_ROOT}/raylib/src/external"
        "${DONKEY_ROOT}/src"
    )

    if(LWS_FOUND)
        target_include_directories(personaclient PRIVATE ${LWS_INCLUDE_DIRS})
        target_link_libraries(personaclient PRIVATE ${LWS_LIBRARIES})
    else()
        target_link_libraries(personaclient PRIVATE websockets)
    endif()

    target_link_libraries(personaclient PRIVATE
        raylib
        X11
        pthread
        dl
        m
    )
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT ANDROID)
    target_compile_options(personaclient PRIVATE -g -fsanitize=address)
    target_link_options(personaclient PRIVATE -fsanitize=address)
endif()
