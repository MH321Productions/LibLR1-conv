cmake_minimum_required(VERSION 3.19) #TODO: Check if this version works
project(LibLR1-conv)

set(CMAKE_CXX_STANDARD 20)

# Main Project check
# determine if LibLR1 is built as a subproject (using add_subdirectory) or if it is the main project
set(MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MAIN_PROJECT ON)
endif()

# Imports
find_package(nlohmann_json REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(glm REQUIRED)

pkg_check_modules(libav REQUIRED IMPORTED_TARGET
    libavformat
    libavcodec
    libavutil
)

# Options
option(LR1_BUILD_TESTS "Build and run the unit tests" ${MAIN_PROJECT})
option(LR1_BUILD_CONV "Build the simple converter program" ${MAIN_PROJECT})


# Actual library
add_library(LR1 SHARED
        src/LR1/types.cpp
        src/LR1/io/reader.cpp
        src/LR1/io/writer.cpp
        src/LR1/io/binaryfilehelper.cpp
        src/LR1/io/serializers.cpp
        src/LR1/jam/entries.cpp
        src/LR1/jam/extractor.cpp
        src/LR1/formats/bmp.cpp
        src/LR1/formats/srf.cpp
        src/LR1/formats/audio.cpp
        src/LR1/formats/bvb.cpp
        src/LR1/formats/mdb.cpp
        src/LR1/utils/color.cpp
)
target_include_directories(LR1 PUBLIC include)
target_include_directories(LR1 PRIVATE external)
target_link_libraries(LR1 nlohmann_json::nlohmann_json PkgConfig::libav)

if (LR1_BUILD_CONV)
    add_library(tinyfiledialogs STATIC external/tinyfiledialogs/tinyfiledialogs.c)

    add_executable(LR1-conv WIN32
            src/Converter/main.cpp
    )
    target_include_directories(LR1-conv PUBLIC include)
    target_include_directories(LR1-conv PUBLIC external)
    target_link_libraries(LR1-conv PUBLIC LR1 tinyfiledialogs)
endif()

# Tests
if (LR1_BUILD_TESTS)
    include(CTest)
    enable_testing()
    message(STATUS "Building tests is enabled")

    # Jam Extractor
    add_executable(JamExtractor test/jam/extractJam.cpp)
    target_link_libraries(JamExtractor LR1)
    add_test(NAME "JamExtractor with real file" COMMAND JamExtractor res/NORMAL.JAM WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
    add_test(NAME "JamExtractor with wrong file" COMMAND JamExtractor res/OTHER.JAM WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
    set_property(TEST "JamExtractor with wrong file" PROPERTY WILL_FAIL TRUE)
endif ()