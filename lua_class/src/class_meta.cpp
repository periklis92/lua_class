#include "lua_class/detail/class_meta.h"

#include <iostream>

#include "lua_class/instance_wrapper.h"
#include "lua_class/class_wrapper.h"

namespace lclass
{
	namespace detail
	{
		int class_constructor_callback(lua_State* L)
		{
			class_wrapper* ptr = static_cast<class_wrapper*>(lua_touserdata(L, 1));
			ptr->call("__cpp_ctor");
			return 1;
		}

		int class_garbage_collector(lua_State* L)
		{
			class_wrapper* ptr = static_cast<class_wrapper*>(lua_touserdata(L, 1));
			ptr->~class_wrapper();
			return 1;
		}

		int class_index_callback(lua_State* L)
		{
			class_wrapper* ptr = static_cast<class_wrapper*>(lua_touserdata(L, 1));
			assert(ptr);
			std::string name = lua_tostring(L, 2);
			if (ptr->member_exists(name))
			{
				ptr->push_call(name);
				return 1;
			}
			
			std::string get_name = "__get_" + name;
			if (ptr->member_exists(get_name))
			{
				ptr->call(get_name);
				return 1;
			}

			return 0;
		}

		int class_newindex_callback(lua_State* L)
		{
			class_wrapper* ptr = static_cast<class_wrapper*>(lua_touserdata(L, 1));
			std::string name = lua_tostring(L, 2);
			if (ptr->member_exists(name))
			{
				std::cerr << "Member with name: " << name << " already exists!" << std::endl;
				return 0;
			}

			std::string varName = "__set_" + name;
			if (ptr->member_exists(varName)) 
				return  ptr->call("__set_" + name);

			if (lua_type(L, 3) != LUA_TFUNCTION)
			{
				std::cerr << "You can only add function members to the class!\n||\t-> "
					<< ptr->name().c_str() << ":" << lua_tostring(L, 2) << " (" << lua_typename(L, 3) << ")" << std::endl;
				return 0;
			}

			// const char* name = lua_tostring(L, 2);
			// if (ptr->member_exists(name))
			// {
			// 	std::cerr << "Member with name: " << name << " already exists!" << std::endl;
			// 	return 0;
			// }
			
			ptr->push_table();
			lua_insert(L, -3);
			lua_rawset(L, -3);
			lua_pop(L, 1);

			return 1;
		}

		int class_tostring_callback(lua_State* L)
		{
			void* ptr = lua_touserdata(L, 1);
			std::string info = static_cast<class_wrapper*>(ptr)->get_info();
			lua_pushstring(L, info.c_str());
			return 1;
		}
	}
}