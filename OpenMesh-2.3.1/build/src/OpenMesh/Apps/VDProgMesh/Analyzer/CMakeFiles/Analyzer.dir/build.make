# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = "/Applications/CMake 2.8-11.app/Contents/bin/cmake"

# The command to remove a file.
RM = "/Applications/CMake 2.8-11.app/Contents/bin/cmake" -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = "/Applications/CMake 2.8-11.app/Contents/bin/ccmake"

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/build

# Include any dependencies generated for this target.
include src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/depend.make

# Include the progress variables for this target.
include src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/progress.make

# Include the compile flags for this target's objects.
include src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/flags.make

src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o: src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/flags.make
src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o: ../src/OpenMesh/Apps/VDProgMesh/Analyzer/vdpmanalyzer.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o"
	cd /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/build/src/OpenMesh/Apps/VDProgMesh/Analyzer && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o -c /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/src/OpenMesh/Apps/VDProgMesh/Analyzer/vdpmanalyzer.cc

src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.i"
	cd /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/build/src/OpenMesh/Apps/VDProgMesh/Analyzer && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/src/OpenMesh/Apps/VDProgMesh/Analyzer/vdpmanalyzer.cc > CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.i

src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.s"
	cd /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/build/src/OpenMesh/Apps/VDProgMesh/Analyzer && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/src/OpenMesh/Apps/VDProgMesh/Analyzer/vdpmanalyzer.cc -o CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.s

src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o.requires:
.PHONY : src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o.requires

src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o.provides: src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o.requires
	$(MAKE) -f src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/build.make src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o.provides.build
.PHONY : src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o.provides

src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o.provides.build: src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o

# Object files for target Analyzer
Analyzer_OBJECTS = \
"CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o"

# External object files for target Analyzer
Analyzer_EXTERNAL_OBJECTS =

src/OpenMesh/Apps/VDProgMesh/Analyzer/Analyzer: src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o
src/OpenMesh/Apps/VDProgMesh/Analyzer/Analyzer: src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/build.make
src/OpenMesh/Apps/VDProgMesh/Analyzer/Analyzer: src/OpenMesh/Core/libOpenMeshCored.2.3.dylib
src/OpenMesh/Apps/VDProgMesh/Analyzer/Analyzer: src/OpenMesh/Tools/libOpenMeshToolsd.2.3.dylib
src/OpenMesh/Apps/VDProgMesh/Analyzer/Analyzer: src/OpenMesh/Core/libOpenMeshCored.2.3.dylib
src/OpenMesh/Apps/VDProgMesh/Analyzer/Analyzer: src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable Analyzer"
	cd /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/build/src/OpenMesh/Apps/VDProgMesh/Analyzer && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Analyzer.dir/link.txt --verbose=$(VERBOSE)
	cd /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/build/src/OpenMesh/Apps/VDProgMesh/Analyzer && "/Applications/CMake 2.8-11.app/Contents/bin/cmake" -E copy_if_different /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/build/src/OpenMesh/Apps/VDProgMesh/Analyzer/./Analyzer /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/build/Build/bin/Analyzer

# Rule to build all files generated by this target.
src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/build: src/OpenMesh/Apps/VDProgMesh/Analyzer/Analyzer
.PHONY : src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/build

src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/requires: src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/vdpmanalyzer.cc.o.requires
.PHONY : src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/requires

src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/clean:
	cd /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/build/src/OpenMesh/Apps/VDProgMesh/Analyzer && $(CMAKE_COMMAND) -P CMakeFiles/Analyzer.dir/cmake_clean.cmake
.PHONY : src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/clean

src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/depend:
	cd /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1 /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/src/OpenMesh/Apps/VDProgMesh/Analyzer /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/build /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/build/src/OpenMesh/Apps/VDProgMesh/Analyzer /Users/jordandavidson/temp/SpaceGame/OpenMesh-2.3.1/build/src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/OpenMesh/Apps/VDProgMesh/Analyzer/CMakeFiles/Analyzer.dir/depend

