cmake_minimum_required(VERSION 3.5)

project(HabitTracker VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(app_core)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Gui Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Gui Multimedia)
find_package(JKQTPlotter${QT_VERSION_MAJOR} REQUIRED)

set(PROJECT_SOURCES
    ImportDialog.hpp
    RegexInputDialog.cpp
    ListViewCustom.cpp
    RegexInputDialog.hpp
    ListViewCustom.hpp
    resource.qrc
    main.cpp
    SoundPlayer.cpp
    HabitDelegate.cpp
    MainWindow.cpp
    SoundPlayer.hpp
    HabitDelegate.hpp
    MainWindow.hpp
    StatusProxyModel.cpp
    HabitDialog.cpp
    MainWindow.ui
    StatusProxyModel.hpp
    HabitDialog.hpp
    HabTableView.cpp
    HabTableView.hpp
    ReccurenceDialog.cpp
    ImportDialog.cpp
    ReccurenceDialog.hpp
    ScoreGraph.hpp
    ScoreGraph.cpp
    ScoreGraph.ui
    CircleProgressBar.hpp
    CircleProgressBar.cpp
    StatsDialog.hpp
    StatsDialog.cpp
    StatsDialog.ui
    RelativeResultGraph.cpp
    RelativeResultGraph.hpp
    RelativeResultGraph.ui
    ResultsBarGraph.cpp
    ResultsBarGraph.hpp
    ResultsBarGraph.ui
    SettingsDialog.hpp
    SettingsDialog.cpp
    SettingsDialog.ui
    ScoreGraphSettingsDialog.hpp ScoreGraphSettingsDialog.cpp ScoreGraphSettingsDialog.ui
    JKQTPlotterCustom.hpp JKQTPlotterCustom.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(HabitTracker
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET HabitTracker APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(HabitTracker SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(HabitTracker
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} "${PROJECT_SOURCE_DIR}/app_core")
#include_directories(${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(HabitTracker PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Multimedia)
target_link_libraries(${PROJECT_NAME} PUBLIC app_core)
target_link_libraries(${PROJECT_NAME} PRIVATE JKQTPlotter6::JKQTPlotter6)

set_target_properties(HabitTracker PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS HabitTracker
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(HabitTracker)
endif()
