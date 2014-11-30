cmake_minimum_required (VERSION 2.6)
project(interfaces-manager)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} -g")
if (NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type selected, default to Release")
	set(CMAKE_BUILD_TYPE "Release")
else (NOT CMAKE_BUILD_TYPE)
	message(STATUS "Build type set to: ${CMAKE_BUILD_TYPE}")
endif()

find_package(Threads REQUIRED)

include_directories("source")

add_subdirectory(source)
add_subdirectory(watcher)
