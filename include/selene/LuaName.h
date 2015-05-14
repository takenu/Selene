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

#include <string>

extern "C" {
#include <lua.h>
}

namespace sel {
/*
 * Used to ensure that globals set in Lua are deleted (set to nil) in
 * the event that the parent object is destroyed. Checks if the Lua
 * context in which it was registered still exists before doing so.
 * Prevents copying but permits moving.
 */
class LuaName {
private:
    lua_State **_l;
    std::string _name;

public:
    LuaName(lua_State *&l, const std::string &name) : _l(&l), _name(name) {}
    LuaName(const LuaName &other) = delete;
    LuaName(LuaName &&other)
        : _l(other._l),
          _name(other._name) {
        *other._l = nullptr;
    }
    ~LuaName() {
        if (_l != nullptr && *_l != nullptr) {
            lua_pushnil(*_l);
            lua_setglobal(*_l, _name.c_str());
        }
    }

    void Register() {
        if (_l != nullptr && *_l != nullptr) {
            lua_setglobal(*_l, _name.c_str());
        }
    }

    std::string GetName() const {return _name;}
    lua_State *GetState() {return *_l;}
};
}
