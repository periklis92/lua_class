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

#include <typeindex>
#include <memory>
#include <map>
#include <string>

#include "lua.hpp"

namespace lclass
{
	class class_wrapper;
	namespace detail
	{
		class class_registry
		{
		public:
			static class_registry& getInstance(lua_State* L);
			void register_class(class_wrapper* cls);
			bool is_registered(const std::type_info& tinfo);
			bool is_registered(const std::string& name);
			class_wrapper* get_class(const std::type_info& tinfo);

		private:
			lua_State* m_L;
			std::map<std::type_index, class_wrapper*> m_classes;

		private:

			class_registry(lua_State* L)
				:m_L(L)
			{
			}

			~class_registry()
			{
			}

			static int __destroy(lua_State* L);

		};
	}
}