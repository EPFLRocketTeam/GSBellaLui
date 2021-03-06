cmake_minimum_required(VERSION 3.7)
project(ground_station LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport
        3DCore 3DExtras 3DRender 3DInput REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

qt5_wrap_ui(UIS_HDRS /UI/gswidget.ui)
qt5_add_resources(RESOURCES qt_resources.qrc)

set_property(SOURCE ${CMAKE_BINARY_DIR}/ground_station/ui_gswidget.h
        ${CMAKE_BINARY_DIR}/ground_station/qrc_qt_resources.cpp PROPERTY SKIP_AUTOGEN ON)

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

add_executable(ground_station ${SOURCES} ${UIS_HDRS} ${HEADERS} ${RESOURCES})
add_library(ground_station_lib ${SOURCES} ${HEADERS})

# Include QCustomPlot libtrary and include file
include_directories(${CMAKE_BINARY_DIR}/qcustomplot-src/)
target_link_libraries(ground_station qcustomplot_lib)
target_link_libraries(ground_station_lib qcustomplot_lib)

#Include Boost
target_link_libraries(ground_station Boost::boost Boost::system Boost::thread Boost::filesystem Boost::iostreams)
target_link_libraries(ground_station_lib Boost::boost Boost::system Boost::thread Boost::filesystem Boost::iostreams)

# Include serial library
include_directories(${CMAKE_BINARY_DIR}/serial-src/include/)
target_link_libraries(ground_station serial)
target_link_libraries(ground_station_lib serial)

# This will link_libraries for the specified Qt modules
target_link_libraries(ground_station
        Qt5::Core Qt5::Gui Qt5::Widgets
        Qt5::3DCore Qt5::3DExtras Qt5::3DRender Qt5::3DInput)

# This will link_libraries for the specified Qt modules
target_link_libraries(ground_station_lib
        Qt5::Core Qt5::Gui Qt5::Widgets
        Qt5::3DCore Qt5::3DExtras Qt5::3DRender Qt5::3DInput)