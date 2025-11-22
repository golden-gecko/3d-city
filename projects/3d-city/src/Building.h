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

class Building : public Shape
{
public:
	Building(Map* map, unsigned int id) : Shape(map, OT_BUILDING, id),
		mNumber(0), mHeight(30.0f)
	{
		mName = "Building #" + Convert::ToString(mID);
	}

	unsigned short getNumber() const
	{
		return mNumber;
	}
	float getHeight() const
	{
		return mHeight;
	}
	const std::string& getTexture() const
	{
		return mTexture;
	}

	void setNumber(unsigned short number)
	{
		mNumber = number;
	}
	void setHeight(float height)
	{
		mHeight = height;
	}
	void setTexture(const std::string& texture)
	{
		mTexture = texture;
	}
protected:
	unsigned short	mNumber;
	float			mHeight;
	std::string		mTexture;
};
