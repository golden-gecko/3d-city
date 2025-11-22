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

#include "Plant.h"
#include "ShapeView.h"

class PlantView : public ShapeView
{
public:
	PlantView(unsigned int id, MapView* mapView, Plant* building) : ShapeView(id,
		mapView, building), mPlant(building), mShowShape(false), mShowTrees(true)
	{
		load();
	}

	virtual ~PlantView()
	{
		unload();
	}

	Plant* getPlant() const
	{
		return mPlant;
	}

	virtual bool load();
	virtual void unload();
	virtual bool update();
public:
	bool getShowShape() const
	{
		return mShowShape;
	}
	bool getShowTrees() const
	{
		return mShowTrees;
	}
protected:
	Plant* mPlant;

	/// Show plant shape (polygon).
	bool mShowShape;

	/// Show tree models.
	bool mShowTrees;

	/// Tree entities.
	Ogre::Entity* mEntities[3];

	/// Static geometry for tree models.
	Ogre::StaticGeometry* mStaticGeometry;
};
