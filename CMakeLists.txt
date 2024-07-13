cmake_minimum_required(VERSION 3.5)
project(VideoDownloader)

set(CMAKE_CXX_STANDARD 11)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(ICU REQUIRED icu-i18n)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${ICU_INCLUDE_DIRS})

set(CMAKE_AUTOMOC ON)

add_executable(VideoDownloader main.cpp)

target_link_libraries(VideoDownloader Qt5::Widgets ${ICU_LIBRARIES})
