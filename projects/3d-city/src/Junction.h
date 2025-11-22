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

#include <set>

#include "Object.h"
#include "Route.h"

class Junction : public Object
{
public:
	Junction(Map* map, unsigned int id) : Object(map, OT_JUNCTION, id)
	{
		mName = "Route Junction #" + Convert::ToString(mID);
	}

	void addRoute(Route* route)
	{
		if (route)
		{
			mRoutes.insert(std::make_pair(route->getID(), route));
		}
	}
	void addRoute(unsigned int id)
	{
		mRoutes[id] = NULL;
	}
	void removeRoute(Route* route)
	{
		if (route)
		{
			removeRoute(route->getID());
		}
	}
	void removeRoute(unsigned int id)
	{
		RouteList::iterator i = mRoutes.find(id);

		if (i != mRoutes.end())
		{
			mRoutes.erase(i);
		}
	}
	const Point3D& getCenter() const
	{
		return mCenter;
	}
	unsigned short getRouteCount() const
	{
		return (unsigned short)mRoutes.size();
	}

	void updateRoutes();

	const RouteList& getRoutes() const
	{
		return mRoutes;
	}

	void setCenter(const Point3D& center)
	{
		mCenter = center;
	}
	void setRoutes(const RouteList& routes)
	{
		mRoutes = routes;
	}

	virtual Point3D getPosition() const
	{
		return mCenter;
	}
protected:
	Point3D		mCenter;
	RouteList	mRoutes;
};
