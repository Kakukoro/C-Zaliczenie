cmake_minimum_required(VERSION 3.5)
project(BioAnalyzerDLL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(BioAnalyzerDLL SHARED BioAnalyzerDLL.cpp)
target_compile_definitions(BioAnalyzerDLL PRIVATE BIOANALYZERDLL_EXPORTS)
set_target_properties(BioAnalyzerDLL PROPERTIES
    OUTPUT_NAME BioAnalyzer
    WINDOWS_EXPORT_ALL_SYMBOLS TRUE
)