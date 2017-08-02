cmake_minimum_required (VERSION 3.0.2)

set(ENGINE "Flux")

project (${ENGINE})

set (${ENGINE}_VERSION_MAJOR 1)
set (${ENGINE}_VERSION_MINOR 0)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

configure_file (
  "${PROJECT_SOURCE_DIR}/${ENGINE}Config.h.in"
  "${PROJECT_BINARY_DIR}/${ENGINE}Config.h"
)

add_subdirectory(Editor)
add_subdirectory(Engine/Source)

include_directories("${PROJECT_BINARY_DIR}")
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/Engine/Includes/)
include_directories ("C:/Includes/")

add_executable(${ENGINE} WIN32
    ${EDITOR_HEADERS}
    ${EDITOR_SOURCES}
    ${ENGINE_HEADERS}
    ${ENGINE_SOURCES}
    ${EXCEPTION_HEADERS}
    ${INPUT_HEADERS}
    ${INPUT_SOURCES}
    ${RENDERING_HEADERS}
    ${RENDERING_SOURCES}
)

if(WIN32)
   set_target_properties(${ENGINE} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
endif(WIN32)

source_group( Editor FILES ${EDITOR_SOURCES} ${EDITOR_HEADERS})
source_group( Engine FILES ${ENGINE_SOURCES} ${ENGINE_HEADERS})
source_group( Engine\\Exceptions FILES ${EXCEPTION_HEADERS})
source_group( Engine\\Input FILES ${INPUT_SOURCES} ${INPUT_HEADERS})
source_group( Engine\\Rendering FILES ${RENDERING_SOURCES} ${RENDERING_HEADERS})

target_link_libraries(${ENGINE} ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Libraries/assimp.lib)
target_link_libraries(${ENGINE} ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Libraries/glfw3.lib)
target_link_libraries(${ENGINE} ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Libraries/nvToolsExt64_1.lib)

add_custom_command(TARGET ${ENGINE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/Engine/Binaries/"
        ${CMAKE_BINARY_DIR}/Release)
