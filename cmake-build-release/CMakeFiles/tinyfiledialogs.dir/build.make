# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/davidepaollilo/Workspaces/C++/SphereMeshEditor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/cmake-build-release

# Include any dependencies generated for this target.
include CMakeFiles/tinyfiledialogs.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/tinyfiledialogs.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/tinyfiledialogs.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tinyfiledialogs.dir/flags.make

CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.o: CMakeFiles/tinyfiledialogs.dir/flags.make
CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.o: /Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/Core/tinyfiledialogs.c
CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.o: CMakeFiles/tinyfiledialogs.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.o -MF CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.o.d -o CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.o -c /Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/Core/tinyfiledialogs.c

CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/Core/tinyfiledialogs.c > CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.i

CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/Core/tinyfiledialogs.c -o CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.s

# Object files for target tinyfiledialogs
tinyfiledialogs_OBJECTS = \
"CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.o"

# External object files for target tinyfiledialogs
tinyfiledialogs_EXTERNAL_OBJECTS =

libtinyfiledialogs.a: CMakeFiles/tinyfiledialogs.dir/Core/tinyfiledialogs.c.o
libtinyfiledialogs.a: CMakeFiles/tinyfiledialogs.dir/build.make
libtinyfiledialogs.a: CMakeFiles/tinyfiledialogs.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libtinyfiledialogs.a"
	$(CMAKE_COMMAND) -P CMakeFiles/tinyfiledialogs.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tinyfiledialogs.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tinyfiledialogs.dir/build: libtinyfiledialogs.a
.PHONY : CMakeFiles/tinyfiledialogs.dir/build

CMakeFiles/tinyfiledialogs.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tinyfiledialogs.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tinyfiledialogs.dir/clean

CMakeFiles/tinyfiledialogs.dir/depend:
	cd /Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/cmake-build-release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/davidepaollilo/Workspaces/C++/SphereMeshEditor /Users/davidepaollilo/Workspaces/C++/SphereMeshEditor /Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/cmake-build-release /Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/cmake-build-release /Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/cmake-build-release/CMakeFiles/tinyfiledialogs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tinyfiledialogs.dir/depend

