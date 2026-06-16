cmake_minimum_required(VERSION 3.16)

project(BioAnalyzerApp VERSION 0.1 LANGUAGES CXX)

# Enable Qt meta-object compiler and UI compiler
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt (supports Qt5 and Qt6)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

# --- Add the DLL subproject ---
add_subdirectory(BioAnalyzerDLL)

# --- Translation files ---
set(TS_FILES BioAnalyzerApp_pl_PL.ts)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})
endif()

# --- Qt app sources ---
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${QM_FILES}  # Include translation files
)

# --- Create the Qt executable ---
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BioAnalyzerApp
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(BioAnalyzerApp SHARED ${PROJECT_SOURCES})
    else()
        add_executable(BioAnalyzerApp ${PROJECT_SOURCES})
    endif()
endif()

# Link Qt libraries
target_link_libraries(BioAnalyzerApp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# --- Copy the DLL to the output directory (Windows only) ---
if(WIN32)
    add_custom_command(TARGET BioAnalyzerApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:BioAnalyzerDLL>"  # Source: Built DLL
            "$<TARGET_FILE_DIR:BioAnalyzerApp>/BioAnalyzer.dll"  # Destination: Next to the .exe
    )
endif()

# --- macOS/iOS bundle settings ---
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BioAnalyzerApp)
endif()
set_target_properties(BioAnalyzerApp PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# --- Installation (optional) ---
include(GNUInstallDirs)
install(TARGETS BioAnalyzerApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalize Qt6 executable
if(${QT_VERSION_MAJOR} EQUAL 6)
    qt_finalize_executable(BioAnalyzerApp)
endif()