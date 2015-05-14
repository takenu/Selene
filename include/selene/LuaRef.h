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

#include <memory>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace sel {
namespace detail {
class LuaRefDeleter {
private:
    lua_State *_state;
public:
    LuaRefDeleter(lua_State *state) : _state{state} {}
    void operator()(int *ref) const {
        luaL_unref(_state, LUA_REGISTRYINDEX, *ref);
        delete ref;
    }
};
}
class LuaRef {
private:
    std::shared_ptr<int> _ref;
public:
    LuaRef(lua_State *state, int ref)
        : _ref(new int{ref}, detail::LuaRefDeleter{state}) {}

    void Push(lua_State *state) {
        lua_rawgeti(state, LUA_REGISTRYINDEX, *_ref);
    }
};
}
