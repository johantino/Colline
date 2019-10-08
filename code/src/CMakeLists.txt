
file(GLOB SRCS *.cpp) # TODO remove GLOB
file(GLOB HDRS *.h) # TODO remove GLOB

add_executable(${PROJECT_NAME} WIN32 ${SRCS} ${HDRS}) # WIN32 is needed for MFC

# PRIVATE - Means 'use only for this specified build target'
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)
