# lua_class
## *Simple and minimal C++ - lua binder library*

## The library:
The purpose of this library is to make it easy for anyone to bind their C++ code with lua scripting. With the use of templates in C++ and metatables in lua, it is possible to bind and entire class and its members in a single line.


### Example usage:
### C++:
```C++
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    lclass::open_lclass(L);
    lclass::lua_class<example_class>(L)
        .ctor("example_class") //Constructor with no arguments
        .method("method_one", &example_class::method_one)
        .method("static_method", &example_class::static_method)
        .var("var_one", &example_class::var_one);
```
### Lua:
```lua
    local obj = example_class();
    obj:method_one();
    example_class.static_method();
    obj.var_one = 2;
```
---

## To do:
- Create a single include header.
- Improve error messages.

---

## CMake Compilation and Linking:
Due to the nature of the library (it uses templates extensively), it is necessary to include and compile the library with your project.

Example:

```Cmake
    add_executable(${PROJECT_NAME} main.cpp ${LUA_CLASS_SOURCES})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LUA_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_CLASS_INCLUDE_DIR} ${LUA_INCLUDE_DIR})
```
There two convenience CMake variables one for the sources ${LUA_CLASS_SOURCES} and one for the include directory ${LUA_CLASS_INCLUDE_DIR}.
