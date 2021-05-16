#pragma once

#include "lua.hpp"
#include "lua_class/detail/class_registry.h"

namespace lclass
{
	namespace detail
	{
		template<typename T>
		void push(lua_State* L, T value)
		{
			static_assert(std::is_pointer<T>::value, "LuaStack: Non pointer types cannot be pushed on the lua stack like that!");
			auto cls_wrap = class_registry::getInstance(L).get_class(typeid(std::remove_pointer<T>::type));
			if (!cls_wrap)
				lua_pushlightuserdata(L, value);
			else
			{
				auto instance = cls_wrap->find_instance(value);
				if (instance)
				{
					auto new_ptr = reinterpret_cast<instance_wrapper**>(lua_newuserdata(L, sizeof(instance_wrapper*)));
					(*new_ptr) = instance;
					instance->inc_ref_count();
					luaL_setmetatable(L, "__luacpp_class_instance_metatable");
				}
				else
				{
					lua_pushnil(L);
				}
			}
		}

		template<>
		void push<const std::string&>(lua_State* L, const std::string& value) { lua_pushstring(L, value.c_str()); }

		template<>
		void push<int>(lua_State* L, int value) { lua_pushinteger(L, value); }

		template<>
		void push<double>(lua_State* L, double value) { lua_pushnumber(L, value); }

		template<>
		void push<bool>(lua_State* L, bool value) { lua_pushboolean(L, value); }

		template<class T>
		void push(lua_State* L);

		template<>
		void push<void>(lua_State* L) { lua_pushnil(L); }

		template<class T>
		T to(lua_State* L, int i = 1)
		{
			return *reinterpret_cast<T**>(lua_touserdata(L, i));
		}

		template<>
		std::string to<std::string>(lua_State* L, int i) { return lua_tostring(L, i); }

		template<>
		const std::string to<const std::string>(lua_State* L, int i) { return lua_tostring(L, i); }

		template<>
		const char* to<const char*>(lua_State* L, int i) { return lua_tostring(L, i); }

		template<>
		int to<int>(lua_State* L, int i) { return static_cast<int>(lua_tointeger(L, i)); }

		template<>
		long to<long>(lua_State* L, int i) { return static_cast<long>(lua_tointeger(L, i)); }

		template<>
		long long to<long long>(lua_State* L, int i) { return lua_tointeger(L, i); }

		template<>
		float to<float>(lua_State* L, int i) { return static_cast<float>(lua_tonumber(L, i)); }

		template<>
		double to<double>(lua_State* L, int i) { return lua_tonumber(L, i); }

		template<>
		bool to<bool>(lua_State* L, int i) { return lua_toboolean(L, i); }

		template<class T>
		bool is(lua_State* L, int i = 1) { return lua_isuserdata(L, i);	}

		template<>
		bool is<std::string>(lua_State* L, int i) {	return lua_isstring(L, i); }

		template<>
		bool is<const std::string>(lua_State* L, int i) { return lua_isstring(L, i); }

		template<>
		bool is<const char*>(lua_State* L, int i) { return lua_isstring(L, i); }

		template<>
		bool is<int>(lua_State* L, int i) {	return lua_isinteger(L, i);	}

		template<>
		bool is<long>(lua_State* L, int i) { return lua_isinteger(L, i); }

		template<>
		bool is<long long>(lua_State* L, int i)	{ return lua_isinteger(L, i); }

		template<>
		bool is<float>(lua_State* L, int i)	{ return lua_isnumber(L, i); }

		template<>
		bool is<double>(lua_State* L, int i) { return lua_isnumber(L, i); }

		template<>
		bool is<bool>(lua_State* L, int i) { return lua_isboolean(L, i); }
	}
}