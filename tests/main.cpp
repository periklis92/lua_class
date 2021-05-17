#include <lua.hpp>
#include <lua_class/lua_class.h>
#include <iostream>
#include <string>

const auto l = []() -> void {};

class test_class
{
public:
    test_class()
    {
        std::cout << "Test Class Created!" << std::endl;
    }
    
    void void1()
    {
        std::cout << "Test Function Void called!" << std::endl;
        a = 1;
    }

    void void2()
    {
        std::cout << "Test Function Void 2 called!" << std::endl;
        a = 2;
    }
    
    void add(int f, int s)
    {
        std::cout <<std::to_string(f) << "+" << std::to_string(s) << "=" << std::to_string(f + s) << std::endl;
    }

    int add_a_b()
    {
        int c = a + b;
        std::cout << "a+b=" << std::to_string(c) << std::endl;
        return c;
    }

    void pointer_from(test_class* t)
    {
        std::cout << "pointer has a value of: " << std::to_string(t->a) << std::endl;
    }

    test_class* instance()
    {
        std::cout << "Test Function returning instance called!" << std::endl;
        return this;
    }

    ~test_class()
    {
        std::cout << "Destroyed!" << std::endl;
    }

public:
    int a = 0;
    int b = 20;
};

int main()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    lclass::open_lclass(L);
    lclass::lua_class<test_class>
        ::ctor(L, "test_class")
        .method("void", &test_class::void1)
        .method("void2", &test_class::void2)
        .method("add", &test_class::add)
        .method("add_a_b", &test_class::add_a_b)
        .method("pointer_from", &test_class::pointer_from)
        .var("a_var", &test_class::a)
        .var("b_read", &test_class::b, true)
        .method("instance", &test_class::instance);
    
    
    int err = luaL_dostring(L, 
    "local e = test_class();    "
    "local e3 = test_class();    "
    "local array = {};"
    "e:void();                  "
    "local e2 = e:instance();   " 
    "e2:void2();                "
    "e2:add(1.4, 2);              "
    "local c = e2:add_a_b();    "
    "print(\"c= \", c);         "
    "e.a_var = 3;"
    "e2.a_var = 4;"
    "e3.a_var = 125;"
    "e2 = e3;"
    "print(e2.a_var);"
    "print(e.a_var);"
    "print(e3.a_var);"
    "e:pointer_from(e3);"
    );
    
	std::cout << "test1 errCode: " << std::to_string(err) << std::endl;

    // err = luaL_dostring(L,
    // "for i=1,10000 do            "
    // "   local i = test_class()  "
    // "end                        "
    // );
	// std::cout << "test2 errCode: " << std::to_string(err) << std::endl;
    
    lua_close(L);
	// std::cin.get();
    return 0;
}