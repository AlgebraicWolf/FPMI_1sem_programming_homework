# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\Program Files\JetBrains\CLion 2019.2.1\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "D:\Program Files\JetBrains\CLion 2019.2.1\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = D:\1sem_programming_homework\MIPT_1sem_programming_homework\countCircles

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = D:\1sem_programming_homework\MIPT_1sem_programming_homework\countCircles\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/countCircles.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/countCircles.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/countCircles.dir/flags.make

CMakeFiles/countCircles.dir/main.cpp.obj: CMakeFiles/countCircles.dir/flags.make
CMakeFiles/countCircles.dir/main.cpp.obj: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=D:\1sem_programming_homework\MIPT_1sem_programming_homework\countCircles\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/countCircles.dir/main.cpp.obj"
	"D:\Program Files\mingw-w64\x86_64-8.1.0-win32-seh-rt_v6-rev0\mingw64\bin\g++.exe"  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\countCircles.dir\main.cpp.obj -c D:\1sem_programming_homework\MIPT_1sem_programming_homework\countCircles\main.cpp

CMakeFiles/countCircles.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/countCircles.dir/main.cpp.i"
	"D:\Program Files\mingw-w64\x86_64-8.1.0-win32-seh-rt_v6-rev0\mingw64\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E D:\1sem_programming_homework\MIPT_1sem_programming_homework\countCircles\main.cpp > CMakeFiles\countCircles.dir\main.cpp.i

CMakeFiles/countCircles.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/countCircles.dir/main.cpp.s"
	"D:\Program Files\mingw-w64\x86_64-8.1.0-win32-seh-rt_v6-rev0\mingw64\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S D:\1sem_programming_homework\MIPT_1sem_programming_homework\countCircles\main.cpp -o CMakeFiles\countCircles.dir\main.cpp.s

# Object files for target countCircles
countCircles_OBJECTS = \
"CMakeFiles/countCircles.dir/main.cpp.obj"

# External object files for target countCircles
countCircles_EXTERNAL_OBJECTS =

countCircles.exe: CMakeFiles/countCircles.dir/main.cpp.obj
countCircles.exe: CMakeFiles/countCircles.dir/build.make
countCircles.exe: CMakeFiles/countCircles.dir/linklibs.rsp
countCircles.exe: CMakeFiles/countCircles.dir/objects1.rsp
countCircles.exe: CMakeFiles/countCircles.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=D:\1sem_programming_homework\MIPT_1sem_programming_homework\countCircles\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable countCircles.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\countCircles.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/countCircles.dir/build: countCircles.exe

.PHONY : CMakeFiles/countCircles.dir/build

CMakeFiles/countCircles.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\countCircles.dir\cmake_clean.cmake
.PHONY : CMakeFiles/countCircles.dir/clean

CMakeFiles/countCircles.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" D:\1sem_programming_homework\MIPT_1sem_programming_homework\countCircles D:\1sem_programming_homework\MIPT_1sem_programming_homework\countCircles D:\1sem_programming_homework\MIPT_1sem_programming_homework\countCircles\cmake-build-debug D:\1sem_programming_homework\MIPT_1sem_programming_homework\countCircles\cmake-build-debug D:\1sem_programming_homework\MIPT_1sem_programming_homework\countCircles\cmake-build-debug\CMakeFiles\countCircles.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/countCircles.dir/depend

