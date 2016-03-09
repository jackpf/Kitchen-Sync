# Main CMake file

cmake_minimum_required(VERSION 2.8.4)
project(KitchenSync)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -shared")
add_definitions("-Wall")

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

file(GLOB includes
    "include/*.cpp"
)

add_executable(libkitchensync.dylib CInterface.cpp KitchenSync.cpp ${includes})
target_link_libraries(libkitchensync.dylib soundtouch fftw3 m flac flac++ mp3lame aiff)

# Install targets
install(TARGETS libkitchensync.dylib DESTINATION /usr/local/lib/)