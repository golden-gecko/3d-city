//------------------------------------------------------------------------------
//
//	3d-city v0.1 alpha (3D map view and editor)
//	Copyright (C) 2008/2009 Wojciech Holisz (wholisz@wp.pl)
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//------------------------------------------------------------------------------

#pragma once

#include <windows.h>

#include "Connection.h"

typedef Connection* (*CreateInstance)(const std::string&);
typedef const std::string& (*GetName)();

class Plugin
{
public:
	Plugin(const std::string& fileName);

	bool isLoaded() const
	{
		return mIsLoaded;
	}

	Connection* createInstance(const std::string& params) const
	{
		return mCreateInstance(params);
	}
	const std::string& getConnectionName() const
	{
		return mGetName();
	}
protected:
	bool			mIsLoaded;
	HMODULE			mLibrary;

	CreateInstance	mCreateInstance;
	GetName			mGetName;
};

typedef std::map<std::string, Plugin*> PluginList;
