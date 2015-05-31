# Selene

This fork of [Selene](https://github.com/jeremyong/Selene) is tailored towards use by the Chathran project and
its related repositories (e.g. Chathran Strata). Its functionality is
not equal to the original functionality (even though it is not very
significantly changed).

This fork is intended solely for use as a header library for Chathran
repositories. Copyright resides with the original author. Please refer
to LICENSE for the terms under which Selene is available.

## Usage

Include as '<code>#include \<selene.h\></code>'. Include before including Lua itself,
especially when using Lua 5.3, since in that case we need to enable a
compatibility mode for handling unsigned ints.
