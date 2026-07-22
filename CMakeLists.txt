cmake_minimum_required(VERSION 4.4)
project(LibLR1-conv)

set(CMAKE_CXX_STANDARD 20)

find_package(nlohmann_json REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(libav REQUIRED IMPORTED_TARGET
    libavformat
    libavcodec
    libavutil
)

if (APPLE)
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)
endif ()

add_library(LR1 SHARED
        src/io/reader.cpp
        src/jam/entries.cpp
        src/jam/extractor.cpp
        src/formats/bmp.cpp
        src/formats/srf.cpp
        src/formats/audio.cpp
)
target_include_directories(LR1 PUBLIC include)
target_include_directories(LR1 PRIVATE external)
target_link_libraries(LR1 nlohmann_json::nlohmann_json avformat avcodec avutil)