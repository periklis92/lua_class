#include "lua_class/class_wrapper.h"

#include <algorithm>
#include <cassert>

#include "lua_class/instance_wrapper.h"

namespace lclass
{
	class_wrapper::class_wrapper(lua_State* L, const std::type_info& tinfo, const std::string& name)
		:m_tinfo(&tinfo), m_name(name), m_L(L), m_luaFuncTable(-1)
	{
		lua_newtable(L);
		m_luaFuncTable = luaL_ref(L, LUA_REGISTRYINDEX);
	}

	void class_wrapper::register_member(const std::string& name, std::function<int(lua_State*)>&& function)
	{
		m_members.emplace(name, std::move(function));
	}

	int class_wrapper::call(const std::string& name) const
	{
		auto member = m_members.find(name);
		if (member == m_members.end()) return 0;
		return member->second(m_L);
	}

	void class_wrapper::push_call(const std::string& name)
	{
		auto it = m_members.find(name);
		assert(it != m_members.end());
		m_func_to_call = &(it->second);

		auto lFunc = [](lua_State* L) -> int
		{
			class_wrapper* ptr = static_cast<class_wrapper*>(lua_touserdata(L, lua_upvalueindex(1)));
			assert(ptr);
			(*((ptr)->m_func_to_call))(L);
			ptr->m_func_to_call = nullptr;
			return 1;
		};
		lua_pushlightuserdata(m_L, this);
		lua_pushcclosure(m_L, lFunc, 1);
	}

	std::string class_wrapper::get_info() const
	{
		std::stringstream str;
		str << "class_name: " << m_name << "\n|-type_hash: " << m_tinfo->hash_code() <<
			"\n|-cpp_name: " << m_tinfo->name() << "\n|-members:";
		for (auto& m : m_members)
		{
			str << "\n||--" << m.first;
		}
		str << "\n";
		return str.str();
	}

	std::function<int(lua_State*)>* class_wrapper::get_member_prototype(const std::string& name)
	{
		auto it = m_members.find(name);
		assert(it != m_members.end());
		return &(it->second);
	}

	void class_wrapper::push_table()
	{
		lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_luaFuncTable);
	}

	void class_wrapper::register_instance(instance_wrapper* instance)
	{
		if (std::find(m_instances.begin(), m_instances.end(), instance) == m_instances.end())
			m_instances.push_back(instance);
	}

	instance_wrapper* class_wrapper::find_instance(void* data)
	{
		auto it = std::find_if(m_instances.begin(), m_instances.end(), [data](instance_wrapper* i) -> bool { return i->get_data() == data; });
		if (it == m_instances.end()) return nullptr;
		return *it;
	}

	void class_wrapper::unregister_instance(instance_wrapper* instance)
	{
		m_instances.remove(instance);
	}

	class_wrapper::~class_wrapper()
	{
		luaL_unref(m_L, LUA_REGISTRYINDEX, m_luaFuncTable);
	}
}