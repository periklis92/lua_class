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

#include "lua_class/instance_wrapper.h"
#include "lua_class/class_wrapper.h"

namespace lclass
{
		instance_wrapper::instance_wrapper(class_wrapper* cls, void* data)
			:m_cls(cls), m_data(data) {}


		class_wrapper& instance_wrapper::get_class() const
		{
			return *m_cls;
		}

		void instance_wrapper::push_function(std::function<int(lua_State*)>* function)
		{
			m_func_to_call = function;
		}

		void instance_wrapper::push_function(lua_State*L, const std::string& name)
		{
			assert(m_cls);
			auto f = m_cls->get_member_prototype(name);
			assert(f);
			m_func_to_call = f;
            lua_pushcclosure(L, __call, 0);
		}
		
		int instance_wrapper::__call(lua_State* L)
		{
			instance_wrapper** ptr = static_cast<instance_wrapper**>(lua_touserdata(L, 1));
			assert(ptr);
			(*(*ptr)->m_func_to_call)(L);
			(*ptr)->m_func_to_call = nullptr;
			return 1;
		}
}