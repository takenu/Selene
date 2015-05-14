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

#include <tuple>
#include "Selector.h"

namespace sel {
template <typename... T>
class Tuple {
private:
    std::tuple<T&...> _tuple;
public:
    Tuple(T&... args) : _tuple(args...) {}

    void operator=(const sel::Selector &s) {
        _tuple = s.GetTuple<typename std::remove_reference<T>::type...>();
    }
};

template <typename... T>
Tuple<T&...> tie(T&... args) {
    return Tuple<T&...>(args...);
}
}
