#pragma once

#include <typeindex>
#include <map>
#include <string>
#include <functional>
#include <sstream>
#include <vector>
#include <list>
#include <cassert>

#include "lua.hpp"

namespace lclass
{
	class instance_wrapper;
	
	/**
	 * class_wrapper, as the name suggests, wraps all the information
	 * about a c++ class necessary for lua to be able to call functions
	 */
	class class_wrapper
	{
	public:
		using lua_func = lua_CFunction;

		class_wrapper(lua_State* L, const std::type_info& tinfo, const std::string& name);
		~class_wrapper();

		/**
		 * Register a member function that can be called from lua.
		 * 
		 * @param name The name of the function.
		 * @param function The function.
		 */
		void register_member(const std::string& name, std::function<int(lua_State*)>&& function);
		
		/**
		 * Call a class member function.
		 * 
		 * @param name The name of the function.
		 */
		int call(const std::string& name) const;
		
		/**
		 * Push a static function to the stack
		 * 
		 * @param name The name of the function.
		 */
		void push_call(const std::string& name);

		/**
		 * @return String with meta info about the class.
		 */
		std::string get_info() const;

		/**
		 * Check if a specific member exists within this class.
		 * 
		 * @return Whether or not the member is registered within the class.
		 */
		bool member_exists(const std::string& name) const { return m_members.find(name) != m_members.end(); }

		/**
		 * Get a lua function that calls the member function
		 * 
		 * @param name The name of the function
		 * @return Function wrapper pointer for a lua callable function
		 */
		std::function<int(lua_State*)>* get_member_prototype(const std::string& name);

		/**
		 * @return The name of the class.
		 */
		const std::string& name() const { return m_name; }
		
		/**
		 * @return The type info of the class.
		 */
		const std::type_info& tinfo() const { return *m_tinfo; }

		/**
		 * @tparam T The type to check against.
		 * @return The type info of the class.
		 */
		template<class T>
		bool is_of_type() { return typeid(T) == *m_tinfo; }

		void push_table();
	
		/**
		 * Register an instance of the class.
		 * 
		 * @param instance Pointer to the instance wrapper of the class
		 */
		void register_instance(instance_wrapper* instance);
		
		/**
		 * Find an instance of the class from the actual instance pointer.
		 * 
		 * @param data Pointer to the instance of the class
		 * @return Pointer to the instance wrapper of the class
		 */
		instance_wrapper* find_instance(void* data);
		
		/**
		 * Unegister an instance of the class.
		 * 
		 * @param instance Pointer to the instance wrapper of the class
		 */
		void unregister_instance(instance_wrapper* instance);

	private:
		int m_luaFuncTable;
		std::string m_name;
		const std::type_info* m_tinfo;
		lua_State* m_L;
		std::map<std::string, std::function<int(lua_State*)>> m_members;
		std::list<instance_wrapper*> m_instances;
		std::function<int(lua_State*)>* m_func_to_call = nullptr;
	};
}