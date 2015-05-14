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

#include <iostream>
#include <memory>
#include <typeinfo>
#include <unordered_map>

namespace sel {
class MetatableRegistry {
private:
    using TypeID = std::reference_wrapper<const std::type_info>;
    struct Hasher {
        std::size_t operator()(TypeID code) const {
            return code.get().hash_code();
        }
    };
    struct EqualTo {
        bool operator()(TypeID lhs, TypeID rhs) const {
            return lhs.get() == rhs.get();
        }
    };
    std::unordered_map<TypeID, const std::string*, Hasher, EqualTo> _metatables;

public:
    MetatableRegistry() {}

    inline void Insert(TypeID type, const std::string& name) {
        _metatables[type] = &name;
    }

    inline void Erase(TypeID type) {
        _metatables.erase(type);
    }

    inline const std::string* Find(TypeID type) {
        auto it = _metatables.find(type);
        if (it == _metatables.end()) return nullptr;
        return it->second;
    }
};
}
