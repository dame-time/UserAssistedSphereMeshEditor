cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_COMPILER /usr/bin/clang++)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(SphereMeshEditor)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Find yaml-cpp package
find_package(yaml-cpp REQUIRED)

set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include")
set(OpenMP_CXX_LIB_NAMES "omp")
set(OpenMP_omp_LIBRARY "/usr/local/opt/libomp/lib/libomp.dylib")

find_package(OpenMP COMPONENTS CXX)

include_directories(Math Math/Vector/ Math/Versor/ Math/Rotation/ Math/Point/ Math/Matrix/ Shader Core include/imgui)

find_package(glfw3 3.3 REQUIRED)

set(GLAD_DIR include/)
add_library(GLAD "${GLAD_DIR}/glad.c")
include_directories("${GLAD_DIR}")

add_library(tinyfiledialogs STATIC Core/tinyfiledialogs.c)

file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

set(SEEN_FILES "")
set(DUPLICATE_FILES "")

foreach(source ${SOURCES})
    # Check if the file has already been seen
    if(source IN_LIST SEEN_FILES)
        list(APPEND DUPLICATE_FILES ${source})
    else()
        list(APPEND SEEN_FILES ${source})
    endif()
endforeach()

if(DUPLICATE_FILES)
    message(FATAL_ERROR "Duplicate files found:")
    foreach(duplicate ${DUPLICATE_FILES})
        message(FATAL_ERROR "${duplicate}")
    endforeach()
endif()

foreach(source ${SOURCES})
    if(NOT (${source} MATCHES "/Math/" OR ${source} MATCHES "/Shader/" OR ${source} MATCHES "/Core/" OR ${source} MATCHES "/imgui/"))
        list(REMOVE_ITEM SOURCES ${source})
    endif()
endforeach()

foreach(source ${SOURCES})
    message(STATUS "Source file: ${source}")
endforeach()

link_directories(/usr/local/lib)

add_executable(${PROJECT_NAME} ${SOURCES} main.cpp)
target_compile_options(${PROJECT_NAME} PUBLIC -g -O3 -march=native -flto -funroll-loops -std=c++17)
#target_include_directories(${PROJECT_NAME} PUBLIC ${includes})
target_link_libraries(${PROJECT_NAME} glfw GLAD ${CMAKE_DL_LIBS} yaml-cpp tinyfiledialogs OpenMP::OpenMP_CXX)
