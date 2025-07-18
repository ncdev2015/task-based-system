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
# Core Library (shared code for main app and tests)
##############################################################################

# Find all source files except main.cpp
file(GLOB_RECURSE LIB_SOURCES
    "${SOURCE_DIR}/*.cpp"
)

# Remove main.cpp from library sources
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# Create core library
add_library(${PROJECT_NAME}-core STATIC ${LIB_SOURCES})

target_include_directories(${PROJECT_NAME}-core PUBLIC
    ${INCLUDE_DIR}
    ${THIRD_PARTY_DIR}
)

target_link_libraries(${PROJECT_NAME}-core PUBLIC
    fmt::fmt
    parsec_interface
    stdc++fs
)

# Set library properties
set_target_properties(${PROJECT_NAME}-core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

##############################################################################
# Main executable
##############################################################################

# Create main executable with just main.cpp
add_executable(${PROJECT_NAME} ${SOURCE_DIR}/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_DIR}
    ${THIRD_PARTY_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${PROJECT_NAME}-core
)

# Copy tasks directory to build directory
file(COPY ${CMAKE_SOURCE_DIR}/tasks DESTINATION ${CMAKE_BINARY_DIR})

##############################################################################
# Installation
##############################################################################

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY ${INCLUDE_DIR}/ DESTINATION include)

##############################################################################
# Testing
##############################################################################

option(BUILD_TESTING "Build tests" ON)  # Enabled by default for development

if(BUILD_TESTING)
    enable_testing()
    
    # Add Catch2 testing framework
    CPMAddPackage(
        NAME Catch2
        GITHUB_REPOSITORY catchorg/Catch2
        GIT_TAG v3.4.0
        OPTIONS 
            "CATCH_INSTALL_DOCS OFF" 
            "CATCH_INSTALL_EXTRAS OFF"
            "CATCH_DEVELOPMENT_BUILD OFF"
    )
    
    # Find all test source files
    file(GLOB_RECURSE TEST_SOURCES
        "${CMAKE_SOURCE_DIR}/tests/*.cpp"
    )
    
    # Only create test executable if test sources exist
    if(TEST_SOURCES)
        # Create test executable
        add_executable(${PROJECT_NAME}-tests ${TEST_SOURCES})
        
        # Configure test dependencies
        target_include_directories(${PROJECT_NAME}-tests PRIVATE
            ${INCLUDE_DIR}
            ${THIRD_PARTY_DIR}
        )
        
        target_link_libraries(${PROJECT_NAME}-tests PRIVATE
            ${PROJECT_NAME}-core  # Link with core library instead of executable
            Catch2::Catch2WithMain
        )
        
        # Set test properties
        set_target_properties(${PROJECT_NAME}-tests PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
        )
        
        # Add test to CTest
        add_test(
            NAME ${PROJECT_NAME}-tests
            COMMAND ${PROJECT_NAME}-tests
        )
        
        # Enable colored output if supported
        if(NOT MSVC)
            target_compile_options(${PROJECT_NAME}-tests PRIVATE -fdiagnostics-color=always)
        endif()
        
        message(STATUS "Tests configured. Found ${CMAKE_MATCH_COUNT} test files.")
    else()
        message(STATUS "No test files found in tests/ directory. Skipping test configuration.")
    endif()
endif()