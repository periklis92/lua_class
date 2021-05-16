#pragma once

#include <functional>
#include "lua_class/detail/class_registry.h"


namespace lclass
{
	class class_wrapper;

	class instance_wrapper
	{
	public:
		instance_wrapper(class_wrapper* cls, void* data)
			:m_cls(cls), m_data(data) {}

		void* get_data() const { return m_data; }

		class_wrapper& get_class() const
		{
			return *m_cls;
		}

		void push_function(const std::function<int(lua_State*)>* function)
		{
			m_func_to_call = function;
		}

		void call_function(lua_State* L)
		{
			(*m_func_to_call)(L);
			m_func_to_call = nullptr;
		}

	private:
		class_wrapper* m_cls = nullptr;
		void* m_data = nullptr;
		const std::function<int(lua_State*)>* m_func_to_call = nullptr;
	};
}