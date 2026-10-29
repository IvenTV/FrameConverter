cmake_minimum_required(VERSION 3.16)
project(FrameConverter C CXX)

# --- SOFA ---
file(GLOB_RECURSE SOFA_SOURCES "sofa_src/src/*.c")
add_library(sofa STATIC ${SOFA_SOURCES})
target_include_directories(sofa PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/sofa_src/src)

# --- Eigen ---
find_package(Eigen3 REQUIRED)

add_executable(frame_converter
    main.cpp
    Time.cpp
    TimeConverter.cpp
    EopData.cpp
    Frame.cpp
)

target_link_libraries(frame_converter PRIVATE sofa Eigen3::Eigen)