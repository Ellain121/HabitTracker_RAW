cmake_minimum_required(VERSION 3.14)

project(app_core
    VERSION "0.2.1"
    LANGUAGES CXX)


set(APP_NAME "HabitTracker")
set(ORG_NAME "Nop Inc.")


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Sql)

configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_SOURCE_DIR}/config.h)

add_library(app_core SHARED
    app_core_global.hpp
    DatabaseManager.hpp
    Habit.hpp
    ImportLoader.hpp
    CMakeLists.txt
    Habit.cpp
    HabitModel.cpp
    Utility.cpp
    Constants.hpp
    HabitDao.cpp
    HabitModel.hpp
    Utility.hpp
    DatabaseManager.cpp
    HabitDao.hpp
    ImportLoader.cpp
    Settings.hpp
    Settings.cpp
    config.h
)

target_link_libraries(app_core PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Sql)

target_compile_definitions(app_core PRIVATE APP_CORE_LIBRARY)
