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