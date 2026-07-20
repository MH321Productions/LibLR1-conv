cmake_minimum_required(VERSION 4.4)
project(LibLR1-conv)

set(CMAKE_CXX_STANDARD 20)


add_library(LR1 SHARED
        src/formats/bmp.cpp
        src/io/BinaryReader.cpp
        src/jam/JamEntries.cpp
        src/jam/JamExtractor.cpp
)
target_include_directories(LR1 PUBLIC include)
target_include_directories(LR1 PRIVATE external)