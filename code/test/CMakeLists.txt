set_property(DIRECTORY PROPERTY CXX_EXTENSIONS OFF)
set_property(DIRECTORY PROPERTY CMAKE_CXX_STANDARD 17)
set_property(DIRECTORY PROPERTY CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}Test
        "main.cpp"
		"GridTest.cpp"
)

target_include_directories(${PROJECT_NAME}Test PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME}Test PRIVATE ${PROJECT_NAME}Lib Catch2::Catch2)

add_test(NAME ${PROJECT_NAME}Test COMMAND ${PROJECT_NAME}Test)
