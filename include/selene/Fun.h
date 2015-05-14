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

#include "BaseFun.h"
#include "MetatableRegistry.h"
#include <string>

namespace sel {
template <int N, typename Ret, typename... Args>
class Fun : public BaseFun {
private:
    using _fun_type = std::function<Ret(Args...)>;
    _fun_type _fun;
    MetatableRegistry &_meta_registry;

public:
    Fun(lua_State *&l,
        MetatableRegistry &meta_registry,
        Ret(*fun)(Args...))
        : Fun(l, meta_registry, _fun_type{fun}) {}

    Fun(lua_State *&l,
        MetatableRegistry &meta_registry,
        _fun_type fun) : _fun(fun), _meta_registry(meta_registry) {
        lua_pushlightuserdata(l, (void *)static_cast<BaseFun *>(this));
        lua_pushcclosure(l, &detail::_lua_dispatcher, 1);
    }

    // Each application of a function receives a new Lua context so
    // this argument is necessary.
    int Apply(lua_State *l) override {
        std::tuple<Args...> args = detail::_get_args<Args...>(l);
        Ret value = detail::_lift(_fun, args);
        detail::_push(l, _meta_registry, std::forward<Ret>(value));
        return N;
    }

};

template <typename... Args>
class Fun<0, void, Args...> : public BaseFun {
private:
    using _fun_type = std::function<void(Args...)>;
    _fun_type _fun;

public:
    Fun(lua_State *&l,
        MetatableRegistry &dummy,
        void(*fun)(Args...))
        : Fun(l, dummy, _fun_type{fun}) {}

    Fun(lua_State *&l,
        MetatableRegistry &,
        _fun_type fun) : _fun(fun) {
        lua_pushlightuserdata(l, (void *)static_cast<BaseFun *>(this));
        lua_pushcclosure(l, &detail::_lua_dispatcher, 1);
    }

    // Each application of a function receives a new Lua context so
    // this argument is necessary.
    int Apply(lua_State *l) {
        std::tuple<Args...> args = detail::_get_args<Args...>(l);
        detail::_lift(_fun, args);
        return 0;
    }
};
}
