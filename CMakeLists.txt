cmake_minimum_required(VERSION 3.26)
project(smol-game LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(smol-engine)

set(GAME_SOURCES
    src/main.cpp
    src/test_component.cpp
)

add_executable(${PROJECT_NAME} ${GAME_SOURCES})

if(WIN32)
    set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE $<$<CONFIG:Release>:ON>
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE smol-engine)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)