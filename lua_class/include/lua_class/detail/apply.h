#pragma once

#include <string>
#include <tuple>
#include "lua.hpp"
#include "lua_class/detail/stack_temp.h"

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

			static inline bool check(lua_State* L, int& arg_err, const int i = 1)
			{
				bool result = is<T>(L, i);
				if (result)
				{
					return args<Args...>::check(L, arg_err, i + 1);
				}
				arg_err = i;
				return false;
			}
		};

		template<class T>
		struct args<T> {
			static inline std::tuple<T> get(lua_State* L, const int i = 1)
			{
				return std::tuple<T>(to<T>(L, i));
			}

			static inline bool check(lua_State* L, int& arg_err, const int i = 1)
			{
				bool result = is<T>(L, i);
				if (result)
				{
					return true;
				}
				arg_err = i;
				return false;
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
		};

		template <> struct apply_method<0> {
			template <typename T, typename R, typename... FunctionArgs, typename... TupleArgs, typename... Args>
			static R apply(T* o, R(T::* function)(FunctionArgs...), std::tuple<TupleArgs...>& t, Args... args) {
				return (o->*function)(args...);
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