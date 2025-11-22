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

#include "Dependencies.h"
#include "City.h"
#include "Log.h"
#include "Render.h"

IMPLEMENT_SINGLETON(City)

Database* City::openDatabase(const std::string& name,
	const std::string& type, const std::string& params)
{
	try
	{
		PluginList::const_iterator i = mPlugins.find(type);

		if (i != mPlugins.end())
		{
			mLastID++;

			return mDatabases[mLastID] = new Database(mLastID, name,
				i->second->createInstance(params));
		}
	}
	catch (...)
	{
		Log::getSingleton().error("Controller::openDatabase - error.");
	}

	return NULL;
}

void City::closeDatabase(unsigned int databaseID)
{
	try
	{
		DatabaseList::iterator db = mDatabases.find(databaseID);

		if (db != mDatabases.end())
		{
			// Destroy views.
			MapList::const_iterator map = db->second->getMaps().begin();

			for (; map != db->second->getMaps().end(); ++map)
			{
				if (Render::getSingleton().getActiveMapView())
				{
					if (Render::getSingleton().getActiveMapView()->getMap() == map->second)
					{
						Render::getSingleton().unselect();
					}
				}

				destroyViews(map->second);
			}

			// Close database.
			delete db->second;
			mDatabases.erase(db);
		}
	}
	catch (...)
	{
		Log::getSingleton().error("Controller::closeDatabase error.");
	}
}

MapView* City::createView(Map* map, unsigned int objects)
{
	if (map)
	{
		mLastViewID++;

		MapView* mapView = new MapView(mLastViewID, map, objects);

		mMapViews.insert(std::make_pair(map, mapView));

		return mapView;
	}

	return NULL;
}

MapView* City::getMapView(unsigned int mapViewID) const
{
	MapViewList::const_iterator i = mMapViews.begin();

	for (; i != mMapViews.end(); ++i)
	{
		if (mapViewID == i->second->getID())
		{
			return i->second;
		}
	}

	return NULL;
}

ObjectView* City::getObjectView(unsigned int mapViewID, unsigned int objectViewID) const
{
	if (MapView* mapView = getMapView(mapViewID))
	{
		return mapView->getObjectView(objectViewID);
	}

	return NULL;
}

void City::updateViews(Map* map) const
{
	MapViewList::const_iterator i = mMapViews.lower_bound(map);

	for (; i != mMapViews.upper_bound(map); ++i)
	{
		i->second->update();
	}
}

void City::updateViews(Object* object) const
{
	MapViewList::const_iterator i = mMapViews.lower_bound(object->getMap());

	for (; i != mMapViews.upper_bound(object->getMap()); ++i)
	{
		ObjectViewList::const_iterator j = i->second->getObjectViews().lower_bound(object);

		for (; j != i->second->getObjectViews().upper_bound(object); ++j)
		{
			j->second->update();
		}
	}
}

void City::destroyViews(Map* map)
{
	MapViewList::const_iterator i = mMapViews.lower_bound(map);

	for (; i != mMapViews.upper_bound(map); ++i)
	{
		delete i->second;
	}

	mMapViews.erase(
		mMapViews.lower_bound(map),
		mMapViews.upper_bound(map));
}

void City::destroyView(MapView* view)
{
	MapViewList::iterator i = mMapViews.begin();

	for (; i != mMapViews.end(); ++i)
	{
		if (i->second == view)
		{
			mMapViews.erase(i);

			break;
		}
	}
}

void City::destroyViews(Object* object)
{
	MapViewList::const_iterator i = mMapViews.lower_bound(object->getMap());
	for (; i != mMapViews.upper_bound(object->getMap()); ++i)
	{
		ObjectViewList::const_iterator j = i->second->getObjectViews().lower_bound(object);
		for (; j != i->second->getObjectViews().upper_bound(object); ++j)
		{
			delete j->second;
		}

		i->second->getObjectViews().erase(
			i->second->getObjectViews().lower_bound(object),
			i->second->getObjectViews().upper_bound(object));
	}
}
