/* This file is part of Selene. It is licensed under the zlib license as follows:

Copyright (c) 2013 Jeremy Ong

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution. */

#pragma once

#include <functional>
#include "LuaRef.h"
#include <memory>
#include "primitives.h"
#include "util.h"

namespace sel {
/*
 * Similar to an std::function but refers to a lua function
 */
template <class>
class function {};

template <typename R, typename... Args>
class function<R(Args...)> {
private:
    LuaRef _ref;
    lua_State *_state;
public:
    function(int ref, lua_State *state) : _ref(state, ref), _state(state) {}

    R operator()(Args... args) {
        int handler_index = SetErrorHandler(_state);
        _ref.Push(_state);
        detail::_push_n(_state, args...);
        constexpr int num_args = sizeof...(Args);
        lua_pcall(_state, num_args, 1, handler_index);
        lua_remove(_state, handler_index);
        R ret = detail::_pop(detail::_id<R>{}, _state);
        lua_settop(_state, 0);
        return ret;
    }

    void Push(lua_State *state) {
        _ref.Push(state);
    }
};

template <typename... Args>
class function<void(Args...)> {
private:
    LuaRef _ref;
    lua_State *_state;
public:
    function(int ref, lua_State *state) : _ref(state, ref), _state(state) {}

    void operator()(Args... args) {
        int handler_index = SetErrorHandler(_state);
        _ref.Push(_state);
        detail::_push_n(_state, args...);
        constexpr int num_args = sizeof...(Args);
        lua_pcall(_state, num_args, 1, handler_index);
        lua_remove(_state, handler_index);
        lua_settop(_state, 0);
    }

    void Push(lua_State *state) {
        _ref.Push(state);
    }
};

// Specialization for multireturn types
template <typename... R, typename... Args>
class function<std::tuple<R...>(Args...)> {
private:
    LuaRef _ref;
    lua_State *_state;
public:
    function(int ref, lua_State *state) : _ref(state, ref), _state(state) {}

    std::tuple<R...> operator()(Args... args) {
        int handler_index = SetErrorHandler(_state);
        _ref.Push(_state);
        detail::_push_n(_state, args...);
        constexpr int num_args = sizeof...(Args);
        constexpr int num_ret = sizeof...(R);
        lua_pcall(_state, num_args, num_ret, handler_index);
        lua_remove(_state, handler_index);
        return detail::_pop_n_reset<R...>(_state);
    }

    void Push(lua_State *state) {
        _ref.Push(state);
    }
};
}
