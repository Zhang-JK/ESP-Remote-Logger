cmake_minimum_required(VERSION 3.21)
project(LoggerClient)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        Network
        REQUIRED)

add_executable(LoggerClient main.cpp asset/resource.qrc ConnectionPage/Connection.cpp ConnectionPage/Connection.h ConnectionPage/Connection.ui NetDataHandler/DataHandler.cpp NetDataHandler/DataHandler.h Config.h DataPage/Dashboard.cpp DataPage/Dashboard.h DataPage/Dashboard.ui)
target_link_libraries(LoggerClient
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Network
        )

