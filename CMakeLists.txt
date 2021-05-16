cmake_minimum_required(VERSION 3.0.0)
project(lua_class VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lua_class/include )

file(GLOB LUA_CLASS_HEADERS
${CMAKE_CURRENT_SOURCE_DIR}/lua_class/include/*.h
${CMAKE_CURRENT_SOURCE_DIR}/lua_class/include/**/.h)

file(GLOB LUA_CLASS_INCLUDE_DIR
${CMAKE_CURRENT_SOURCE_DIR}/lua_class/include)

file(GLOB LUA_CLASS_SOURCES
${CMAKE_CURRENT_SOURCE_DIR}/lua_class/src/*.cpp)

# add_library(lua_class ${LUA_CLASS_HEADERS} ${LUA_CLASS_SOURCES})

# target_include_directories(${PROJECT_NAME} PUBLIC
#   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
#   $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
# )
add_subdirectory(tests)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
