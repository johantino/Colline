cmake_minimum_required(VERSION 3.1...3.13)

if(${CMAKE_VERSION} VERSION_LESS 3.13)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.13)
endif()

project(Colline VERSION 1.0 DESCRIPTION "an open source AI system" LANGUAGES CXX)

if(MSVC)
    add_definitions("/W4 /D_CRT_SECURE_NO_WARNINGS /permissive-")

	# use MFC for the moment
	add_definitions(-D_AFXDLL)
	set(CMAKE_MFC_FLAG 1) # use static MFC lib
else()
    add_definitions("-Wall -Wextra -fpermissive")
endif()

find_package(Catch2 CONFIG REQUIRED)

enable_testing()
include(CTest)

add_subdirectory(src)
add_subdirectory(test)
