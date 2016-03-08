# Main CMake file

cmake_minimum_required(VERSION 2.8.4)
project(KitchenSyncTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
add_definitions("-Wall -g")

include_directories(
    /usr/local/include/
    /Library/Java/JavaVirtualMachines/jdk1.8.0_65.jdk/Contents/Home/include/
    /Library/Java/JavaVirtualMachines/jdk1.8.0_65.jdk/Contents/Home/include/darwin/
    "${PROJECT_SOURCE_DIR}/../"
    "${PROJECT_SOURCE_DIR}/../include/"
)
link_directories(
    /usr/local/lib/
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

file(GLOB includes
    "../include/*.cpp"
)
set(libraries soundtouch fftw3 m flac flac++ aiff)

add_executable(kstest Test.cpp ../KitchenSync.cpp ${includes})
target_link_libraries(kstest ${libraries})

add_executable(flactest FLACTest.cpp ${includes})
target_link_libraries(flactest ${libraries})

add_executable(compare Compare.cpp ${includes})
target_link_libraries(compare ${libraries})
