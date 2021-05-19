#pragma once

#include <string>
#include <tuple>
#include "lua.hpp"
#include "lua_class/detail/stack.h"

namespace lclass
{
	namespace detail
	{
		template<class...Args>
		struct args;

		template<class T, class...Args>
		struct args<T, Args...> {
			static inline std::tuple<T, Args...> get(lua_State* L, const int i = 1)
			{
				return std::tuple_cat(std::make_tuple<T>(to<T>(L, i)), args<Args...>::get(L, i + 1));
			}

			static inline void check(lua_State* L, const int i = 1)
			{
				if (!is_type<T>(L, i) && !is_convertible<T>(L, i))
					luaL_argerror(L, i, "Wrong Argument!");
				args<Args...>::check(L, i + 1);
			}
		};

		template<class T>
		struct args<T> {
			static inline std::tuple<T> get(lua_State* L, const int i = 1)
			{
				return std::tuple<T>(to<T>(L, i));
			}

			static inline void check(lua_State* L, const int i = 1)
			{
				if (!is_type<T>(L, i) && !is_convertible<T>(L, i))
					luaL_argerror(L, i, "Wrong Argument!");
			}
		};

		template<>
		struct args<> {
			static inline std::tuple<> get(lua_State* L, const int i = 1)
			{
				return std::make_tuple();
			}

			static inline void check(lua_State* L, const int i = 1)
			{
				
			}
		};

		template <int N, class T, class Alloc = std::allocator<T>>
		struct constructor {
			template <typename...Tuple, typename...Args>
			static inline void create(T* const ptr, std::tuple<Tuple...>& t, Args&&... args) {
				constructor<N - 1, T, Alloc>::create(ptr, t, std::get<N - 1>(t), std::forward<Args>(args)...);
			}
		};

		template <class T, class Alloc>
		struct constructor<0, T, Alloc> {
			template <typename...Tuple, typename...Args>
			static inline void create(T* const ptr, std::tuple<Tuple...>& t, Args&&... args) {
				Alloc().construct(ptr, std::forward<Args>(args)...);
			}
		};

		template <int N> struct apply_method {
			template <typename T, typename R, typename... FunctionArgs, typename... TupleArgs, typename... Args>
			static R apply(T* o, R(T::* function)(FunctionArgs...), std::tuple<TupleArgs...>& t, Args... args) {
				return apply_method<N - 1>::apply(o, function, t, std::get<N - 1>(t), args...);
			}
			
			template <typename R, typename... FunctionArgs, typename... TupleArgs, typename... Args>
			static R apply(R(*function)(FunctionArgs...), std::tuple<TupleArgs...>& t, Args... args) {
				return apply_method<N - 1>::apply(function, t, std::get<N - 1>(t), args...);
			}
		};

		template <> struct apply_method<0> {
			template <typename T, typename R, typename... FunctionArgs, typename... TupleArgs, typename... Args>
			static R apply(T* o, R(T::* function)(FunctionArgs...), std::tuple<TupleArgs...>& t, Args... args) {
				return (o->*function)(args...);
			}
			
			template <typename R, typename... FunctionArgs, typename... TupleArgs, typename... Args>
			static R apply(R(*function)(FunctionArgs...), std::tuple<TupleArgs...>& t, Args... args) {
				return (*function)(args...);
			}
		};

		template <int N> struct apply_function {
			template <typename R, typename... FunctionArgs, typename... TupleArgs, typename... Args>
			static R apply(R(*function)(FunctionArgs...), std::tuple<TupleArgs...>& t, Args... args) {
				return apply_function<N - 1>::apply(function, t, std::get<N - 1>(t), args...);
			}
		};

		template <> struct apply_function<0> {
			template <typename R, typename... FunctionArgs, typename... TupleArgs, typename... Args>
			static R apply(R(*function)(FunctionArgs...), std::tuple<TupleArgs...>& t, Args... args) {
				return (*function)(args...);
			}
		};
	}
}