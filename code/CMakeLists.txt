cmake_minimum_required(VERSION 3.10)

project (Colline)

file(GLOB colline_source_files
        "include/*.h"
        "src/*.cpp"
)

# Specifies the Colline executable and the necessary source files
add_executable(Colline ${colline_source_files})

# Specifies the include directory for Colline
# PRIVATE - Means 'use only for this specified build target'
target_include_directories(Colline PRIVATE "include" )
