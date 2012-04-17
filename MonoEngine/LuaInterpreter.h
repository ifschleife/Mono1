/*
 *  LuaInterpreter.h
 *  Monolith1
 *
 *  Created by Niblit on 2011-02-17.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include <string>
#include <tr1/functional>

class lua_State;

namespace mono
{
    typedef std::tr1::function<int (lua_State*)> LuaFunction;
    
	class LuaInterpreter
	{
	public:
		LuaInterpreter();
		~LuaInterpreter();
		
        void RegisterFunction(const std::string& name, const LuaFunction& func);
		int ExecuteFile(const std::string& file) const;
		int ExecuteString(const std::string& string) const;
		
	private:
	
		lua_State* mLuaState;
	};
    
}

