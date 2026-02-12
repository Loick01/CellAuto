cmake_minimum_required(VERSION 3.10)

project(cellauto VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)

set(SOURCES
    src/main.cpp
    src/window.cpp
    src/event.cpp
    src/gui.cpp
)

file(GLOB IMGUI_SOURCES
    external/imgui/src/*.cpp
)

add_executable(cellauto ${SOURCES} ${IMGUI_SOURCES})

target_include_directories(cellauto PRIVATE 
    include 
    ${SDL2_INCLUDE_DIRS}
    external/imgui/include
)
    
target_link_libraries(cellauto PRIVATE ${SDL2_LIBRARIES})
