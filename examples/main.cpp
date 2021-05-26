#include <lua.hpp>
#include <lua_class/lua_class.h>
#include <iostream>
#include <string>

class example_class
{
public:
    example_class()
    {
        std::cout << "Example Class Created! No args!" << std::endl;
    }

    void set_all(int _a, int _b, int _c)
    {
        m_a = _a;
        m_b = _b;
        m_Sc = _c;
    }

    void print_all()
    {
        std::cout << "a: " << std::to_string(m_a) << "\nb:" << std::to_string(m_b) << 
            "\nc: " << std::to_string(m_Sc) << std::endl;
    }

    int add_all()
    {
        return m_a + m_b + m_Sc;
    }

public:
    int m_a = 0;
    int m_b = 20;
    static int m_Sc;
};

int example_class::m_Sc = 1;

int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    lclass::open_lclass(L);
    lclass::lua_class<example_class>(L)
        .ctor("example_class")
        .method("printAll", &example_class::print_all)
        .method("addAll", &example_class::add_all)
        .method("setAll", &example_class::set_all)
        .var("a", &example_class::m_a)
        .var("b", &example_class::m_b)
        .var("c", &example_class::m_Sc);
    

    
    int err = luaL_dostring(L, 
        "local i1 = example_class();"
        "i1.a = 2;"
        "i1.b = 10;"
        "example_class.c = 2;"
        "i1:printAll();"
        "i1:setAll(1, 2, 3);"
        "print(i1:addAll());"

    );
    
	std::cout << "error code: " << std::to_string(err) << std::endl;
    
    lua_close(L);
    return 0;
}