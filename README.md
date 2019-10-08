# Colline
Colline is an open source AI system. The code was originally developed in 2001-2003 has not been touched since then.

The last update in the code has been done on Oct 6th 2019, this changes along with the documentation (see folder) should be enough to get us started.

Once we get this thing going we can use the subreddit r/Colline to share progress and also discuss the implications for our current worldview in order to mature this project. 

Strong AI should work hand in hand with an ethical mindset, also should be open source and not placed in the hands of some privately owned company.

Looking forward to follow the progress!

Johan Tino Frederiksen
(founder of this project & programmer of the original code)


## Build Instructions

Colline requires a C++ compiler conforming to C++17. It has been tested with
- Visual Studio 2019

### Windows

Using vcpkg to install dependencies is recommended.

    1. Clone or download the repository

    2. Install dependencies: `vcpkg install catch2`

    3. Navigate into the terminal to /Colline/code/

    4. `md build && cd build`

    5. `cmake ../ -DCMAKE_TOOLCHAIN_FILE=\<path to vcpkg\>/scripts/buildsystems/vcpkg.cmake`

    6. Open in Visual Studio as CMake project.

### Linux

    1. Clone or download the repository

    2. Navigate into the terminal to /Colline/code/

    3. `mkdir build`

    4. `cd build`

    5. `cmake ..`

    6. `make`