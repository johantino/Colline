set_property(DIRECTORY PROPERTY CXX_EXTENSIONS OFF)
set_property(DIRECTORY PROPERTY CMAKE_CXX_STANDARD 17)
set_property(DIRECTORY PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRCS *.cpp) # TODO remove GLOB
file(GLOB HDRS *.h) # TODO remove GLOB

add_library(${PROJECT_NAME}Lib ${SRCS} ${HDRS})
target_include_directories(${PROJECT_NAME}Lib PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} WIN32 ${SRCS} ${HDRS}) # WIN32 is needed for MFC
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
