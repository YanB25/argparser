add_executable(arg arg.cpp)
target_link_libraries(arg PRIVATE argparser_obj)

add_executable(parse_flag parse_flag.cpp)
target_link_libraries(parse_flag gtest_main argparser_obj)
add_test(NAME parse_flag COMMAND parse_flag)

enable_testing()