cmake_minimum_required (VERSION 3.0.2)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(PROJECT "TestProject")
project (${PROJECT})

add_executable(${PROJECT} WIN32
    TestProject/Application.cpp
    TestProject/Application.h
)

if(WIN32)
   set_target_properties(${PROJECT} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
endif(WIN32)

target_link_libraries(${PROJECT} ${CMAKE_BINARY_DIR}/Release/Flux.lib)
target_link_libraries(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Libraries/assimp.lib)
target_link_libraries(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Libraries/glfw3.lib)
target_link_libraries(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Libraries/nvToolsExt64_1.lib)

set(EDITOR "Editor")
project (${EDITOR})

add_subdirectory(Editor)

include_directories(${CMAKE_SOURCE_DIR}/Engine/Includes/)

add_executable(${EDITOR} WIN32
    ${EDITOR_HEADERS}
    ${EDITOR_SOURCES}
)

if(WIN32)
   set_target_properties(${EDITOR} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
endif(WIN32)

target_link_libraries(${EDITOR} ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Libraries/assimp.lib)

set(ENGINE "Flux")
project (${ENGINE})

set (${ENGINE}_VERSION_MAJOR 0)
set (${ENGINE}_VERSION_MINOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/${ENGINE}Config.h.in"
  "${PROJECT_BINARY_DIR}/${ENGINE}Config.h"
)

add_subdirectory(Engine/Source)

include_directories("${PROJECT_BINARY_DIR}")
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/Engine/Source)
include_directories(${CMAKE_SOURCE_DIR}/Engine/Includes/)

add_library(${ENGINE} STATIC
    ${BASE}
    ${SCRIPTS}
    ${COMPONENTS}
    ${UTIL}
    ${EXCEPTIONS}
    ${INPUT}
    ${RENDERING}
    ${LOADERS}
)

source_group( Engine FILES ${BASE})
source_group( Engine\\Exceptions FILES ${EXCEPTIONS})
source_group( Engine\\Input FILES ${INPUT})
source_group( Engine\\Scripts FILES ${SCRIPTS})
source_group( Engine\\Components FILES ${COMPONENTS})
source_group( Engine\\Util FILES ${UTIL})
source_group( Engine\\Loaders FILES ${LOADERS})
source_group( Engine\\Rendering FILES ${RENDERING})

target_link_libraries(${ENGINE} ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Libraries/assimp.lib)
target_link_libraries(${ENGINE} ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Libraries/glfw3.lib)
target_link_libraries(${ENGINE} ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Libraries/nvToolsExt64_1.lib)

add_custom_command(TARGET ${ENGINE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/Engine/Binaries/"
        ${CMAKE_BINARY_DIR}/Release)

add_custom_command(TARGET ${ENGINE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/Engine/Shaders/"
        ${CMAKE_SOURCE_DIR}/build/res/Shaders)
