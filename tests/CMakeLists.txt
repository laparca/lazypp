enable_testing()

find_package(Boost COMPONENTS unit_test_framework)

include_directories(${PROJECT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})

ADD_EXECUTABLE(lazypptests tests.cpp)

ADD_TEST(NAME lazytests COMMAND lazypptests)

target_compile_options(lazypptests PRIVATE ${lazypp_COMPILE_OPTIONS})
#target_link_libraries(lazypptests ${Boost_LIBRARIES})
