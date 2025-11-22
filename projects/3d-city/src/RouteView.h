//------------------------------------------------------------------------------
//
//  3d-city v0.1 alpha
//  Copyright (C) 2008/2009 Wojciech Holisz (wholisz@wp.pl)
//
//  Niniejszy program jest wolnym oprogramowaniem; mo¿esz go 
//  rozprowadzaæ dalej i/lub modyfikowaæ na warunkach Powszechnej
//  Licencji Publicznej GNU, wydanej przez Fundacjê Wolnego
//  Oprogramowania - wed³ug wersji 2-giej tej Licencji lub którejœ
//  z póŸniejszych wersji. 
//
//  Niniejszy program rozpowszechniany jest z nadziej¹, i¿ bêdzie on 
//  u¿yteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyœlnej 
//  gwarancji PRZYDATNOŒCI HANDLOWEJ albo PRZYDATNOŒCI DO OKREŒLONYCH 
//  ZASTOSOWAÑ. W celu uzyskania bli¿szych informacji - Powszechna 
//  Licencja Publiczna GNU. 
//
//  Z pewnoœci¹ wraz z niniejszym programem otrzyma³eœ te¿ egzemplarz 
//  Powszechnej Licencji Publicznej GNU (GNU General Public License);
//  jeœli nie - napisz do Free Software Foundation, Inc., 675 Mass Ave,
//  Cambridge, MA 02139, USA.
//
//------------------------------------------------------------------------------

#pragma once

#include "Route.h"
#include "ShapeView.h"

class RouteView : public ShapeView
{
public:
	RouteView(unsigned int id, MapView* mapView, Route* route) : ShapeView(id,
		mapView, route),  mRoute(route), mSegments(25)
	{
		load();
	}

	virtual ~RouteView()
	{
		unload();
	}

	Route* getRoute() const
	{
		return mRoute;
	}
	unsigned short getSegments() const
	{
		return mSegments;
	}

	void setSegments(unsigned short segments)
	{
		mSegments = segments;
	}

	virtual bool load();
	virtual void unload();
	virtual bool update();

	Ogre::Vector3* getNearest(const Point3D& center)
	{
		Ogre::Vector3 c(center.x, center.y, center.z);

		if (mFirst[1].squaredDistance(c) < mLast[1].squaredDistance(c))
		{
			return mFirst;
		}

		return mLast;
	}
protected:
	Route*					mRoute;
	unsigned short			mSegments;

	Ogre::Entity*			mLightEntity;
	Ogre::StaticGeometry*	mLightStatic;

	Ogre::Vector3 mFirst[3];
	Ogre::Vector3 mLast[3];
};
