add_executable(arg arg.cpp)
target_link_libraries(arg PRIVATE argparser_obj)

add_executable(argtest argtest.cpp)
target_link_libraries(argtest gtest_main)
add_test(NAME argtest COMMAND argtest)

enable_testing()