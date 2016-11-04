enable_testing()

find_package(Boost COMPONENTS unit_test_framework)

include_directories(${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})

ADD_EXECUTABLE(lazypptests tests.cpp)

ADD_TEST(NAME lazytests COMMAND lazypptests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" "${lazypp_COMPILE_OPTIONS}")
# target_compile_options doesn't work in travis-ci
#target_compile_options(lazypptests PRIVATE ${lazypp_COMPILE_OPTIONS})
