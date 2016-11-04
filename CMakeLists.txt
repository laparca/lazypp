cmake_minimum_required(VERSION 2.8)
project (lazypp CXX)

if(CMAKE_COMPILER_IS_GNUCXX)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 6.2 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6.2)
        set(lazypp_COMPILE_OPTIONS -fconcepts)
    endif()
endif()

ADD_SUBDIRECTORY(tests)
