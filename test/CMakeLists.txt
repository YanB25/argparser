add_executable(parse_flag parse_flag.cpp)
target_link_libraries(parse_flag gtest_main argparser_obj)
add_test(NAME parse_flag COMMAND parse_flag)

add_executable(parse_combined_flag parse_combined_flag.cpp)
target_link_libraries(parse_combined_flag gtest_main argparser_obj)
add_test(NAME parse_combined_flag COMMAND parse_combined_flag)

add_executable(misc misc.cpp)
target_link_libraries(misc gtest_main argparser_obj)
add_test(NAME misc COMMAND misc)

add_executable(cmd cmd.cpp)
target_link_libraries(cmd gtest_main argparser_obj)
add_test(NAME cmd COMMAND cmd)

add_executable(global_flag global_flag.cpp)
target_link_libraries(global_flag gtest_main argparser_obj)
add_test(NAME global_flag COMMAND global_flag)

add_executable(flag_store flag_store.cpp)
target_link_libraries(flag_store gtest_main argparser_obj)
add_test(NAME flag_store COMMAND flag_store)

add_executable(parse_custom parse_custom.cpp)
target_link_libraries(parse_custom gtest_main argparser_obj)
add_test(NAME parse_custom COMMAND parse_custom)

enable_testing()