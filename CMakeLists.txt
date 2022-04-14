cmake_minimum_required(VERSION 3.15)

project(version)
enable_language(C CXX ASM_MASM)

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/libs/minhook/include
  ${CMAKE_CURRENT_LIST_DIR}/libs
  ${CMAKE_CURRENT_LIST_DIR})

file(GLOB sources umamusume/*.cc umamusume/*.hpp umamusume/*/*.cc umamusume/*/*.def)
file(GLOB minhook libs/minhook/src/*.c libs/minhook/src/*/*.c)
file(GLOB json libs/nlohmann/*.hpp)

add_library(${PROJECT_NAME} SHARED ${minhook} ${json} ${sources})
  
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)