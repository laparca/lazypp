cmake_minimum_required(VERSION 2.8)
project (lazypp CXX)

include(GetCompilerVersion.cmake)

message("Compiler is GNUCXX? ${CMAKE_COMPILER_IS_GNUCXX}")
message("CXX version: ${CMAKE_CXX_COMPILER_VERSION}")

OPTION(DIABLE_CONCEPTS "Disables the use of concepts")

if(CMAKE_COMPILER_IS_GNUCXX AND NOT DISABLE_CONCEPTS)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 6.2 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6.2)
		message("Compiler supports concepts")
        set(lazypp_COMPILE_OPTIONS -fconcepts)
	else()
		message("Compiler doesn't supports concepts")
    endif()
else()
	message("Compiler doesn't supports concepts")
endif()

if(DISABLE_CONCEPTS)
    set(lazypp_COMPILE_OPTIONS "${lazypp_COMPILE_OPTIONS} -DDISABLE_CONCEPTS")
endif()

ADD_SUBDIRECTORY(tests)
