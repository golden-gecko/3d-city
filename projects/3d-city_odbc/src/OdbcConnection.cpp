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

#include "OdbcConnection.h"
#include "Database.h"

bool OdbcConnection::connect()
{
	disconnect();

	mInfo = new (std::nothrow) wxDbConnectInf();
 
	if (mInfo)
	{
		if (mInfo->AllocHenv())
		{
			mODBC = new (std::nothrow) wxDb(mInfo->GetHenv(), false);

			if (mODBC)
			{
				mODBC->SetDebugErrorMessages(true);

				if (mODBC->Open(mParameters, false))
				{
					return mIsConnected = true;
				}
			}
			mInfo->FreeHenv();
		}
		delete mInfo;
	}
	return false;
}

void OdbcConnection::disconnect()
{
	if (mIsConnected)
	{
		mIsConnected = false;

		mODBC->CommitTrans();
		mODBC->Close();

		mInfo->FreeHenv();

		delete mODBC;
		delete mInfo;
	}
}

bool OdbcConnection::deleteMaps()
{
	return mODBC->ExecSql("delete from maps");
}

bool OdbcConnection::deleteObjects()
{
	return mODBC->ExecSql("delete from objects");
}

bool OdbcConnection::select()
{
	if (mIsConnected == false)
	{
		return false;
	}

	selectMaps();
	selectMapsTerrain();
	selectBuildings();
	selectBuildingsPoints();
	selectJunctions();
	selectJunctionsRoutes();
	selectPlants();
	selectPlantsPoints();
	selectRoutes();
	selectRoutesPoints();
	selectStreets();
	selectStreetsRoutes();
	selectWaterObjects();
	selectWaterObjectsPoints();

	return true;
}

bool OdbcConnection::selectMaps()
{
	if (mODBC->ExecSql("select * from v_maps") == false)
	{
		return false;
	}

	long cbReturned;

	while (mODBC->GetNext())
	{
		unsigned int mapID = 0, terrainSizeX = 0, terrainSizeZ = 0;
		char name[500] = { '\0' }, description[5000] = { '\0' };
		double centerLatitude = 0.0, centerLongitude = 0.0;
		float sizeX = 0.0f, sizeZ = 0.0f, zoom = 0.0f;
		char texture[500] = { '\0' };

		mODBC->GetData(1, SQL_C_LONG, &mapID, 1, &cbReturned);
		mODBC->GetData(2, SQL_C_CHAR, name, 500, &cbReturned);
		mODBC->GetData(3, SQL_C_CHAR, description, 5000, &cbReturned);
		mODBC->GetData(4, SQL_C_FLOAT, &sizeX, 1, &cbReturned);
		mODBC->GetData(5, SQL_C_FLOAT, &sizeZ, 1, &cbReturned);
		mODBC->GetData(6, SQL_C_LONG, &terrainSizeX, 1, &cbReturned);
		mODBC->GetData(7, SQL_C_LONG, &terrainSizeZ, 1, &cbReturned);
		mODBC->GetData(8, SQL_C_DOUBLE, &centerLatitude, 1, &cbReturned);
		mODBC->GetData(9, SQL_C_DOUBLE, &centerLongitude, 1, &cbReturned);
		mODBC->GetData(10, SQL_C_FLOAT, &zoom, 1, &cbReturned);
		mODBC->GetData(11, SQL_C_CHAR, texture, 500, &cbReturned);

		if (Map* map = mDatabase->createMap(mapID))
		{
			map->setName(name);
			map->setDescription(description);
			map->setSize(Size(sizeX, sizeZ));
			map->setTerrainSize(TerrainSize(terrainSizeX, terrainSizeZ));
			map->setCenter(Point(centerLatitude, centerLongitude));
			map->setZoom(zoom);
			map->setTexture(texture);
		}
	}

	return true;
}

bool OdbcConnection::selectMapsTerrain()
{
	MapList::const_iterator i = mDatabase->getMaps().begin();

	for (; i != mDatabase->getMaps().end(); ++i)
	{
		std::stringstream query;

		query << "select offset, value from v_maps_terrain ";
		query << "where map_id = " << i->first;

		if (mODBC->ExecSql(query.str()) == false)
		{
			return false;
		}

		TerrainData data(i->second->getTerrainSize().x * i->second->getTerrainSize().z);

		long cbReturned;

		while (mODBC->GetNext())
		{
			unsigned int offset = 0;
			float value = 0.0f;

			mODBC->GetData(1, SQL_C_LONG, &offset, 1, &cbReturned);
			mODBC->GetData(2, SQL_C_FLOAT, &value, 1, &cbReturned);

			data[offset] = value;
		}

		i->second->setTerrainData(data);
	}

	return true;
}

bool OdbcConnection::selectBuildings()
{
	if (mODBC->ExecSql("select * from v_buildings") == false)
	{
		return false;
	}

	long cbReturned;

	while (mODBC->GetNext())
	{
		unsigned int buildingID = 0, mapID = 0, number = 0;
		char name[500] = { '\0' }, description[5000] = { '\0' }, type[500] = { '\0' };
		float height = 0.0f;
		char texture[500] = { '\0' };

		mODBC->GetData(1, SQL_C_LONG, &buildingID, 1, &cbReturned);
		mODBC->GetData(2, SQL_C_LONG, &mapID, 1, &cbReturned);
		mODBC->GetData(3, SQL_C_CHAR, name, 500, &cbReturned);
		mODBC->GetData(4, SQL_C_CHAR, description, 5000, &cbReturned);
		mODBC->GetData(5, SQL_C_LONG, &number, 1, &cbReturned);
		mODBC->GetData(6, SQL_C_FLOAT, &height, 1, &cbReturned);
		mODBC->GetData(7, SQL_C_CHAR, texture, 500, &cbReturned);

		if (Map* map = mDatabase->getMap(mapID))
		{
			if (Building* building = map->create<Building>(buildingID))
			{
				building->setName(name);
				building->setDescription(description);
				building->setNumber(number);
				building->setHeight(height);
				building->setTexture(texture);
			}
		}
	}

	return true;
}

bool OdbcConnection::selectBuildingsPoints()
{
	if (mODBC->ExecSql("select * from v_buildings_points") == false)
	{
		return false;
	}

	long cbReturned;

	while (mODBC->GetNext())
	{
		unsigned int mapID = 0, buildingID = 0, number = 0;
		float x = 0.0f, y = 0.0f, z = 0.0f;

		mODBC->GetData(1, SQL_C_LONG, &mapID, 1, &cbReturned);
		mODBC->GetData(2, SQL_C_LONG, &buildingID, 1, &cbReturned);
		mODBC->GetData(3, SQL_C_LONG, &number, 1, &cbReturned);
		mODBC->GetData(4, SQL_C_FLOAT, &x, 1, &cbReturned);
		mODBC->GetData(5, SQL_C_FLOAT, &y, 1, &cbReturned);
		mODBC->GetData(6, SQL_C_FLOAT, &z, 1, &cbReturned);

		if (Map* map = mDatabase->getMap(mapID))
		{
			if (Building* building = map->get<Building>(buildingID))
			{
				building->setPoint(number, Point3D(x, y, z));
			}
		}
	}

	return true;
}

bool OdbcConnection::selectJunctions()
{
	if (mODBC->ExecSql("select * from junctions") == false)
	{
		return false;
	}

	long cbReturned;

	while (mODBC->GetNext())
	{
		unsigned int mapID = 0, id = 0;
		char name[500] = { '\0' }, description[5000] = { '\0' };
		float x = 0.0f, y = 0.0f, z = 0.0f;

		mODBC->GetData(1, SQL_C_LONG, &mapID, 1, &cbReturned);
		mODBC->GetData(2, SQL_C_LONG, &id, 1, &cbReturned);
		mODBC->GetData(3, SQL_C_CHAR, name, 500, &cbReturned);
		mODBC->GetData(4, SQL_C_CHAR, description, 5000, &cbReturned);
		mODBC->GetData(6, SQL_C_FLOAT, &x, 1, &cbReturned);
		mODBC->GetData(7, SQL_C_FLOAT, &y, 1, &cbReturned);
		mODBC->GetData(8, SQL_C_FLOAT, &z, 1, &cbReturned);

		if (Map* map = mDatabase->getMap(mapID))
		{
			if (Junction* junction = map->create<Junction>(id))
			{
				junction->setName(name);
				junction->setDescription(description);
				junction->setCenter(Point3D(x, y, z));
			}
		}
	}

	return true;
}

bool OdbcConnection::selectJunctionsRoutes()
{
	if (mODBC->ExecSql("select * from v_junctions_routes") == false)
	{
		return false;
	}

	long cbReturned;

	while (mODBC->GetNext())
	{
		unsigned int mapID = 0, junctionID = 0, routeID = 0;

		mODBC->GetData(1, SQL_C_LONG, &mapID, 1, &cbReturned);
		mODBC->GetData(2, SQL_C_LONG, &junctionID, 1, &cbReturned);
		mODBC->GetData(3, SQL_C_LONG, &routeID, 1, &cbReturned);

		if (Map* map = mDatabase->getMap(mapID))
		{
			if (Junction* junction = map->get<Junction>(junctionID))
			{
				junction->addRoute(routeID);
			}
		}
	}

	return true;
}

bool OdbcConnection::selectPlants()
{
	if (mODBC->ExecSql("select * from v_plants") == false)
	{
		return false;
	}

	long cbReturned;

	while (mODBC->GetNext())
	{
		unsigned int mapID = 0, plantID = 0;
		char name[500] = { '\0' }, description[5000] = { '\0' };
		float height = 0.0f;

		mODBC->GetData(1, SQL_C_LONG, &mapID, 1, &cbReturned);
		mODBC->GetData(2, SQL_C_LONG, &plantID, 1, &cbReturned);
		mODBC->GetData(3, SQL_C_CHAR, name, 500, &cbReturned);
		mODBC->GetData(4, SQL_C_CHAR, description, 5000, &cbReturned);
		mODBC->GetData(5, SQL_C_FLOAT, &height, 1, &cbReturned);

		if (Map* map = mDatabase->getMap(mapID))
		{
			if (Plant* plant = map->create<Plant>(plantID))
			{
				plant->setName(name);
				plant->setDescription(description);
				plant->setHeight(height);
			}
		}
	}

	return true;
}

bool OdbcConnection::selectPlantsPoints()
{
	if (mODBC->ExecSql("select * from v_plants_points") == false)
	{
		return false;
	}

	long cbReturned;

	while (mODBC->GetNext())
	{
		unsigned int mapID = 0, plantID = 0, number = 0;
		float x = 0.0f, y = 0.0f, z = 0.0f;

		mODBC->GetData(1, SQL_C_LONG, &mapID, 1, &cbReturned);
		mODBC->GetData(2, SQL_C_LONG, &plantID, 1, &cbReturned);
		mODBC->GetData(3, SQL_C_LONG, &number, 1, &cbReturned);
		mODBC->GetData(4, SQL_C_FLOAT, &x, 1, &cbReturned);
		mODBC->GetData(5, SQL_C_FLOAT, &y, 1, &cbReturned);
		mODBC->GetData(6, SQL_C_FLOAT, &z, 1, &cbReturned);

		if (Map* map = mDatabase->getMap(mapID))
		{
			if (Plant* plant = map->get<Plant>(plantID))
			{
				plant->setPoint(number, Point3D(x, y, z));
			}
		}
	}

	return true;
}

bool OdbcConnection::selectRoutes()
{
	if (mODBC->ExecSql("select * from v_routes") == false)
	{
		return false;
	}

	while (mODBC->GetNext())
	{
		unsigned int mapID = 0, routeID = 0, maxSpeed = 0;
		char name[500] = { '\0' }, description[5000] = { '\0' };
		float width = 0.0f, avgSpeed = 0.0f;

		long cbReturned;

		mODBC->GetData(1, SQL_C_LONG, &mapID, 1, &cbReturned);
		mODBC->GetData(2, SQL_C_LONG, &routeID, 1, &cbReturned);
		mODBC->GetData(3, SQL_C_CHAR, name, 500, &cbReturned);
		mODBC->GetData(4, SQL_C_CHAR, description, 5000, &cbReturned);
		mODBC->GetData(5, SQL_C_FLOAT, &avgSpeed, 1, &cbReturned);
		mODBC->GetData(6, SQL_C_LONG, &maxSpeed, 1, &cbReturned);
		mODBC->GetData(7, SQL_C_FLOAT, &width, 1, &cbReturned);

		if (Map* map = mDatabase->getMap(mapID))
		{
			if (Route* route = map->create<Route>(routeID))
			{
				route->setName(name);
				route->setDescription(description);
				route->setAvarageSpeed(avgSpeed);
				route->setMaxSpeed(maxSpeed);
				route->setWidth(width);
			}
		}
	}

	return true;
}

bool OdbcConnection::selectRoutesPoints()
{
	if (mODBC->ExecSql("select * from v_routes_points") == false)
	{
		return false;
	}

	long cbReturned;

	while (mODBC->GetNext())
	{
		unsigned int mapID = 0, routeID = 0, number = 0;
		float x = 0.0f, y = 0.0f, z = 0.0f;

		mODBC->GetData(1, SQL_C_LONG, &mapID, 1, &cbReturned);
		mODBC->GetData(2, SQL_C_LONG, &routeID, 1, &cbReturned);
		mODBC->GetData(3, SQL_C_LONG, &number, 1, &cbReturned);
		mODBC->GetData(4, SQL_C_FLOAT, &x, 1, &cbReturned);
		mODBC->GetData(5, SQL_C_FLOAT, &y, 1, &cbReturned);
		mODBC->GetData(6, SQL_C_FLOAT, &z, 1, &cbReturned);

		if (Map* map = mDatabase->getMap(mapID))
		{
			if (Route* route = map->get<Route>(routeID))
			{
				route->setPoint(number, Point3D(x, y, z));
			}
		}
	}

	return true;
}

bool OdbcConnection::selectStreets()
{
	return true;
}

bool OdbcConnection::selectStreetsRoutes()
{
	return true;
}

bool OdbcConnection::selectWaterObjects()
{
	if (mODBC->ExecSql("select * from v_water_objects") == false)
	{
		return false;
	}

	long cbReturned;

	while (mODBC->GetNext())
	{
		unsigned int mapID = 0, waterObjectID = 0, isRiver = 0;
		char name[500] = { '\0' }, description[5000] = { '\0' };
		float width = 0.0f;

		mODBC->GetData(1, SQL_C_LONG, &mapID, 1, &cbReturned);
		mODBC->GetData(2, SQL_C_LONG, &waterObjectID, 1, &cbReturned);
		mODBC->GetData(3, SQL_C_CHAR, name, 500, &cbReturned);
		mODBC->GetData(4, SQL_C_CHAR, description, 5000, &cbReturned);
		mODBC->GetData(5, SQL_C_LONG, &isRiver, 1, &cbReturned);
		mODBC->GetData(6, SQL_C_FLOAT, &width, 1, &cbReturned);

		if (Map* map = mDatabase->getMap(mapID))
		{
			if (WaterObject* waterObject = map->create<WaterObject>(waterObjectID))
			{
				waterObject->setName(name);
				waterObject->setDescription(description);
				waterObject->setIsRiver(isRiver ? true : false);
				waterObject->setWidth(width);
			}
		}
	}

	return true;
}

bool OdbcConnection::selectWaterObjectsPoints()
{
	if (mODBC->ExecSql("select * from v_water_objects_points") == false)
	{
		return false;
	}

	long cbReturned;

	while (mODBC->GetNext())
	{
		unsigned int mapID = 0, waterObjectID = 0, number = 0;
		float x = 0.0f, y = 0.0f, z = 0.0f;

		mODBC->GetData(1, SQL_C_LONG, &mapID, 1, &cbReturned);
		mODBC->GetData(2, SQL_C_LONG, &waterObjectID, 1, &cbReturned);
		mODBC->GetData(3, SQL_C_LONG, &number, 1, &cbReturned);
		mODBC->GetData(4, SQL_C_FLOAT, &x, 1, &cbReturned);
		mODBC->GetData(5, SQL_C_FLOAT, &y, 1, &cbReturned);
		mODBC->GetData(6, SQL_C_FLOAT, &z, 1, &cbReturned);

		if (Map* map = mDatabase->getMap(mapID))
		{
			if (WaterObject* waterObject = map->get<WaterObject>(waterObjectID))
			{
				waterObject->setPoint(number, Point3D(x, y, z));
			}
		}
	}

	return true;
}

bool OdbcConnection::update()
{
	if (mIsConnected == false)
	{
		return false;
	}

	if (mODBC->ExecSql("call begin_transaction()") == false)
	{
		return false;
	}

	if (deleteMaps() && updateMaps() && updateObjects())
	{
		return mODBC->CommitTrans();
	}

	mODBC->RollbackTrans();

	return false;
}

bool OdbcConnection::updateMaps()
{
	MapList::const_iterator i = mDatabase->getMaps().begin();

	for (; i != mDatabase->getMaps().end(); ++i)
	{
		if (updateMap(i->second) == false)
		{
			return false;
		}
	}

	return true;
}

bool OdbcConnection::updateMap(Map* map)
{
	std::stringstream query;
	query.precision(16);

	query << "insert into maps (";
	query << "map_id, name, description,";
	query << "size_x, size_z, terrain_size_x, terrain_size_z,";
	query << "center_latitude, center_longitude, zoom, texture) values (";
	query << "\"" << map->getID() << "\",";
	query << "\"" << map->getName() << "\",";
	query << "\"" << map->getDescription() << "\",";
	query << "\"" << map->getSize().x << "\",";
	query << "\"" << map->getSize().z << "\",";
	query << "\"" << map->getTerrainSize().x << "\",";
	query << "\"" << map->getTerrainSize().z << "\",";
	query << "\"" << map->getCenter().latitude << "\",";
	query << "\"" << map->getCenter().longitude << "\",";
	query << "\"" << map->getZoom() << "\",";
	query << "\"" << map->getTexture() << "\")";

	if (mODBC->ExecSql(query.str()) == false)
	{
		return false;
	}

	for (TerrainData::size_type i = 0; i < map->getTerrainData().size(); ++i)
	{
		query.str("");

		query << "insert into maps_terrain (map_id, offset, value) values (";
		query << "\"" << map->getID() << "\",";
		query << "\"" << i << "\",";
		query << "\"" << map->getTerrainData()[i] << "\")";

		if (mODBC->ExecSql(query.str()) == false)
		{
			return false;
		}
	}

	return true;
}

bool OdbcConnection::updateObjects()
{
	MapList::const_iterator m = mDatabase->getMaps().begin();

	for (; m != mDatabase->getMaps().end(); ++m)
	{
		ObjectList::const_iterator o = m->second->getObjects().begin();

		for (; o != m->second->getObjects().end(); ++o)
		{
			if (updateObject(o->second) == false)
			{
				return false;
			}
		}
	}

	return true;
}

bool OdbcConnection::updateObject(Object* object)
{
	switch (object->getType())
	{
		case OT_BUILDING:
		{
			if (updateBuilding(static_cast<Building*>(object)) == false)
			{
				return false;
			}
			break;
		}

		case OT_JUNCTION:
		{
			if (updateJunction(static_cast<Junction*>(object)) == false)
			{
				return false;
			}
			break;
		}

		case OT_PLANT:
		{
			if (updatePlant(static_cast<Plant*>(object)) == false)
			{
				return false;
			}
			break;
		}

		case OT_ROUTE:
		{
			if (updateRoute(static_cast<Route*>(object)) == false)
			{
				return false;
			}
			break;
		}

		case OT_STREET:
		{
			if (updateStreet(static_cast<Street*>(object)) == false)
			{
				return false;
			}
			break;
		}

		case OT_WATER_OBJECT:
		{
			if (updateWaterObject(static_cast<WaterObject*>(object)) == false)
			{
				return false;
			}
			break;
		}
	}

	return true;
}

bool OdbcConnection::updateBuilding(Building* building)
{
	std::stringstream query;
	query.precision(16);

	query << "insert into buildings (";
	query << "map_id, building_id, name, description,";
	query << "number, height, texture) values (";
	query << "\"" << building->getMap()->getID() << "\", ";
	query << "\"" << building->getID() << "\", ";
	query << "\"" << building->getName() << "\", ";
	query << "\"" << building->getDescription() << "\", ";
	query << "\"" << building->getNumber() << "\", ";
	query << "\"" << building->getHeight() << "\", ";
	query << "\"" << building->getTexture() << "\")";

	if (mODBC->ExecSql(query.str().c_str()) == false)
	{
		return false;
	}

	for (unsigned int i = 0; i < building->getPointCount(); ++i)
	{
		query.str("");

		query << "insert into buildings_points (building_id, number, x, y, z) values (";
		query << "\"" << building->getID() << "\", ";
		query << "\"" << i << "\", ";
		query << "\"" << building->getPoint(i).x << "\", ";
		query << "\"" << building->getPoint(i).y << "\", ";
		query << "\"" << building->getPoint(i).z << "\")";

		if (mODBC->ExecSql(query.str().c_str()) == false)
		{
			return false;
		}
	}

	return true;
}

bool OdbcConnection::updateJunction(Junction* junction)
{
	std::stringstream query;
	query.precision(16);

	query << "insert into junctions (map_id, junction_id, name, description, center_x, center_y, center_z) values (";
	query << "\"" << junction->getMap()->getID() << "\", ";
	query << "\"" << junction->getID() << "\", ";
	query << "\"" << junction->getName() << "\", ";
	query << "\"" << junction->getDescription() << "\", ";
	query << "\"" << junction->getCenter().x << "\", ";
	query << "\"" << junction->getCenter().y << "\", ";
	query << "\"" << junction->getCenter().z << "\")";

	if (mODBC->ExecSql(query.str().c_str()) == false)
	{
		return false;
	}

	RouteList::const_iterator i = junction->getRoutes().begin();
	for (0; i != junction->getRoutes().end(); ++i)
	{
		query.str("");

		query << "insert into junctions_routes (junction_id, route_id) values (";
		query << "\"" << junction->getID() << "\", ";
		query << "\"" << i->first << "\")";

		if (mODBC->ExecSql(query.str().c_str()) == false)
		{
			return false;
		}
	}

	return true;
}

bool OdbcConnection::updatePlant(Plant* plant)
{
	std::stringstream query;
	query.precision(16);

	query << "insert into plants (map_id, plant_id, name, description, height) values (";
	query << "\"" << plant->getMap()->getID() << "\", ";
	query << "\"" << plant->getID() << "\", ";
	query << "\"" << plant->getName() << "\", ";
	query << "\"" << plant->getDescription() << "\", ";
	query << "\"" << plant->getHeight() << "\")";

	if (mODBC->ExecSql(query.str().c_str()) == false)
	{
		return false;
	}

	for (unsigned int i = 0; i < plant->getPointCount(); ++i)
	{
		query.str("");

		query << "insert into plants_points (plant_id, number, x, y, z) values (";
		query << "\"" << plant->getID() << "\", ";
		query << "\"" << i << "\", ";
		query << "\"" << plant->getPoint(i).x << "\", ";
		query << "\"" << plant->getPoint(i).y << "\", ";
		query << "\"" << plant->getPoint(i).z << "\")";

		if (mODBC->ExecSql(query.str().c_str()) == false)
		{
			return false;
		}
	}

	return true;
}

bool OdbcConnection::updateRoute(Route* route)
{
	std::stringstream query;
	query.precision(16);

	query << "insert into routes (";
	query << "map_id, route_id, name, description,";
	query << "avarage_speed, max_speed, width) values (";
	query << "\"" << route->getMap()->getID() << "\", ";
	query << "\"" << route->getID() << "\", ";
	query << "\"" << route->getName() << "\", ";
	query << "\"" << route->getDescription() << "\", ";
	query << "\"" << route->getAvarageSpeed() << "\", ";
	query << "\"" << route->getMaxSpeed() << "\", ";
	query << "\"" << route->getWidth() << "\")";

	if (mODBC->ExecSql(query.str().c_str()) == false)
	{
		return false;
	}

	for (unsigned int i = 0; i < route->getPointCount(); ++i)
	{
		query.str("");

		query << "insert into routes_points (route_id, number, x, y, z) values (";
		query << "\"" << route->getID() << "\", ";
		query << "\"" << i << "\", ";
		query << "\"" << route->getPoint(i).x << "\", ";
		query << "\"" << route->getPoint(i).y << "\", ";
		query << "\"" << route->getPoint(i).z << "\")";

		if (mODBC->ExecSql(query.str().c_str()) == false)
		{
			return false;
		}
	}

	return true;
}

bool OdbcConnection::updateStreet(Street* street)
{
	return true;
}

bool OdbcConnection::updateWaterObject(WaterObject* waterObject)
{
	std::stringstream query;
	query.precision(16);

	query << "insert into water_objects (";
	query << "map_id, water_object_id, name,";
	query << "description, is_river, width) values (";
	query << "\"" << waterObject->getMap()->getID() << "\", ";
	query << "\"" << waterObject->getID() << "\", ";
	query << "\"" << waterObject->getName() << "\", ";
	query << "\"" << waterObject->getDescription() << "\", ";
	query << "\"" << waterObject->getIsRiver() << "\", ";
	query << "\"" << waterObject->getWidth() << "\")";

	if (mODBC->ExecSql(query.str().c_str()) == false)
	{
		return false;
	}

	for (unsigned int i = 0; i < waterObject->getPointCount(); ++i)
	{
		query.str("");

		query << "insert into water_objects_points (water_object_id, number, ";
		query << "x, y, z) values (";
		query << "\"" << waterObject->getID() << "\", ";
		query << "\"" << i << "\", ";
		query << "\"" << waterObject->getPoint(i).x << "\", ";
		query << "\"" << waterObject->getPoint(i).y << "\", ";
		query << "\"" << waterObject->getPoint(i).z << "\")";

		if (mODBC->ExecSql(query.str().c_str()) == false)
		{
			return false;
		}
	}

	return true;
}

extern "C" __declspec(dllexport) Connection* CreateInstance(const std::string& params)
{
	return new OdbcConnection(params);
}

extern "C" __declspec(dllexport) const std::string& GetName()
{
	static std::string name = "ODBC";

	return name;
}
