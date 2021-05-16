#pragma once

#include "lua.hpp"

namespace lclass
{
	namespace detail
	{
		int instance_newindex_callback(lua_State* L);
		int instance_constructor_callback(lua_State* L);
		int instance_garbage_collector(lua_State* L);
		int instance_index_callback(lua_State* L);
	}
}