cmake_minimum_required(VERSION 3.10)
project(DroneTrackerApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find GStreamer
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0)

# 2. Explicitly define DeepStream and CUDA paths
set(DEEPSTREAM_DIR "/opt/nvidia/deepstream/deepstream")
set(CUDA_DIR "/usr/local/cuda")

# 3. CRITICAL: Add DeepStream include directory explicitly
include_directories(
    ${GST_INCLUDE_DIRS}
    /opt/nvidia/deepstream/deepstream/sources/includes  # <-- Đường dẫn tuyệt đối chứa nvds_meta.h
    ${CUDA_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src                     # Để include "probe.h" dễ dàng
)

# 4. Link directories
link_directories(
    ${GST_LIBRARY_DIRS}
    ${DEEPSTREAM_DIR}/lib
    ${CUDA_DIR}/lib64
)

# 5. Add all source files
add_executable(drone_tracker
    src/main.cpp
    src/pipeline.cpp
    src/source.cpp
    src/inference.cpp
    src/tracker.cpp
    src/osd.cpp
    src/sink.cpp
    src/probe.cpp
    src/config_parser.cpp
)

# 6. Link libraries
target_link_libraries(drone_tracker
    ${GST_LIBRARIES}
    nvds_meta           # library include nvds_meta.h
    nvds_yml_parser
    cuda
    cudart
    nvdsgst_helper
    nvdsgst_meta
)