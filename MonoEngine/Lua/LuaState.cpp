//
//  LuaState.cpp
//  Mono1
//
//  Created by Niblit on 2012-04-23.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "LuaState.h"
#include <stdexcept>

extern "C"
{
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
}


using namespace lua;

LuaState::LuaState()
{
    L = lua_open();
    luaL_openlibs(L);
}

LuaState::LuaState(const std::string& file)
{
    L = lua_open();
    luaL_openlibs(L);

    const bool failed = luaL_dofile(L, file.c_str());
    if(failed)
        throw std::runtime_error("Unable to load file");
}

LuaState::~LuaState()
{
    lua_close(L);
}

LuaState::operator lua_State*()
{
    return L;
}



