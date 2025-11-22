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

#include "Shape.h"

class Junction;

typedef std::set<Junction*> JunctionList;

class Route : public Shape
{
public:
	Route(Map* map, unsigned int id) : Shape(map, OT_ROUTE, id), mAvarageSpeed(45.0f), mMaxSpeed(50), mWidth(10.0f)
	{
		mName = "Route #" + Convert::ToString(mID);
	}

	float getAvarageSpeed() const
	{
		return mAvarageSpeed;
	}
	unsigned short getMaxSpeed() const
	{
		return mMaxSpeed;
	}
	float getWidth() const
	{
		return mWidth;
	}
	bool getHasLights() const
	{
		return mHasLights;
	}

	void setAvarageSpeed(float speed)
	{
		mAvarageSpeed = speed;
	}
	void setMaxSpeed(unsigned short speed)
	{
		mMaxSpeed = speed;
	}
	void setWidth(float width)
	{
		mWidth = width;
	}
	void setHasLights(bool lights)
	{
		mHasLights = lights;
	}

	Point3D getNearest(const Point3D& p)
	{
		if (mPoints.size() == 1)
		{
		}
		else if (mPoints.size() > 1)
		{
		}

		return Point3D();
	}

	void attachJunction(Junction* junction)
	{
		mJunctions.insert(junction);
	}
	JunctionList& getJunctions()
	{
		return mJunctions;
	}
protected:
	float			mAvarageSpeed;
	unsigned short	mMaxSpeed;
	float			mWidth;
	bool			mHasLights;
	JunctionList	mJunctions;
};

typedef std::map<unsigned int, Route*> RouteList;
