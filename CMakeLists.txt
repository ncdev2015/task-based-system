cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(wzh-parsersec
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set directories
set(PS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/source)

####################################################################################################
# CPM as cmake package manager
####################################################################################################
set(CPM_DOWNLOAD_VERSION 0.40.8)

if(CPM_SOURCE_CACHE)
  set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif(DEFINED ENV{CPM_SOURCE_CACHE})
  set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else()
  set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif()

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
  message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
  file(DOWNLOAD
       https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
       ${CPM_DOWNLOAD_LOCATION}
  )
endif()

include(${CPM_DOWNLOAD_LOCATION})

# Download dependencies
CPMAddPackage(
  NAME fmt
  GITHUB_REPOSITORY fmtlib/fmt
  GIT_TAG "8.1.1"
  OPTIONS "FMT_HEADER_ONLY ON"
)

####################################################################################################

# add subdirectories
add_subdirectory(${PS_SOURCE_DIR}/parsec)

# add executable
add_executable(main ${PS_SOURCE_DIR}/main.cpp)
target_link_libraries(main fmt::fmt parsec)

# Link filesystem library for C++17
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(main stdc++fs)
endif()

# Copy task files
file(COPY ${CMAKE_SOURCE_DIR}/tasks DESTINATION ${CMAKE_BINARY_DIR})