#include "lua_class/detail/init.h"

#include <iostream>
#include "lua_class/class_wrapper.h"
#include "lua_class/detail/class_registry.h"
#include "lua_class/detail/class_meta.h"
#include "lua_class/detail/instance_meta.h"

namespace lclass
{
	namespace detail
	{
			bool register_cpp_class(lua_State* L, const type_info& tinfo, const std::string& name)
			{
				void* ptr = lua_newuserdata(L, sizeof(class_wrapper));
				new(ptr) class_wrapper(L, tinfo, name);

				luaL_setmetatable(L, "__luacpp_class_metatable");
				lua_setglobal(L, name.c_str());

				auto& cr = class_registry::getInstance(L);
				if (cr.is_registered(name))
				{
					std::cerr << "A class with the name: " << name
						<< " already exists!" << std::endl;
					return false;
				}
				if (cr.is_registered(tinfo))
				{
					std::cerr << "A class with the type: " << tinfo.name()
						<< " already exists!" << std::endl;
					return false;
				}

				cr.register_class(static_cast<class_wrapper*>(ptr));
				return true;
			}

			void init_cpp_class_metatable(lua_State* L)
			{
				luaL_newmetatable(L, "__luacpp_class_metatable");
				lua_pushcclosure(L, class_constructor_callback, 0);
				lua_setfield(L, -2, "__call");

				lua_pushcclosure(L, class_garbage_collector, 0);
				lua_setfield(L, -2, "__gc");

				lua_pushcclosure(L, class_index_callback, 0);
				lua_setfield(L, -2, "__index");

				lua_pushcclosure(L, class_newindex_callback, 0);
				lua_setfield(L, -2, "__newindex");

				lua_pushcclosure(L, class_tostring_callback, 0);
				lua_setfield(L, -2, "__tostring");

				lua_pop(L, 1);
			}

			void init_cpp_instance_metatable(lua_State* L)
			{
				luaL_newmetatable(L, "__luacpp_class_instance_metatable");
				lua_pushcclosure(L, instance_constructor_callback, 0);
				lua_setfield(L, -2, "__call");

				lua_pushcclosure(L, instance_garbage_collector, 0);
				lua_setfield(L, -2, "__gc");

				lua_pushcclosure(L, instance_index_callback, 0);
				lua_setfield(L, -2, "__index");

				lua_pushcclosure(L, instance_newindex_callback, 0);
				lua_setfield(L, -2, "__newindex");

				lua_pop(L, 1);
			}
	}
}