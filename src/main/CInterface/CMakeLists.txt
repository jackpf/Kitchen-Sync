# Main CMake file

cmake_minimum_required(VERSION 2.8.4)
project(KitchenSync)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -shared")

include_directories(
    /usr/local/include/
    /Library/Java/JavaVirtualMachines/jdk1.8.0_65.jdk/Contents/Home/include/darwin
    /Library/Java/JavaVirtualMachines/jdk1.8.0_65.jdk/Contents/Home/include/
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/")

add_executable(soundtouch.so /usr/local/lib/libSoundTouch.dylib CInterface.cpp)
#target_link_libraries(soundtouch.so)

# Install targets
install(TARGETS soundtouch.so DESTINATION /usr/local/lib/)