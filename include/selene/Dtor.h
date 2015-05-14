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

namespace sel {

template <typename T>
class Dtor : public BaseFun {
private:
    std::string _metatable_name;
public:
    Dtor(lua_State *l,
         const std::string &metatable_name)
        : _metatable_name(metatable_name) {
        lua_pushlightuserdata(l, (void *)(this));
        lua_pushcclosure(l, &detail::_lua_dispatcher, 1);
        lua_setfield(l, -2, "__gc");
    }

    int Apply(lua_State *l) {
        T *t = (T *)luaL_checkudata(l, 1, _metatable_name.c_str());
        t->~T();
        return 0;
    }
};
}
