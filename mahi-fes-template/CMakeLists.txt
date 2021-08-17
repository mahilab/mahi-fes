cmake_minimum_required(VERSION 3.13)

# create your C++ project
project(MyProject VERSION 0.1.0 LANGUAGES CXX)

# get the relevant libraries to use from github. FetchContent is great because it pulls the most recent version of a library and will add it to your project.
include(FetchContent)
FetchContent_Declare(mahi-fes GIT_REPOSITORY https://github.com/mahilab/mahi-fes.git)
FetchContent_MakeAvailable(mahi-fes)

# add your include directories
# these should be where you put your *.hpp files
include_directories("include")

# create an app from your source files (*.cpp involved in the executable)
add_executable(my_app # executable name
    "src/my_app.cpp"  # file that has the main() function for the application
    "src/MyClass.cpp" # other cpp files that may be used in the main cpp file
)

# link your app to the mahi-fes libary
target_link_libraries(my_app mahi::fes)