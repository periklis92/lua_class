#include <lua.hpp>
#include <lua_class/lua_class.h>
#include <iostream>

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

    test_class* instance()
    {
        std::cout << "Test Function returning instance called!" << std::endl;
        return this;
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
    lclass::lua_class<test_class>::
        ctor(L, "test_class")
        .method("void", &test_class::void1)
        .method("void2", &test_class::void2)
        .method("add", &test_class::add)
        .method("add_a_b", &test_class::add_a_b)
        .var("a_var", &test_class::a)
        .var("b_read", &test_class::b, true)
        .method("instance", &test_class::instance);

    int err = luaL_dostring(L, 
    "local e = test_class();    "
    "e:void();                  " 
    "local e2 = e:instance();   " 
    "e2:void2();                "
    "e2:add(1, 2);              "
    "local c = e2:add_a_b();    "
    "print(c);                  "
    );

	std::cout << "finished errCode: " << std::to_string(err) << std::endl;
	std::cin.get();
    return 0;
}