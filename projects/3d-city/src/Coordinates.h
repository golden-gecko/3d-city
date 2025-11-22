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

#include <vector>

class Point
{
public:
	double latitude;
	double longitude;
	double height;

	Point(double latitude = 0.0, double longitude = 0.0,
		double height = 0.0) : latitude(latitude), longitude(longitude), height(height)
	{
	}
};

class Point3D
{
public:
	float x, y, z;

	Point3D(float x = 0.0, float y = 0.0, float z = 0.0) : x(x), y(y), z(z)
	{
	}

	Point3D operator + (const Point3D& p) const
	{
		return Point3D(x + p.x, y + p.y, z + p.z);
	}

	Point3D operator - (const Point3D& p) const
	{
		return Point3D(x - p.x, y - p.y, z - p.z);
	}

	Point3D operator * (float s) const
	{
		return Point3D(x * s, y * s, z * s);
	}

	Point3D operator / (float s) const
	{
		return Point3D(x / s, y / s, z / s);
	}

	Point3D& operator += (const Point3D& p)
	{
		x += p.x;
		y += p.y;
		z += p.z;

		return *this;
	}

	Point3D& operator -= (const Point3D& p)
	{
		x -= p.x;
		y -= p.y;
		z -= p.z;

		return *this;
	}

	Point3D& operator *= (float s)
	{
		x *= s;
		y *= s;
		z *= s;

		return *this;
	}

	Point3D& operator /= (float s)
	{
		x /= s;
		y /= s;
		z /= s;

		return *this;
	}

	bool operator > (const Point3D& p) const
	{
		return x > p.x && y > p.y && z > p.z;
	}

	bool operator < (const Point3D& p) const
	{
		return x < p.x && y < p.y && z < p.z;
	}

	bool operator >= (const Point3D& p) const
	{
		return x >= p.x && y >= p.y && z >= p.z;
	}

	bool operator <= (const Point3D& p) const
	{
		return x <= p.x && y <= p.y && z <= p.z;
	}
};

typedef std::vector<Point3D> PointList;
