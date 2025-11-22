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

#include <wx/db.h>
#include <wx/wx.h>

#include "Database.h"
#include "Convert.h"

class OdbcConnection : public Connection
{
public:
	OdbcConnection(const std::string& params) : Connection(params)
	{
		connect();
	}

	virtual ~OdbcConnection()
	{
		disconnect();
	}

	virtual bool connect();
	virtual void disconnect();

	virtual bool select();
	virtual bool update();
protected:
	wxDbConnectInf*	mInfo;
	wxDb*			mODBC;

	bool deleteMaps();
	bool deleteObjects();

	bool selectMaps();
	bool selectMapsTerrain();
	bool selectBuildings();
	bool selectBuildingsPoints();
	bool selectJunctions();
	bool selectJunctionsRoutes();
	bool selectPlants();
	bool selectPlantsPoints();
	bool selectRoutes();
	bool selectRoutesPoints();
	bool selectStreets();
	bool selectStreetsRoutes();
	bool selectWaterObjects();
	bool selectWaterObjectsPoints();

	bool updateMaps();
	bool updateMap(Map* map);
	bool updateObjects();
	bool updateObject(Object* object);
	bool updateBuilding(Building* building);
	bool updateJunction(Junction* junction);
	bool updatePlant(Plant* plant);
	bool updateRoute(Route* route);
	bool updateStreet(Street* street);
	bool updateWaterObject(WaterObject* waterObject);
};

extern "C" __declspec(dllexport) Connection* CreateInstance(const std::string& params);
extern "C" __declspec(dllexport) const std::string& GetName();
