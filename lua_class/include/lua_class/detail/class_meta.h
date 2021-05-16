#pragma once

#include "lua.hpp"

namespace lclass
{
	namespace detail
	{
		int class_constructor_callback(lua_State* L);
		int class_garbage_collector(lua_State* L);
		int class_index_callback(lua_State* L);
		int class_newindex_callback(lua_State* L);
		int class_tostring_callback(lua_State* L);
	}
}