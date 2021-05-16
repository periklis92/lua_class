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
			bool is_registered(const type_info& tinfo);
			bool is_registered(const std::string& name);
			class_wrapper* get_class(const type_info& tinfo);

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