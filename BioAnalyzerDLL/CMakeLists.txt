# BioAnalyzerDLL/CMakeLists.txt
cmake_minimum_required(VERSION 3.5)
project(BioAnalyzerDLL LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the DLL
add_library(BioAnalyzerDLL SHARED
    BioAnalyzerDLL.cpp
)

# Define export macro for Windows
target_compile_definitions(BioAnalyzerDLL PRIVATE BIOANALYZERDLL_EXPORTS)

# Set output name for the DLL (without .dll extension)
set_target_properties(BioAnalyzerDLL PROPERTIES
    OUTPUT_NAME BioAnalyzer 
    WINDOWS_EXPORT_ALL_SYMBOLS TRUE
)