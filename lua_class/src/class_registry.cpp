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

#include "lua_class/detail/class_registry.h"

#include <iostream>
#include "lua_class/class_wrapper.h"

namespace lclass
{
	namespace detail
	{
		class_registry& class_registry::getInstance(lua_State* L)
		{
			lua_pushstring(L, "__luacpp_class_registry");
			int t = lua_gettable(L, LUA_REGISTRYINDEX);

			if (t == LUA_TNIL)
			{
				lua_pop(L, 1);
				auto cr = new class_registry(L);
				*reinterpret_cast<class_registry**>(lua_newuserdata(L, sizeof(class_registry*)))
					= cr;

				luaL_newmetatable(L, "__luacpp_class_registry_metatable");
				lua_pushstring(L, "__gc");
				lua_pushcclosure(L, __destroy, 0);
				lua_rawset(L, -3);

				lua_setmetatable(L, -2);
				lua_pushstring(L, "__luacpp_class_registry");
				lua_insert(L, -2);
				lua_settable(L, LUA_REGISTRYINDEX);

				return *cr;
			}

			auto ptr = *static_cast<class_registry**>(lua_touserdata(L, -1));
			lua_pop(L, 1);
			return *ptr;
		}

		void class_registry::register_class(class_wrapper* cls)
		{
			if (is_registered(cls->tinfo()))
			{
				std::cerr << "Class of type: " << cls->tinfo().name() << " is already registered!" << std::endl;
				return;
			}
			m_classes.insert(std::pair<std::type_index, class_wrapper*>(std::type_index(cls->tinfo()), cls));
		}

		bool class_registry::is_registered(const std::type_info& tinfo)
		{
			return m_classes.find(tinfo) != m_classes.end();
		}

		bool class_registry::is_registered(const std::string& name)
		{
			for (auto& cls : m_classes)
			{
				if (cls.second->name() == name) return true;
			}
			return false;
		}

		class_wrapper* class_registry::get_class(const std::type_info& tinfo)
		{
			auto it = m_classes.find(tinfo);
			if (it == m_classes.end()) return nullptr;
			return it->second;
		}

		int class_registry::__destroy(lua_State* L)
		{
			auto ptr = *static_cast<class_registry**>(lua_touserdata(L, -1));
			ptr->~class_registry();
			std::free(ptr);
			return 1;
		}
	}
}