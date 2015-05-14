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

#include "ObjFun.h"
#include <functional>
#include <memory>
#include "State.h"
#include <string>
#include <utility>
#include <vector>

namespace sel {
struct BaseObj {
    virtual ~BaseObj() {}
};
template <typename T, typename... Members>
class Obj : public BaseObj {
private:
    std::vector<std::unique_ptr<BaseFun>> _funs;

    template <typename M>
    void _register_member(lua_State *state,
                          T *t,
                          const char *member_name,
                          M T::*member) {
        _register_member(state, t, member_name, member,
                         typename std::is_const<M>::type{});
    }

    template <typename M>
    void _register_member(lua_State *state,
                          T *t,
                          const char *member_name,
                          M T::*member,
                          std::false_type) {
        std::function<M()> lambda_get = [t, member]() {
            return t->*member;
        };
        _funs.emplace_back(
            new ObjFun<1, M>{state, std::string{member_name}, lambda_get});

        std::function<void(M)> lambda_set = [t, member](M value) {
            t->*member = value;
        };
        _funs.emplace_back(
            new ObjFun<0, void, M>
            {state, std::string{"set_"} + member_name, lambda_set});
    }

    template <typename M>
    void _register_member(lua_State *state,
                          T *t,
                          const char *member_name,
                          M T::*member,
                          std::true_type) {
        std::function<M()> lambda_get = [t, member]() {
            return t->*member;
        };
        _funs.emplace_back(
            new ObjFun<1, M>{state, std::string{member_name}, lambda_get});
    }

    template <typename Ret, typename... Args>
    void _register_member(lua_State *state,
                          T *t,
                          const char *fun_name,
                          Ret(T::*fun)(Args&&...)) {
        std::function<Ret(Args&&...)> lambda = [t, fun](Args&&... args) {
            return (t->*fun)(std::forward<Args>(args)...);
        };
        constexpr int arity = detail::_arity<Ret>::value;
        _funs.emplace_back(
            new ObjFun<arity, Ret, Args...>
            {state, std::string(fun_name), lambda});
    }

    template <typename Ret, typename... Args>
    void _register_member(lua_State *state,
                          T *t,
                          const char *fun_name,
                          Ret(T::*fun)(Args...)) {
        std::function<Ret(Args...)> lambda = [t, fun](Args... args) {
            return (t->*fun)(args...);
        };
        constexpr int arity = detail::_arity<Ret>::value;
        _funs.emplace_back(
            new ObjFun<arity, Ret, Args...>
            {state, std::string(fun_name), lambda});
    }

    void _register_members(lua_State *, T *) {}

    template <typename M, typename... Ms>
    void _register_members(lua_State *state, T *t,
                           const char *name,
                           M member,
                           Ms... members) {
        _register_member(state, t, name, member);
        _register_members(state, t, members...);
    }
public:
    Obj(lua_State *state, T *t, Members... members) {
        lua_createtable(state, 0, sizeof...(Members));
        _register_members(state, t, members...);
    }
};
}
