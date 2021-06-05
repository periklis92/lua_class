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

#include <string>
#include <lua.hpp>
#include <typeinfo>
#include "lua_class/instance_wrapper.h"
#include "lua_class/detail/class_registry.h"

namespace lclass
{
	void registerCManagedInstace(lua_State* L, const std::string& globalName, const std::type_info& tinfo, void* ptr)
	{
		class_wrapper* cls = detail::class_registry::getInstance(L).get_class(tinfo);

		if (!cls) return;

		instance_wrapper* iptr = static_cast<instance_wrapper*>(lua_newuserdata(L, sizeof(instance_wrapper)));

		if (!iptr) return;

		new(iptr) instance_wrapper(cls, ptr);
		luaL_setmetatable(L, "__luacpp_class_instance_metatable");
		lua_setglobal(L, globalName.c_str());
	}
}