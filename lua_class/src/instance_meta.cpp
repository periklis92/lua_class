#include "lua_class/detail/instance_meta.h"

#include <string>
#include <iostream>
#include "lua_class/class_wrapper.h"
#include "lua_class/instance_wrapper.h"
namespace lclass
{
	namespace detail
	{
		int instance_newindex_callback(lua_State* L)
		{
			instance_wrapper* ptr = reinterpret_cast<instance_wrapper*>(lua_touserdata(L, 1));
			std::string name = lua_tostring(L, 2);
			std::string varName = "__set_" + name;
			if (ptr->get_class().member_exists(name))
				return 0;
			if (ptr->get_class().member_exists(varName)) 
				return  ptr->get_class().call("__set_" + name);

			return 0;
		}

		int instance_constructor_callback(lua_State* L)
		{
			std::cerr << "You can not call class instance as a function!" << std::endl;
			return 0;
		}

		int instance_garbage_collector(lua_State* L)
		{
			instance_wrapper* ptr = reinterpret_cast<instance_wrapper*>(lua_touserdata(L, 1));
			ptr->get_class().call("__cpp_dtor");
			return 1;
		}

		int instance_index_callback(lua_State* L)
		{
			instance_wrapper** iptr = reinterpret_cast<instance_wrapper**>(lua_touserdata(L, 1));
			class_wrapper& ptr = (*iptr)->get_class();
			int top = lua_gettop(L);
			if (!iptr)
			{
				std::cerr << "Object instance is null" << std::endl;
				lua_pushnil(L);
				return 0;
			}

			std::string name = lua_tostring(L, 2);
			if (ptr.member_exists(name))
			{
				ptr.push_call(name, *iptr);
				return 1;
			}

			std::string get_name = "__get_" + name;
			if (ptr.member_exists(get_name))
			{
				ptr.call(get_name);
				return 1;
			}

			ptr.push_table();
			lua_insert(L, -2);
			lua_rawget(L, -2);

			return 1;
		}
	}
}