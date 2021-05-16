#pragma once

#include <string>
#include <lua.hpp>
#include <typeinfo>
#include "lua_class/instance_wrapper.h"
#include "lua_class/detail/class_registry.h"

namespace lclass
{
	void registerCManagedInstace(lua_State* L, const std::string& globalName, const std::type_info& tinfo, void* ptr)
	{
		class_wrapper* cls = detail::class_registry::getInstance(L).get_class(tinfo);

		if (!cls) return;

		instance_wrapper* iptr = static_cast<instance_wrapper*>(lua_newuserdata(L, sizeof(instance_wrapper)));

		if (!iptr) return;

		new(iptr) instance_wrapper(cls, ptr);
		luaL_setmetatable(L, "__luacpp_class_instance_metatable");
		lua_setglobal(L, globalName.c_str());
	}
}