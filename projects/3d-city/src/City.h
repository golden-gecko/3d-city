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

#include "Dependencies.h"
#include "BuildingView.h"
#include "Database.h"
#include "MapView.h"
#include "RouteView.h"
#include "Plugin.h"

typedef std::multimap<Map*, MapView*> MapViewList;

class City : public Ogre::Singleton<City>
{
public:
	City() : mLastID(0), mLastViewID(0)
	{
		WIN32_FIND_DATA wfd;
		HANDLE hFind = FindFirstFile("3d-city_*.dll", &wfd);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				Plugin* p = new Plugin(wfd.cFileName);

				if (p->isLoaded())
				{
					mPlugins[p->getConnectionName()] = p;
				}
				else
				{
					delete p;
				}
			}
			while (FindNextFile(hFind, &wfd));
		}
	}

	virtual ~City()
	{
		MapViewList::const_iterator i = mMapViews.begin();

		for (; i != mMapViews.end(); ++i)
		{
			delete i->second;
		}

		DatabaseList::const_iterator j = mDatabases.begin();

		for (; j != mDatabases.end(); ++j)
		{
	//		j->second->getConnection()->update();

			delete j->second;
		}

		PluginList::const_iterator k = mPlugins.begin();

		for (; k != mPlugins.end(); ++k)
		{
			delete k->second;
		}
	}

	Database* openDatabase(const std::string& name,
		const std::string& type, const std::string& params = "");

	Database* getDatabase(unsigned int databaseID) const
	{
		DatabaseList::const_iterator i = mDatabases.find(databaseID);

		if (i != mDatabases.end())
		{
			return i->second;
		}

		return NULL;
	}
	Map* getMap(unsigned int databaseID, unsigned int mapID) const
	{
		if (Database* database = getDatabase(databaseID))
		{
			return database->getMap(mapID);
		}

		return NULL;
	}
	Object* getObject(unsigned int databaseID, unsigned int mapID, unsigned int objectID) const
	{
		if (Database* database = getDatabase(databaseID))
		{
			return database->getObject(mapID, objectID);
		}

		return NULL;
	}
	void closeDatabase(unsigned int databaseID);

	MapView* createView(Map* map, unsigned int objects = OT_NONE);
	MapView* getMapView(unsigned int mapViewID) const;
	ObjectView* getObjectView(unsigned int mapViewID, unsigned int objectViewID) const;

	/// Updates all map views.
	void updateViews(Map* map) const;

	/// Updates all object views.
	void updateViews(Object* object) const;

	/// Destroys one map view.
	void destroyView(MapView* view);

	/// Destroys one object view.
	void destroyView(ObjectView* view);

	/// Destroys all map views.
	void destroyViews(Map* map);

	/// Destroys all object views.
	void destroyViews(Object* object);

	const PluginList& getPlugins() const
	{
		return mPlugins;
	}
	unsigned int getLastID() const
	{
		return mLastID;
	}
	const DatabaseList& getDatabases() const
	{
		return mDatabases;
	}
	unsigned int getLastViewID() const
	{
		return mLastViewID;
	}
	const MapViewList& getMapViews() const
	{
		return mMapViews;
	}

	void setLastViewID(unsigned int id)
	{
		mLastViewID = id;
	}
protected:
	PluginList		mPlugins;

	unsigned int	mLastID;
	DatabaseList		mDatabases;

	unsigned int	mLastViewID;
	MapViewList		mMapViews;
};
