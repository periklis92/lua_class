project(lua_class_test)

if (MSVC)
include("C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

include_directories(${LUA_CLASS_INCLUDE_DIR})

find_package(Lua REQUIRED)

add_executable(${PROJECT_NAME} main.cpp ${LUA_CLASS_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LUA_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_CLASS_INCLUDE_DIR} ${LUA_INCLUDE_DIR})