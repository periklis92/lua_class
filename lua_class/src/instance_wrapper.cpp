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