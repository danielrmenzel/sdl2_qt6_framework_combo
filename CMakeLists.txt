cmake_minimum_required(VERSION 3.5)

project(test3 VERSION 0.1 LANGUAGES CXX)

set(PROJECT_ROOT_PATH "${CMAKE_SOURCE_DIR}/")

#configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt packages
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        SDLApp.cpp
        SDLApp.h
)

# Find and include SDL2 on both Linux and Windows
if(WIN32)
    set(SDL2_INCLUDE_DIRS "C:/Users/danie/OneDrive/Desktop/bachelor/local_project_files/SDL2-2.30.2_test/include")
    set(SDL2_LIBRARIES "C:/Users/danie/OneDrive/Desktop/bachelor/local_project_files/SDL2-2.30.2_test/lib/x64/SDL2.lib")

    set(SDL2IMAGE_INCLUDE_DIRS "C:/Users/danie/OneDrive/Desktop/bachelor/local_project_files/SDL2_image-2.8.2/include")
    set(SDL2IMAGE_LIBRARIES "C:/Users/danie/OneDrive/Desktop/bachelor/local_project_files/SDL2_image-2.8.2/lib/x64/SDL2_image.lib")

    set(SDL2TTF_INCLUDE_DIRS "C:/Users/danie/OneDrive/Desktop/bachelor/local_project_files/SDL2_ttf-2.22.0/include")
    set(SDL2TTF_LIBRARIES "C:/Users/danie/OneDrive/Desktop/bachelor/local_project_files/SDL2_ttf-2.22.0/lib/x64/SDL2_ttf.lib")

    #set(PROJECT_ROOT_PATH "C:/path/to/your/project/root/")
else() # Assuming Linux
    set(SDL2_INCLUDE_DIRS "/usr/include/SDL2")
    set(SDL2_LIBRARIES "-lSDL2")

    set(SDL2IMAGE_INCLUDE_DIRS "/usr/include/SDL2")
    set(SDL2IMAGE_LIBRARIES "-lSDL2_image")

    set(SDL2TTF_INCLUDE_DIRS "/usr/include/SDL2")
    set(SDL2TTF_LIBRARIES "-lSDL2_ttf")
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(test3
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        SDLApp.h
        SDLApp.cpp
    )
else()
    if(ANDROID)
        add_library(test3 SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(test3
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Include directories for your project
target_include_directories(test3 PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2IMAGE_INCLUDE_DIRS}
    ${SDL2TTF_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include # If you have additional include directories
)

# Link libraries
target_link_libraries(test3 PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${SDL2_LIBRARIES}
    ${SDL2IMAGE_LIBRARIES}
    ${SDL2TTF_LIBRARIES}
)

if(WIN32)
    # Hide the console window
    set_target_properties(test3 PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(test3)
endif()
