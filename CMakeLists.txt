PROJECT(can_test CXX) #项目名称
 
#Cmake最低版本要求
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
 
#允许gdb调试
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
 
#添加C++11支持及其他选项
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs -Wno-deprecated")
#set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS -g -ftest-coverage -fprofile-arcs"})

link_libraries(/usr/local/lib/) 
 
#添加dict子目录
# add_subdirectory(dict)
 
#当前目录下所有源文件赋给DIR_SRCS
AUX_SOURCE_DIRECTORY(. DIR_SRCS)
 
#生成可执行文件
add_executable(can_test ${DIR_SRCS})
 
#添加C++11编译选项,但是只有CMAKE 3.1支持
#target_compile_features(mmseg PRIVATE cxx_range_for)
 
#添加链接库
target_link_libraries(can_test ntcan)
target_link_libraries(can_test libntcan.so)
target_link_libraries(can_test libntcan.so.4)
# target_link_libraries(can_test libntcan.so.4.0.1)