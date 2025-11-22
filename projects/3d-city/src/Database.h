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

#include "Connection.h"
#include "Map.h"

class Database
{
	friend Map;
	friend Object;
public:
	Database(unsigned int id, const std::string& name, Connection* connection) : mID(id),
		mName(name), mConnection(connection), mLastMapID(0), mLastObjectID(0)
	{
		connection->setDatabase(this);
	}

	virtual ~Database()
	{
		for (MapList::const_iterator i = mMaps.begin(); i != mMaps.end(); ++i)
		{
			delete i->second;
		}

		delete mConnection;
	}

	Map* createMap()
	{
		mLastMapID++;

		return mMaps[mLastMapID] = new Map(this, mLastMapID);
	}
	Map* createMap(unsigned int id)
	{
		if (id > mLastMapID)
		{
			mLastMapID = id;
		}

		return mMaps[id] = new Map(this, id);
	}
	Map* getMap(unsigned int mapID) const
	{
		MapList::const_iterator i = mMaps.find(mapID);

		if (i != mMaps.end())
		{
			return i->second;
		}

		return NULL;
	}
	void destroyMap(unsigned int mapID)
	{
		MapList::iterator i = mMaps.find(mapID);

		if (i != mMaps.end())
		{
			delete i->second;
			mMaps.erase(i);
		}
	}

	Object* getObject(unsigned int mapID, unsigned int objectID) const
	{
		if (Map* map = getMap(mapID))
		{
			return map->getObject(objectID);
		}

		return NULL;
	}

	//void destroyObject(unsigned int mapID, unsigned int objectID)
	//{
	//	if (Map* map = getMap(mapID))
	//	{
	//		map->destroyObject(objectID);
	//	}
	//}

	//Building* createBuilding(unsigned int mapID, const std::string& name)
	//{
	//	if (Map* map = getMap(mapID))
	//	{
	//		return map->createBuilding(name);
	//	}

	//	return Building*(NULL);
	//}

	unsigned int getID() const
	{
		return mID;
	}
	const std::string& getName() const
	{
		return mName;
	}
	Connection* getConnection() const
	{
		return mConnection;
	}
	unsigned int getLastID() const
	{
		return mLastMapID;
	}
	const MapList& getMaps() const
	{
		return mMaps;
	}

	void setName(const std::string& name)
	{
		mName = name;
	}

	bool select()
	{
		if (mConnection->select())
		{
			MapList::const_iterator i = mMaps.begin();
			for (; i != mMaps.end(); ++i)
			{
				// Update data.
				ObjectList::const_iterator j = i->second->getObjects().begin();
				for (; j != i->second->getObjects().end(); ++j)
				{
					// Update junctions.
					if (Junction* junction = dynamic_cast<Junction*>(j->second))
					{
						junction->updateRoutes();
					}
				}
			}

			return true;
		}

		return false;
	}
protected:
	unsigned int	mID;
	std::string		mName;
	Connection*		mConnection;

	unsigned int	mLastMapID;
	unsigned int	mLastObjectID;

	MapList			mMaps;

	unsigned int getLastObjectID() const
	{
		return mLastObjectID;
	}

	void setLastObjectID(unsigned int id)
	{
		mLastObjectID = id;
	}
};

typedef std::map<unsigned int, Database*> DatabaseList;
