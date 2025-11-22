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

#include "Junction.h"
#include "ShapeView.h"

class JunctionView : public ObjectView
{
public:
	JunctionView(unsigned int id, MapView* mapView, Junction* junction) : ObjectView(id,
		mapView, junction),  mJunction(junction)
	{
		load();
	}

	virtual ~JunctionView()
	{
		unload();
	}

	Junction* getJunction() const
	{
		return mJunction;
	}

	virtual bool load();
	virtual void unload();
	virtual bool update();
protected:
	Junction*				mJunction;

	Ogre::Material*			mMaterial;
	Ogre::Mesh*				mMesh;
	Ogre::Entity*			mEntity;
	Ogre::SceneNode*		mSceneNode;
};
