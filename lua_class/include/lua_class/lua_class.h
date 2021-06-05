#pragma once

#include <string>
#include <memory>
#include <functional>
#include <type_traits>
#include <cassert>

#include "lua.hpp"

#include "lua_class/detail/init.h"
#include "lua_class/detail/class_registry.h"
#include "lua_class/class_wrapper.h"
#include "lua_class/instance_wrapper.h"
#include "lua_class/detail/apply.h"

struct error_creating_class {};
namespace lclass
{
	/**
	 * Initialize the library for a specify lua state
	 * 
	 * @param L The lua State.
	 */
	void open_lclass(lua_State* L)
	{
		detail::init_cpp_class_metatable(L);
		detail::init_cpp_instance_metatable(L);
	}

	/**
	 * Invokes a function for all instances
	 * 
	 * @param name The name of the function.
	 */
	template<typename T>
	void invoke_function(lua_State* L, const std::string& name)
	{
		detail::class_registry& cr = detail::class_registry::getInstance(L);
		class_wrapper* cw = cr.get_class(typeid(T));
		cw->invoke_for_all(name);
	}

	/**
	 * Helper class to bind any class to a lua state
	 * @tparam T The class to register
	 */
	template<class T>
	class lua_class
	{
	public:
		using Alloc = std::allocator<T>;

		/**
		 * Creates a new lua_class for the specific lua State
		 * @param L The lua state in which the class will be registered
		 */
		lua_class(lua_State* L)
		:m_L(L)
		{
		}

		/**
		 * Registers a constructor with arguments
		 * @tparam Args Constructor arguments
		 * @param name The name used to create the class in lua
		 */
		template<class...Args>
		lua_class& ctor(const std::string& name)
		{
			bool res = detail::register_cpp_class(m_L, typeid(T), name);
			if (!res) throw error_creating_class();
			auto ctor = [](lua_State* L) -> int
			{
				class_wrapper* ptr = static_cast<class_wrapper*>(lua_touserdata(L, 1));
				assert(ptr);
				instance_wrapper** iptr = static_cast<instance_wrapper**>(lua_newuserdata(L, sizeof(instance_wrapper*)));
				assert(iptr);
				luaL_setmetatable(L, "__luacpp_class_instance_metatable");

				
				Alloc alloc = Alloc();
				T* new_inst = alloc.allocate(1);
				assert(new_inst);

				detail::args<Args...>::check(L, 2);

				auto tuple = detail::args<Args...>::get(L, 2);
				detail::constructor<sizeof...(Args), T>::create(new_inst, tuple);
				instance_wrapper* new_instance = new instance_wrapper(ptr, new_inst);
				assert(new_instance);
				*iptr = new_instance;
				ptr->register_instance(new_instance);

				return 1;
			};
			auto dtor = [](lua_State* L) ->int
			{
				instance_wrapper** iptr = static_cast<instance_wrapper**>(lua_touserdata(L, 1));
				assert(*iptr);
				if ((*iptr)->get_ref_count() > 1)
				{
					(*iptr)->dec_ref_count();
				}
				else
				{
					class_wrapper& ptr = (*iptr)->get_class();
					ptr.unregister_instance(*iptr);
					T* inst = static_cast<T*>((*iptr)->get_data());
					assert(inst);
					Alloc alloc = Alloc();
					alloc.destroy(inst);
					alloc.deallocate(inst, 1);
					delete *iptr;
				}
				return 1;
			};
			detail::class_registry& cw = detail::class_registry::getInstance(m_L);
			cw.get_class(typeid(T))->register_member("__cpp_ctor", std::function<int(lua_State*)>(ctor));
			cw.get_class(typeid(T))->register_member("__cpp_dtor", std::function<int(lua_State*)>(dtor));
			return *this;
		}

		/**
		 * Registers a member variable
		 * @tparam R The type of the variable
		 * @param name The name used in lua to access the member
		 * @param _member_var The address of the member in C++
		 * @param readonly Whether the value can only be read from lua (default: false)
		 */
		template<class R>
		lua_class& var(const std::string& name, R(T::* _member_var), bool readonly = false)
		{
			class_wrapper* cw = detail::class_registry::getInstance(m_L).get_class(typeid(T));
			if (!readonly)
			{
				auto _set = [_member_var](lua_State* L) -> int
				{
					//stack of __newindex { object, key, value }
					instance_wrapper** iptr = static_cast<instance_wrapper**>(lua_touserdata(L, 1));
					assert(iptr);
					const char* name = lua_tostring(L, 2);
					R _value = detail::to<R>(L, 3);
					T* _inst = static_cast<T*>((*iptr)->get_data());
					assert(_inst);
					if (_inst)
					{
						_inst->*_member_var = _value;
						return 1;
					}
					return 0;
				};
				cw->register_member("__set_" + name, std::function<int(lua_State*)>(_set));
			}
			auto _get = [_member_var](lua_State* L) -> int
			{
				//stack of __newindex { object, key, value }
				instance_wrapper** iptr = static_cast<instance_wrapper**>(lua_touserdata(L, 1));
				assert(iptr);
				const char* name = lua_tostring(L, 2);
				T* _inst = static_cast<T*>((*iptr)->get_data());
				assert(_inst);
				if (_inst)
				{
					R _value = _inst->*_member_var;
					detail::push<R>(L, _value);
					return 1;
				}
				return 0;
			};
			cw->register_member("__get_" + name, std::function<int(lua_State*)>(_get));
			return *this;
		}

		/**
		 * Registers a static member variable
		 * @tparam R The type of the variable
		 * @param name The name used in lua to access the member
		 * @param _member_var The address of the member in C++
		 * @param readonly Whether the value can only be read from lua (default: false)
		 */
		template<class R>
		lua_class& var(const std::string& name, R(*_member_var), bool readonly = false)
		{
			class_wrapper* cw = detail::class_registry::getInstance(m_L).get_class(typeid(T));
			if (!readonly)
			{
				auto _set = [_member_var](lua_State* L) -> int
				{
					const char* name = lua_tostring(L, 2);
					R _value = detail::to<R>(L, 3);
					*_member_var = _value;
					return 0;
				};
				cw->register_member("__set_" + name, std::function<int(lua_State*)>(_set));
			}
			auto _get = [_member_var](lua_State* L) -> int
			{
				const char* name = lua_tostring(L, 2);
				R _value = *_member_var;
				detail::push<R>(L, _value);
				return 1;
			};
			cw->register_member("__get_" + name, std::function<int(lua_State*)>(_get));
			return *this;
		}

		/**
		 * Registers a member method
		 * @tparam Args The argument types of the method
		 * @param name The name used in lua to access the member
		 * @param method The address of the member in C++
		 */
		template<class...Args>
		lua_class& method(const std::string& name, void(T::* method)(Args...))
		{
			auto function = [method](lua_State* L) -> int
			{
				instance_wrapper** iptr = static_cast<instance_wrapper**>(lua_touserdata(L, 1));
				assert(iptr);
				detail::args<Args...>::check(L, 2);
				auto tuple = detail::args<Args...>::get(L, 2);
				detail::apply_method<sizeof...(Args)>::apply(static_cast<T*>((*iptr)->get_data()), method, tuple);
				return 1;
			};
			class_wrapper* cw = detail::class_registry::getInstance(m_L).get_class(typeid(T));
			cw->register_member(name, std::function<int(lua_State*)>(function));
			return *this;
		}

		/**
		 * Registers a member method that returns a value
		 * @tparam R The return type of the method
		 * @tparam Args The argument types of the method
		 * @param name The name used in lua to access the member
		 * @param method The address of the member in C++
		 */
		template<class R, class...Args>
		lua_class& method(const std::string& name, R(T::* method)(Args...))
		{
			auto function = [method](lua_State* L) -> int
			{
				instance_wrapper** iptr = static_cast<instance_wrapper**>(lua_touserdata(L, 1));
				assert(iptr);
				detail::args<Args...>::check(L, 2);
				auto tuple = detail::args<Args...>::get(L, 2);
				R ret = detail::apply_method<sizeof...(Args)>::apply(static_cast<T*>((*iptr)->get_data()), method, tuple);
				detail::push(L, ret);
				return 1;
			};
			class_wrapper* cw = detail::class_registry::getInstance(m_L).get_class(typeid(T));
			cw->register_member(name, std::function<int(lua_State*)>(function));
			return *this;
		}		

		
		/**
		 * Registers a static member method
		 * @tparam Args The argument types of the method
		 * @param name The name used in lua to access the member
		 * @param method The address of the member in C++
		 */
		template<class...Args>
		lua_class& method(const std::string& name, void(*method)(Args...))
		{
			auto function = [method](lua_State* L) -> int
			{
				detail::args<Args...>::check(L, 1);
				auto tuple = detail::args<Args...>::get(L, 1);
				detail::apply_method<sizeof...(Args)>::apply(method, tuple);
				return 1;
			};
			class_wrapper* cw = detail::class_registry::getInstance(m_L).get_class(typeid(T));
			cw->register_member(name, std::function<int(lua_State*)>(function));
			return *this;
		}

		/**
		 * Registers a static member method that returns a value
		 * @tparam R The return type of the method
		 * @tparam Args The argument types of the method
		 * @param name The name used in lua to access the member
		 * @param method The address of the member in C++
		 */
		template<class R, class...Args>
		lua_class& method(const std::string& name, R(*method)(Args...))
		{
			auto function = [method](lua_State* L) -> int
			{
				detail::args<Args...>::check(L, 2);
				auto tuple = detail::args<Args...>::get(L, 2);
				R ret = detail::apply_method<sizeof...(Args)>::apply(method, tuple);
				detail::push(L, ret);
				return 1;
			};
			class_wrapper* cw = detail::class_registry::getInstance(m_L).get_class(typeid(T));
			cw->register_member(name, std::function<int(lua_State*)>(function));
			return *this;
		}

	private:
		lua_State* m_L;

	private:
		static void __ctor_arg_throw(lua_State* L, const std::string name, int position)
		{
			std::cerr << "Error constructing class " << name << ". Wrong type of parameter found at position " <<
				position << "." << std::endl;
			lua_pushnil(L);
		}
	};
}