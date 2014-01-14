#pragma once

#include "Fun.h"
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include "util.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

template <typename T, typename... Rest>
T singular_t();

template <typename... T>
std::tuple<T...> tuple_t();

namespace luna {
class State {
private:
    lua_State *_l;

    template<typename T, typename... Rest>
    T read_s() {
        return Read<T>(-1);
    }

    std::map<std::string, std::unique_ptr<BaseFun>> _funs;
public:
    State();
    State(const State &other) = delete;
    State &operator=(const State &other) = delete;
    State(State &&other);
    ~State();

    bool Load(const std::string &file);

    void Push() {} // Base case

    template <typename T, typename... Ts>
    void Push(T &&value, Ts&&... values) {
        detail::_push(_l, std::forward<T>(value));
        Push(std::forward<Ts>(values)...);
    }

    // Lua stacks are 1 indexed from the bottom and -1 indexed from
    // the top
    template <typename T>
    T Read(const int index) const {
        return detail::_get<T>(_l, index);
    }

private:
    // Worker type-trait struct to Pop
    // Popping multiple elements returns a tuple
    template <size_t, typename... Ts> // First template argument
                                      // denotes the sizeof(Ts...)
    struct _pop {
        typedef std::tuple<Ts...> type;

        template <typename T>
        static std::tuple<T> worker(const State &l, const int index) {
            return std::make_tuple(l.Read<T>(index));
        }

        template <typename T1, typename T2, typename... Rest>
        static std::tuple<T1, T2, Rest...> worker(const State &l,
                                                  const int index) {
            std::tuple<T1> head = std::make_tuple(l.Read<T1>(index));
            return std::tuple_cat(head, worker<T2, Rest...>(l, index + 1));
        }

        static type apply(State &l) {
            auto ret = worker<Ts...>(l, 1);
            lua_pop(l._l, sizeof...(Ts));
            return ret;
        }
    };

    // Popping nothing returns void
    template <typename... Ts>
    struct _pop<0, Ts...> {
        typedef void type;
        static type apply(State &l) {}
    };

    // Popping one element returns an unboxed value
    template <typename T>
    struct _pop<1, T> {
        typedef T type;
        static type apply(State &l) {
            T ret = l.Read<T>(-1);
            lua_pop(l._l, 1);
            return ret;
        }
    };

public:
    template <typename... T>
    typename _pop<sizeof...(T), T...>::type Pop() {
        return _pop<sizeof...(T), T...>::apply(*this);
    }

    // Calls a lua function with variadic return parameters and
    // function arguments
    template <typename... Ret, typename... Args>
    typename _pop<sizeof...(Ret), Ret...>::type Call(const std::string &fun,
                                     Args&&... args) {
        lua_getglobal(_l, fun.c_str());
        const int num_args = sizeof...(Args);
        const int num_ret = sizeof...(Ret);
        Push(std::forward<Args>(args)...);
        lua_call(_l, num_args, num_ret);
        return Pop<Ret...>();
    }

    template <typename Ret, typename... Args>
    void Register(const std::string &name, std::function<Ret(Args...)> fun) {
        auto tmp = std::unique_ptr<BaseFun>(new Fun<Ret, Args...>{_l, name, fun});
        _funs.insert(std::make_pair(name, std::move(tmp)));
    }

    friend std::ostream &operator<<(std::ostream &os, const State &state);
};
}