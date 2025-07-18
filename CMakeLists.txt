cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(wzh-assesment
    VERSION 0.1
    LANGUAGES CXX
    DESCRIPTION "Task-based user management system"
)

# Set C++ standard and warnings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Warning configuration
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    
    # Disable specific warnings we need to suppress
    add_compile_options(
        -Wno-sign-compare                # For parsec's signed/unsigned comparisons
        -Wno-unused-parameter           # For unused cmd parameters in executors
    )
endif()

# Set directories
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/source)
set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third_party)

# Include directories
include_directories(
    ${INCLUDE_DIR}
    ${THIRD_PARTY_DIR}
)

##############################################################################
# Dependencies
##############################################################################

# CPM package manager
set(CPM_DOWNLOAD_VERSION 0.38.7)
if(NOT EXISTS ${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake)
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
         ${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake)
endif()
include(${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake)

# fmt library
CPMAddPackage(
    NAME fmt
    GITHUB_REPOSITORY fmtlib/fmt
    GIT_TAG 10.1.1
    OPTIONS "FMT_INSTALL ON"
)

# parsec library with specific warning suppression
add_library(parsec_interface INTERFACE)
target_include_directories(parsec_interface INTERFACE ${THIRD_PARTY_DIR}/parsec/interface)

# Create alias to maintain compatibility
add_library(parsec ALIAS parsec_interface)

##############################################################################
# Main executable
##############################################################################

file(GLOB_RECURSE SOURCES
    "${SOURCE_DIR}/*.cpp"
    "${CMAKE_SOURCE_DIR}/main.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_DIR}
    ${THIRD_PARTY_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    fmt::fmt
    parsec_interface
    stdc++fs
)

file(COPY ${CMAKE_SOURCE_DIR}/tasks DESTINATION ${CMAKE_BINARY_DIR})

##############################################################################
# Installation
##############################################################################

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY ${INCLUDE_DIR}/ DESTINATION include)

##############################################################################
# Testing
##############################################################################

option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()