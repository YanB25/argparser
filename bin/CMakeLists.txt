add_executable(arg arg.cpp)
target_link_libraries(arg PRIVATE argparser_obj)

add_executable(example example.cpp)
target_link_libraries(example PRIVATE argparser_obj)