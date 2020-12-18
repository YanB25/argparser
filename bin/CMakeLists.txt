add_executable(arg arg.cpp)
target_link_libraries(arg PRIVATE argparser_obj)

add_executable(example example.cpp)
target_link_libraries(example PRIVATE argparser_obj)

add_executable(helloworld helloworld.cpp)
target_link_libraries(helloworld PRIVATE argparser_obj)

add_executable(tmp tmp.cpp)
target_link_libraries(tmp PRIVATE argparser_obj)