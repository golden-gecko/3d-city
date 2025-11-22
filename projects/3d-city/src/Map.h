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

#include <fstream>

#include "Building.h"
#include "Junction.h"
#include "Plant.h"
#include "Route.h"
#include "Street.h"
#include "WaterObject.h"

class Database;

// -----------------------------------------------------------------------------

struct Size
{
	unsigned int x;
	unsigned int z;

	Size(unsigned int x = 5000, unsigned int z = 5000) : x(x), z(z)
	{
	}
};

// -----------------------------------------------------------------------------

struct TerrainSize
{
	unsigned int x;
	unsigned int z;

	TerrainSize(unsigned int x = 129, unsigned int z = 129) : x(x), z(z)
	{
	}
};

typedef std::vector<float> TerrainData;

// -----------------------------------------------------------------------------

class Map
{
public:
	Map(Database* database, unsigned int id) : mDatabase(database), mID(id)
	{
		mName = "Map #" + Convert::ToString(mID);
		mTerrainData.resize(mTerrainSize.x * mTerrainSize.z);
	}

	virtual ~Map()
	{
		for (ObjectList::const_iterator i = mObjects.begin(); i != mObjects.end(); ++i)
		{
			delete i->second;
		}
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
	const Size& getSize() const
	{
		return mSize;
	}
	const TerrainSize& getTerrainSize() const
	{
		return mTerrainSize;
	}
	const TerrainData& getTerrainData() const
	{
		return mTerrainData;
	}
	const Point& getCenter() const
	{
		return mCenter;
	}
	float getZoom() const
	{
		return mZoom;
	}
	const std::string& getTexture() const
	{
		return mTexture;
	}
	Database* getDatabase() const
	{
		return mDatabase;
	}
	const ObjectList& getObjects() const
	{
		return mObjects;
	}

	void setName(const std::string& name)
	{
		mName = name;
	}
	void setDescription(const std::string& description)
	{
		mDescription = description;
	}
	void setSize(const Size& size)
	{
		mSize = size;
	}
	void setTerrainSize(const TerrainSize& size)
	{
		mTerrainSize = size;
	}
	void setTerrainData(const TerrainData& data)
	{
		mTerrainData = data;
	}
	void setCenter(const Point& center)
	{
		mCenter = center;
	}
	void setZoom(float zoom)
	{
		mZoom = zoom;
	}
	void setTexture(const std::string& texture)
	{
		mTexture = texture;
	}

	template <class T>
	T* create()
	{
		mDatabase->setLastObjectID(mDatabase->getLastObjectID() + 1);

		T* object = new T(this, mDatabase->getLastObjectID());

		mObjects[mDatabase->getLastObjectID()] = object;

		return object;
	}
	template <class T>
	T* create(unsigned int id)
	{
		if (id > mDatabase->getLastObjectID())
		{
			mDatabase->setLastObjectID(id);
		}

		T* object = new T(this, id);

		mObjects[id] = object;

		return object;
	}
	Object* getObject(unsigned int objectID) const
	{
		ObjectList::const_iterator i = mObjects.find(objectID);

		if (i != mObjects.end())
		{
			return i->second;
		}

		return NULL;
	}
	template <class T>
	T* get(unsigned int objectID) const
	{
		return dynamic_cast<T*>(getObject(objectID));
	}

	void destroyObject(unsigned int objectID)
	{
		ObjectList::iterator i = mObjects.find(objectID);

		if (i != mObjects.end())
		{
			delete i->second;
			mObjects.erase(i);
		}
	}
public:
	unsigned int	mID;
	std::string		mName;
	std::string		mDescription;
	Size			mSize;
	TerrainSize		mTerrainSize;
	TerrainData		mTerrainData;
	Point			mCenter;
	float			mZoom;
	std::string		mTexture;

	Database*		mDatabase;
	ObjectList		mObjects;
};

typedef std::map<unsigned int, Map*> MapList;
