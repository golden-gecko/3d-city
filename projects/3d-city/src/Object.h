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

#include <map>

#include "Convert.h"
#include "Coordinates.h"

class Map;

enum ObjectType
{
	OT_NONE				= (1 << 0),
	OT_SHAPE			= (1 << 1),
	OT_JUNCTION			= (1 << 2),
	OT_STREET			= (1 << 3),
	OT_BUILDING			= (1 << 4) | OT_SHAPE,
	OT_PLANT			= (1 << 5) | OT_SHAPE,
	OT_WATER_OBJECT		= (1 << 6) | OT_SHAPE,
	OT_ROUTE			= (1 << 7) | OT_SHAPE,

	OT_ALL = OT_BUILDING | OT_JUNCTION | OT_PLANT | OT_ROUTE | OT_STREET | OT_WATER_OBJECT
};

class Object
{
public:
	virtual ~Object()
	{
	}

	unsigned int getID() const
	{
		return mID;
	}
	const std::string& getName() const
	{
		return mName;
	}
	const std::string& getDescription() const
	{
		return mDescription;
	}
	Map* getMap() const
	{
		return mMap;
	}
	ObjectType getType() const
	{
		return mType;
	}

	void setName(const std::string& name)
	{
		mName = name;
	}
	void setDescription(const std::string& description)
	{
		mDescription = description;
	}

	virtual Point3D getPosition() const = 0;

	bool is(ObjectType type) const
	{
		return mType == type ? true : false;
	}
protected:
	unsigned int	mID;
	std::string		mName;
	std::string		mDescription;

	Map*			mMap;
	ObjectType		mType;

	Object(Map* map, ObjectType type, unsigned int id) : mMap(map), mType(type), mID(id)
	{
	}

	Object(const Object& object)
	{
	}
};

typedef std::map<unsigned int, Object*> ObjectList;
