# Main CMake file

cmake_minimum_required(VERSION 2.8.4)
project(KitchenSync)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -shared")

include_directories(
    /usr/local/include/
    /Library/Java/JavaVirtualMachines/jdk1.8.0_65.jdk/Contents/Home/include/
    /Library/Java/JavaVirtualMachines/jdk1.8.0_65.jdk/Contents/Home/include/darwin/
    "${PROJECT_SOURCE_DIR}/include/"
)
link_directories(
    /usr/local/lib/
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}")

add_executable(libkitchensync.dylib CInterface.cpp include/WavFile.cpp)
target_link_libraries(libkitchensync.dylib SoundTouch)

# Install targets
install(TARGETS libkitchensync.dylib DESTINATION /usr/local/lib/)