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

#include <functional>
#include <iostream>
#include <cassert>

#include "lua_class/detail/class_registry.h"

namespace lclass
{
	class class_wrapper;

	class instance_wrapper
	{
	public:
		instance_wrapper(class_wrapper* cls, void* data);

		void* get_data() const { return m_data; }

		void inc_ref_count() { m_refCount++; }
		void dec_ref_count() { m_refCount--; }
		int  get_ref_count() { return m_refCount; }

		class_wrapper& get_class() const;
		void push_function(std::function<int(lua_State*)>* function);
		void push_function(lua_State*L, const std::string& name);

	private:
		static int __call(lua_State* L);

	private:
		int m_refCount = 1;
		class_wrapper* m_cls = nullptr;
		void* m_data = nullptr;
		std::function<int(lua_State*)>* m_func_to_call = nullptr;
	};
}