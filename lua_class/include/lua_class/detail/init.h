#pragma once

#include <type_traits>
#include <string>
#include <typeinfo>
#include "lua.hpp"

namespace lclass
{
	namespace detail
	{
		bool register_cpp_class(lua_State* L, const std::type_info& tinfo, const std::string& name);
		void init_cpp_class_metatable(lua_State* L);
		void init_cpp_instance_metatable(lua_State* L);
	}
}