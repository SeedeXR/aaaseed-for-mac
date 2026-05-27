# src/gol/metal/CMakeLists.txt
#
# `aaaseed_gol_metal` — the Mac Metal backend for GOL. Pure C++ via Apple's
# metal-cpp headers (vendored in third_party/metal-cpp/). This is the
# canonical TU for the metal-cpp PRIVATE_IMPLEMENTATION symbols ; no other
# TU in the Mac build should define them, otherwise we get linker
# duplicates.

find_library(METAL_FRAMEWORK     Metal     REQUIRED)
find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)

add_library(aaaseed_gol_metal STATIC
    metal_backend.cpp
)

target_include_directories(aaaseed_gol_metal PUBLIC
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/third_party/metal-cpp"
)

target_link_libraries(aaaseed_gol_metal PUBLIC
    "${METAL_FRAMEWORK}"
    "${FOUNDATION_FRAMEWORK}"
    "${QUARTZCORE_FRAMEWORK}"
)

target_compile_features(aaaseed_gol_metal PUBLIC cxx_std_20)
set_target_properties(aaaseed_gol_metal PROPERTIES POSITION_INDEPENDENT_CODE ON)
