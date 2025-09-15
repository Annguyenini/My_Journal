cmake_minimum_required(VERSION 3.16)

project(trip VERSION 0.1 LANGUAGES CXX)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets SerialPort WebEngineWidgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialPort WebEngineWidgets Multimedia MultimediaWidgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(SDL2 REQUIRED)

pkg_check_modules(LIBCAMERA REQUIRED IMPORTED_TARGET libcamera)
pkg_check_modules(GST REQUIRED gstreamer-1.0>=1.16 gstreamer-video-1.0>=1.16)
pkg_check_modules(GST_BASE REQUIRED gstreamer-base-1.0)
pkg_check_modules(GST_APP REQUIRED gstreamer-app-1.0)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.ui
        src/album/album.cpp
        src/album/albumworker.cpp
        src/default/button.cpp
        src/default/map.cpp
        src/default/statusbar.cpp
        src/gps/gps.cpp
        src/gps/gpsworker.cpp
        src/trip/triplayout.cpp
        src/trip/tripworker.cpp
        
        include/mainwindow.h
        include/button.h
        include/gps.h
        include/json.hpp
        include/album.h
        include/configure.h
        include/trip.h

)
include_directories(${CMAKE_SOURCE_DIR}/include)  # where libcam2opencv.h lives
include_directories(/usr/include/opencv4/opencv2) 
include_directories(/usr/local/include)
include_directories(/usr/include)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(trip
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    if(WIN32)
        target_link_options(trip PRIVATE -municode)
    endif()
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET trip APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(trip SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(trip
            ${PROJECT_SOURCES}
        )
        if(WIN32)
            target_link_options(trip PRIVATE -municode)
        endif()
    endif()
endif()

target_link_libraries(trip PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::SerialPort Qt${QT_VERSION_MAJOR}::WebEngineWidgets Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
 Qt${QT_VERSION_MAJOR}::OpenGLWidgets Qt6::Multimedia
    Qt6::MultimediaWidgets ${OpenCV_LIBS}
    ${CMAKE_SOURCE_DIR}/lib/libcam2opencv.a PkgConfig::LIBCAMERA
)
target_link_libraries(trip PRIVATE
    ${GST_LIBRARIES}
    ${GST_BASE_LIBRARIES}
    ${GST_APP_LIBRARIES}
    ${SDL2_LIBRARIES}

)
target_include_directories(trip PRIVATE
    ${GST_INCLUDE_DIRS}
    ${GST_BASE_INCLUDE_DIRS}
    ${GST_APP_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}

)

target_include_directories(trip PRIVATE ${SQLite3_INCLUDE_DIRS})
target_link_libraries(trip PRIVATE ${SQLite3_LIBRARIES} )
target_link_libraries(trip PRIVATE glib-2.0)

target_include_directories(trip PRIVATE /usr/include/boost)
target_include_directories(trip PRIVATE
    ${Qt${QT_VERSION_MAJOR}SerialPort_INCLUDE_DIRS}
)
target_include_directories(trip PRIVATE
    ${GST_INCLUDE_DIRS}
)
link_directories(
  ${GST_LIBRARY_DIRS}
  ${GST_BASE_LIBRARY_DIRS}
  ${GST_APP_LIBRARY_DIRS}
)
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.trip)
endif()
set_target_properties(trip PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS trip
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(trip)
endif()

