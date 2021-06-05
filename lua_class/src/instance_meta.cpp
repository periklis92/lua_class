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
			instance_wrapper** iptr = reinterpret_cast<instance_wrapper**>(lua_touserdata(L, 1));
			std::string name = lua_tostring(L, 2);
			std::string varName = "__set_" + name;
			if ((*iptr)->get_class().member_exists(name))
				return 0;
			if ((*iptr)->get_class().member_exists(varName)) 
				return  (*iptr)->get_class().call("__set_" + name);

			return 0;
		}

		int instance_constructor_callback(lua_State* L)
		{
			std::cerr << "You can not call class instance as a function!" << std::endl;
			return 0;
		}

		int instance_garbage_collector(lua_State* L)
		{
			instance_wrapper** iptr = reinterpret_cast<instance_wrapper**>(lua_touserdata(L, 1));
			(*iptr)->get_class().call("__cpp_dtor");
			return 1;
		}

		int instance_index_callback(lua_State* L)
		{
			instance_wrapper** iptr = reinterpret_cast<instance_wrapper**>(lua_touserdata(L, 1));
			
			assert(iptr);
			class_wrapper& ptr = (*iptr)->get_class();

			std::string name = lua_tostring(L, 2);
			if (ptr.member_exists(name))
			{
				(*iptr)->push_function(L, name);
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