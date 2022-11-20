cmake_minimum_required(VERSION 3.10)
project(OpenCV_Video)

set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -ggdb -std=c++17 -Wall -Wno-deprecated -Werror -Wno-unused-function")
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -ggdb -std=c++17")

list(APPEND CMAKE_PREFIX_PATH "/home/cll/zoe/opencv-4.x")
find_package(OpenCV REQUIRED)

include_directories(/home/cll/zoe/opencv-4.x/include/opencv4)
include_directories(./dependency)
include_directories(./dependency/Log)
include_directories(./dependency/Thread)

set(SRC_FILE
    src/main.cpp
    src/Rasterizer.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/Triangle.cpp
    src/ScanLineZbuffer.cpp
    src/Zbuffer.cpp
    dependency/Log/LogAppender/logAppender.cpp
    dependency/Log/LogEvent/logEvent.cpp
    dependency/Log/LogFormatter/logFormatter.cpp
    dependency/Log/Logger/logger.cpp
    dependency/Log/LoggerManager/loggerManager.cpp
    dependency/Thread/Mutex/mutex.cpp
    dependency/Thread/Semaphore/sem.cpp
    dependency/Thread/thread.cpp
    dependency/utils/getInfo.cpp
)

set(TEST_FILE
    src/test.cpp
    src/Rasterizer.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/Triangle.cpp
    src/ScanLineZbuffer.cpp
    src/Zbuffer.cpp
    dependency/Log/LogAppender/logAppender.cpp
    dependency/Log/LogEvent/logEvent.cpp
    dependency/Log/LogFormatter/logFormatter.cpp
    dependency/Log/Logger/logger.cpp
    dependency/Log/LoggerManager/loggerManager.cpp
    dependency/Thread/Mutex/mutex.cpp
    dependency/Thread/Semaphore/sem.cpp
    dependency/Thread/thread.cpp
    dependency/utils/getInfo.cpp
)

add_executable(main ${SRC_FILE})
target_link_libraries(main ${OpenCV_LIBS} pthread)

add_executable(test ${TEST_FILE})
target_link_libraries(test ${OpenCV_LIBS} pthread)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)