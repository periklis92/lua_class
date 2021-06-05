//
/*
*    lua_class A C++ lua binder library
*    Copyright (C) 2021 Periklis Stinis
* 
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
* 
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
* 
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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