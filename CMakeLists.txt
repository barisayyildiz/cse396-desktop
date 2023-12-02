cmake_minimum_required(VERSION 3.5)

project(OpenGLProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Gui OpenGL OpenGLWidgets Widgets Charts DataVisualization)

find_package(assimp CONFIG REQUIRED)

find_package(glm REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        Camera.h ColorPickerWidget.h EditableLabel.h global.h ImagePickerWidget.h MainWindow.cpp MainWindow.h MainWindow.ui Material.cpp Material.h Mesh.cpp Mesh.h Model.cpp Model.h openglwidget.cpp openglwidget.h Shader.h stb_image.h



)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(OpenGLProject
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        scannedpoints.h scannedpoints.cpp
        pointcloud.h pointcloud.cpp
        footer.h footer.cpp
        scanner.h scanner.cpp
        capturedimages.h capturedimages.cpp

        )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET OpenGLProject APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(OpenGLProject SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(OpenGLProject
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(OpenGLProject PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Widgets
    Qt6::Charts
    Qt6::DataVisualization
    assimp::assimp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${ASSIMP_INCLUDE_DIRS})

include_directories(${GLM_INCLUDE_DIRS})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.OpenGLProject)
endif()
set_target_properties(OpenGLProject PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS OpenGLProject
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(OpenGLProject)
endif()
