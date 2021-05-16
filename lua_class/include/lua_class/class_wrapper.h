#pragma once

#include <typeindex>
#include <map>
#include <string>
#include <functional>
#include <sstream>
#include <vector>
#include <list>

#include "lua.hpp"

namespace lclass
{
	class instance_wrapper;
	class class_wrapper
	{
	public:
		class_wrapper(lua_State* L, const std::type_info& tinfo, const std::string& name);
		~class_wrapper();

		void register_member(const std::string& name, std::function<int(lua_State*)>&& function);
		int call(const std::string& name) const;
		void push_call(const std::string& name, instance_wrapper* iptr) const;
		std::string get_info() const;

		bool member_exists(const std::string& name) const { return m_members.find(name) != m_members.end(); }

		const std::string& name() const { return m_name; }
		const type_info& tinfo() const { return *m_tinfo; }

		template<class T>
		bool is_of_type() { return typeid(T) == *m_tinfo; }

		void push_table();

		void register_instance(instance_wrapper* instance);
		instance_wrapper* find_instance(void* data);

	private:
		int m_luaFuncTable;
		std::string m_name;
		const std::type_info* m_tinfo;
		lua_State* m_L;
		std::map<std::string, std::function<int(lua_State*)>> m_members;
		std::list<instance_wrapper*> m_instances;

	private:
		static int _pushed_call(lua_State* L);
	};
}