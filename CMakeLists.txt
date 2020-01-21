cmake_minimum_required(VERSION 3.13)

# create your C++ project
project(FES VERSION 0.1.0 LANGUAGES CXX)

# find MEL::MEL and all available MEL::xxx modules
find_package(MEL REQUIRED)

# add your include directories
include_directories("include")

# create an app from your source files
add_executable(test_stim
    "include/Stimulator.hpp"
    "src/Stimulator.cpp"
    "examples/test_stim.cpp"
)

# link your app to MEL libaries
target_link_libraries(my_app 
    MEL::MEL 
)
