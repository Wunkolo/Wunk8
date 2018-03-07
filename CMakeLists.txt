cmake_minimum_required( VERSION 3.2.2 )
project( Wunk8 )

### Standard
set( CMAKE_CXX_STANDARD 14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

### Verbosity
set( CMAKE_COLOR_MAKEFILE ON )
set( CMAKE_VERBOSE_MAKEFILE ON )

### Optimizations
if( MSVC )
	add_compile_options( /arch:AVX2 )
	add_compile_options( /Wall )
elseif( CMAKE_COMPILER_IS_GNUCXX )
	add_compile_options( -m64 )
	add_compile_options( -march=native )
	add_compile_options( -Ofast )
	add_compile_options( -Wall )
	add_compile_options( -Wextra )
endif()


### Targets
add_executable(
	Wunk8
	source/main.cpp
	source/Wunk8.cpp
)
target_include_directories(
	Wunk8
	PRIVATE
	include
)