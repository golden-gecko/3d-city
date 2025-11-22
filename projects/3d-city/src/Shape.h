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

#include "Object.h"

class Shape : public Object
{
public:
	Shape(Map* map, ObjectType type, unsigned int id) : Object(map, type, id)
	{
	}

	virtual Point3D getPosition() const
	{
		return mPoints.size() ? mPoints.front() : Point3D();
	}

	const Point3D& getPoint(unsigned int n) const
	{
		return mPoints[n];
	}
	const PointList& getPoints() const
	{
		return mPoints;
	}
	unsigned int getPointCount() const
	{
		return (unsigned int)mPoints.size();
	}

	void setPoint(unsigned int n, const Point3D& point)
	{
		if (n >= mPoints.size())
		{
			mPoints.resize(n + 1);
		}

		mPoints[n] = point;
	}
	void setPoints(const PointList& points)
	{
		mPoints = points;
	}

	void clearPoints()
	{
		mPoints.clear();
	}
	void deletePoint(unsigned int n)
	{
		unsigned int c = 0;

		for (PointList::iterator i = mPoints.begin(); i != mPoints.end(); ++i)
		{
			if (c == n)
			{
				mPoints.erase(i);

				break;
			}

			++c;
		}
	}
	void insertPoint(const Point3D& point)
	{
		if (mPoints.size() < 3)
		{
			mPoints.push_back(point);
		}
		else
		{
			PointList::iterator nearest = mPoints.begin();

			for (PointList::iterator i = mPoints.begin(); i != mPoints.end(); ++i)
			{
				if ((*i) > (*nearest))
				{
					nearest = i;
				}
			}

			mPoints.insert(nearest, point);
		}
	}
	void movePoint(unsigned int n, const Point3D& vector)
	{
		if (n < mPoints.size())
		{
			mPoints[n] += vector;
		}
	}
	virtual void move(const Point3D& vector)
	{
		for (PointList::iterator i = mPoints.begin(); i != mPoints.end(); ++i)
		{
			(*i) += vector;
		}
	}
protected:
	PointList mPoints;
};
