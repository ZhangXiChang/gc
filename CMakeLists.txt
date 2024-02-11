#Cmake最低版本
cmake_minimum_required(VERSION 3.27.0)

#项目名称
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(PROJECT_NAME)

#构建目标
add_subdirectory(app)
