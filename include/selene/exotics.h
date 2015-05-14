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

#include "function.h"

/*
 * Extends manipulation of primitives on the stack with more exotic
 * types
 */

namespace sel {
namespace detail {

template <typename R, typename...Args>
inline sel::function<R(Args...)> _check_get(_id<sel::function<R(Args...)>>,
                                            lua_State *l, const int index) {
    lua_pushvalue(l, index);
    return sel::function<R(Args...)>{luaL_ref(l, LUA_REGISTRYINDEX), l};
}

template <typename R, typename... Args>
inline sel::function<R(Args...)> _get(_id<sel::function<R(Args...)>> id,
                                      lua_State *l, const int index) {
    return _check_get(id, l, index);
}

template <typename R, typename... Args>
inline void _push(lua_State *l, sel::function<R(Args...)> fun) {
    fun.Push(l);
}

}
}
