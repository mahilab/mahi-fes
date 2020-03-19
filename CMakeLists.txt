# MIT License
#
# Copyright (c) 2020 Mechatronics and Haptic Interfaces Lab - Rice University
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# Author(s): Nathan Dunkelberger (nbd2@rice.edu)

cmake_minimum_required(VERSION 3.13.0)

#===============================================================================
# USER OPTIONS
#===============================================================================

# Options
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    option(MAHI_FES_EXAMPLES "Turn ON to build example executable(s)" ON)
else()
    option(MAHI_FES_EXAMPLES "Turn ON to build example executable(s)" OFF)
endif()

#===============================================================================
# FRONT MATTER
#===============================================================================

# create your C++ project
project(mahi-fes VERSION 0.1.0 LANGUAGES CXX)

# add ./cmake to CMake module path so our .cmake files can be found
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(GNUInstallDirs) # defines conventional GNU isntallation directories

# Enable IDE folders and set them for predefined CMake projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -O3") # ALL WARNINGS
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP")    # MULTICORE BUILDS
endif()

include(FetchContent)

#===============================================================================
# MAHI UTIL
#===============================================================================

FetchContent_Declare(mahi-util GIT_REPOSITORY https://github.com/mahilab/mahi-util.git)
FetchContent_MakeAvailable(mahi-util)

#===============================================================================
# MAHI GUI
#===============================================================================

FetchContent_Declare(mahi-gui GIT_REPOSITORY https://github.com/mahilab/mahi-gui.git)
FetchContent_MakeAvailable(mahi-gui)

#===============================================================================
# CREATE LIBRARY
#===============================================================================

add_library(fes "")
add_library(mahi::fes ALIAS fes)
set_target_properties(fes PROPERTIES DEBUG_POSTFIX -d)
target_compile_features(fes PUBLIC cxx_std_11)
install(TARGETS fes EXPORT mahi-fes-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
set_target_properties(fes PROPERTIES OUTPUT_NAME "mahi-fes")
target_link_libraries(fes mahi::util mahi::gui)

# defines
target_compile_definitions(fes PUBLIC MAHI_FES) # for compatibility checks

# add source files
add_subdirectory(src/Mahi/Fes)

# add include files
file(GLOB_RECURSE MAHI_FES_HEADERS "include/*.hpp" "include/*.h" "include/*.inl")
target_sources(fes PRIVATE ${MAHI_FES_HEADERS}) # for intellisense
target_include_directories(fes
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# add your include directories
include_directories("include")

#===============================================================================
# WINDOWS ONLY
#===============================================================================

if(WIN32)
target_compile_definitions(fes
    PUBLIC
        -D_CRT_SECURE_NO_WARNINGS          # remove secure warnings (e.g sprinft_s)
        -DNOMINMAX                         # remove min/max macros
        -D_WINSOCK_DEPRECATED_NO_WARNINGS  # remove winsock deprecated warnings
) 
endif(WIN32)

#===============================================================================
# EXAMPLE EXECUTABLES
#===============================================================================

if(MAHI_FES_EXAMPLES)
    message("Building mahi::fes examples")
    add_subdirectory(examples)
endif()

#===============================================================================
# INSTALL
#===============================================================================

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(MAHI_INSTALL_POSTFIX "") # 64 bit Windows is default
else()
    set(MAHI_INSTALL_POSTFIX "-x86")
endif()

# add the postfix to the standard install prefix, eg. "Program Files/mahi-fes-x86" or "Program Files/mahi-fes"
set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}${MAHI_INSTALL_POSTFIX}")

# install headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# set where we want to install our config
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/mahi-fes)

# export the targets to a script
install(EXPORT mahi-fes-targets
	FILE
		mahi-fes-targets.cmake
	NAMESPACE
		mahi::
	DESTINATION
		${INSTALL_CONFIGDIR}
)

# include helper functions for creating config files that can be included by other projects to find and use a package
include(CMakePackageConfigHelpers)

# generate a package configuration file and a package version file
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/mahi-fes-config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/mahi-fes-config.cmake
	INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/mahi-fes-config-version.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY AnyNewerVersion
)

# install the config and configversion modules
install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/mahi-fes-config.cmake
	${CMAKE_CURRENT_BINARY_DIR}/mahi-fes-config-version.cmake
	DESTINATION ${INSTALL_CONFIGDIR}
)

# export from the build tree
export(EXPORT mahi-fes-targets
	   NAMESPACE mahi::
       FILE ${CMAKE_CURRENT_BINARY_DIR}/mahi-fes-targets.cmake
)



# # create an app from your source files
# add_executable(test_stim
#     "examples/test_stim.cpp"    
#     "include/Mahi/Fes/Core/Stimulator.hpp"
#     "src/Mahi/Fes/Core/Stimulator.cpp"
#     "include/Mahi/Fes/Core/Channel.hpp"
#     "src/Mahi/Fes/Core/Channel.cpp"
#     "include/Mahi/Fes/Core/Event.hpp"
#     "src/Mahi/Fes/Core/Event.cpp"
#     "include/Mahi/Fes/Core/Scheduler.hpp"
#     "src/Mahi/Fes/Core/Scheduler.cpp"
#     "include/Mahi/Fes/Core/Message.hpp"
#     "src/Mahi/Fes/Core/Message.cpp"
#     "include/Mahi/Fes/Core/ReadMessage.hpp"
#     "src/Mahi/Fes/Core/ReadMessage.cpp"
#     "include/Mahi/Fes/Core/WriteMessage.hpp"
#     "src/Mahi/Fes/Core/WriteMessage.cpp"
#     "include/Mahi/Fes/Utility/Utility.hpp"
#     "src/Mahi/Fes/Utility/Utility.cpp"
#     "include/Mahi/Fes/Utility/Visualizer.hpp"
#     "src/Mahi/Fes/Utility/Visualizer.cpp"
#     "include/Mahi/Fes/Utility/VirtualStim.hpp"
#     "src/Mahi/Fes/Utility/VirtualStim.cpp"
# )
# # link your app to MEL libaries
# target_link_libraries(test_stim 
#     mahi::gui
#     mahi::util
# )

# # create an app from your source files
# add_executable(ex_visualization
#     "examples/ex_visualization.cpp"
#     "include/Mahi/Fes/Core/Stimulator.hpp"
#     "src/Mahi/Fes/Core/Stimulator.cpp"
#     "include/Mahi/Fes/Core/Channel.hpp"
#     "src/Mahi/Fes/Core/Channel.cpp"
#     "include/Mahi/Fes/Core/Event.hpp"
#     "src/Mahi/Fes/Core/Event.cpp"
#     "include/Mahi/Fes/Core/Scheduler.hpp"
#     "src/Mahi/Fes/Core/Scheduler.cpp"
#     "include/Mahi/Fes/Core/Message.hpp"
#     "src/Mahi/Fes/Core/Message.cpp"
#     "include/Mahi/Fes/Core/ReadMessage.hpp"
#     "src/Mahi/Fes/Core/ReadMessage.cpp"
#     "include/Mahi/Fes/Core/WriteMessage.hpp"
#     "src/Mahi/Fes/Core/WriteMessage.cpp"
#     "include/Mahi/Fes/Utility/Utility.hpp"
#     "src/Mahi/Fes/Utility/Utility.cpp"
#     "include/Mahi/Fes/Utility/Visualizer.hpp"
#     "src/Mahi/Fes/Utility/Visualizer.cpp"
# )
# # link your app to MEL libaries
# target_link_libraries(ex_visualization 
#     mahi::gui 
#     mahi::util
# )


# add_executable(ex_virtual_stim
#     "examples/ex_virtual_stim.cpp"
#     "include/Mahi/Fes/Utility/VirtualStim.hpp"
#     "src/Mahi/Fes/Utility/VirtualStim.cpp"
# )

# target_link_libraries(ex_virtual_stim 
#     mahi::util
#     mahi::gui
# )
