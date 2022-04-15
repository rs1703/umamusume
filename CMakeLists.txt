cmake_minimum_required(VERSION 3.15)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

include("cmake/HunterGate.cmake")
HunterGate(
  URL "https://github.com/cpp-pm/hunter/archive/v0.23.297.tar.gz"
  SHA1 "3319fe6a3b08090df7df98dee75134d68e2ef5a3")

project(version)
enable_language(C CXX ASM_MASM)

hunter_add_package(sqlite3)
find_package(sqlite3 CONFIG REQUIRED)

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/libs/minhook/include
  ${CMAKE_CURRENT_LIST_DIR}/libs
  ${CMAKE_CURRENT_LIST_DIR}
  ${SQLITE3_ROOT}/include)

file(GLOB PROJECT_SRC umamusume/*.cc umamusume/*.hpp umamusume/*/*.cc umamusume/*/*.def)
file(GLOB MINHOOK_SRC libs/minhook/src/*.c libs/minhook/src/*/*.c)
file(GLOB JSON_SRC libs/nlohmann/*.hpp)

add_library(${PROJECT_NAME} SHARED ${MINHOOK_SRC} ${JSON_SRC} ${PROJECT_SRC})

file(GLOB DUMPER_SRC tools/dumper/*.cc)
add_executable(dumper ${JSON_SRC} ${DUMPER_SRC})
target_link_libraries(dumper PUBLIC sqlite3::sqlite3)

file(GLOB COMPARE_SRC tools/compare/*.cc)
add_executable(compare ${JSON_SRC} ${COMPARE_SRC})
target_link_libraries(compare PUBLIC sqlite3::sqlite3)