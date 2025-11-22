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

#include "Database.h"
#include "MapView.h"
#include "Object.h"
#include "ObjectView.h"

enum IdentityType
{
	IT_NONE,
	IT_DATABASE,
	IT_MAP,
	IT_OBJECT,
	IT_MAP_VIEW,
	IT_OBJECT_VIEW
};

struct Identity
{
	IdentityType	type;

	unsigned int	database;
	unsigned int	map;
	unsigned int	object;
	unsigned int	mapView;
	unsigned int	objectView;

	Identity() : type(IT_NONE)
	{
	}
	Identity(Database* database) : type(IT_DATABASE), database(database->getID())
	{
	}
	Identity(Map* map) : type(IT_MAP), database(map->getDatabase()->getID()), map(map->getID())
	{
	}
	Identity(Object* object) : type(IT_OBJECT), database(object->getMap()->getDatabase()->getID()), map(object->getMap()->getID()), object(object->getID())
	{
	}
	Identity(MapView* mapView) : type(IT_MAP_VIEW), mapView(mapView->getID())
	{
	}
	Identity(ObjectView* objectView) : type(IT_OBJECT_VIEW), mapView(objectView->getMapView()->getID()), objectView(objectView->getID())
	{
	}

	Database* getDatabase() const
	{
		return type == IT_DATABASE ? City::getSingleton().getDatabase(database) : NULL;
	}
	Map* getMap() const
	{
		return type == IT_MAP ? City::getSingleton().getMap(database, map) : NULL;
	}
	Object* getObject() const
	{
		return type == IT_OBJECT ? City::getSingleton().getObject(database, map, object) : NULL;
	}
	MapView* getMapView() const
	{
		return type == IT_MAP_VIEW ? City::getSingleton().getMapView(mapView) : NULL;
	}
	ObjectView* getObjectView() const
	{
		return type == IT_OBJECT_VIEW ? City::getSingleton().getObjectView(mapView, objectView) : NULL;
	}
};
