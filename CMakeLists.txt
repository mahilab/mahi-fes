cmake_minimum_required(VERSION 3.13)

# create your C++ project
project(fes VERSION 0.1.0 LANGUAGES CXX)

# find MEL::MEL and all available MEL::xxx modules
# find_package(MEL REQUIRED)

include(FetchContent)

# add_subdirectory("mahi-gui")
FetchContent_Declare(mahi-gui GIT_REPOSITORY https://github.com/mahilab/mahi-gui.git)
FetchContent_MakeAvailable(mahi-gui)

# FetchContent_Declare(mel 
#                     GIT_REPOSITORY https://github.com/mahilab/MEL.git 
#                     GIT_TAG origin/cmake_edits)
# FetchContent_MakeAvailable(mel)

FetchContent_Declare(mahi-util GIT_REPOSITORY https://github.com/mahilab/mahi-util.git)
FetchContent_MakeAvailable(mahi-util)

# add your include directories
include_directories("include")

# create an app from your source files
add_executable(test_stim
    "examples/test_stim.cpp"    
    "include/FES/Core/Stimulator.hpp"
    "src/FES/Core/Stimulator.cpp"
    "include/FES/Core/Channel.hpp"
    "src/FES/Core/Channel.cpp"
    "include/FES/Core/Event.hpp"
    "src/FES/Core/Event.cpp"
    "include/FES/Core/Scheduler.hpp"
    "src/FES/Core/Scheduler.cpp"
    "include/FES/Core/Message.hpp"
    "src/FES/Core/Message.cpp"
    "include/FES/Core/ReadMessage.hpp"
    "src/FES/Core/ReadMessage.cpp"
    "include/FES/Core/WriteMessage.hpp"
    "src/FES/Core/WriteMessage.cpp"
    "include/FES/Utility/Utility.hpp"
    "src/FES/Utility/Utility.cpp"
    "include/FES/Utility/Visualizer.hpp"
    "src/FES/Utility/Visualizer.cpp"
    "include/FES/Utility/VirtualStim.hpp"
    "src/FES/Utility/VirtualStim.cpp"
)
# link your app to MEL libaries
target_link_libraries(test_stim 
    mahi::gui
    mahi::util
)

# create an app from your source files
add_executable(ex_visualization
    "examples/ex_visualization.cpp"
    "include/FES/Core/Stimulator.hpp"
    "src/FES/Core/Stimulator.cpp"
    "include/FES/Core/Channel.hpp"
    "src/FES/Core/Channel.cpp"
    "include/FES/Core/Event.hpp"
    "src/FES/Core/Event.cpp"
    "include/FES/Core/Scheduler.hpp"
    "src/FES/Core/Scheduler.cpp"
    "include/FES/Core/Message.hpp"
    "src/FES/Core/Message.cpp"
    "include/FES/Core/ReadMessage.hpp"
    "src/FES/Core/ReadMessage.cpp"
    "include/FES/Core/WriteMessage.hpp"
    "src/FES/Core/WriteMessage.cpp"
    "include/FES/Utility/Utility.hpp"
    "src/FES/Utility/Utility.cpp"
    "include/FES/Utility/Visualizer.hpp"
    "src/FES/Utility/Visualizer.cpp"
)
# link your app to MEL libaries
target_link_libraries(ex_visualization 
    mahi::gui 
    mahi::util
)


add_executable(ex_virtual_stim
    "examples/ex_virtual_stim.cpp"
    "include/FES/Utility/VirtualStim.hpp"
    "src/FES/Utility/VirtualStim.cpp"
)

target_link_libraries(ex_virtual_stim 
    mahi::util
    mahi::gui
)
