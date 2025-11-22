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

#include "BinaryConnection.h"
#include "Database.h"

bool BinaryConnection::connect()
{
	if (mIsConnected)
	{
		disconnect();
	}

	mFile.open(
		mParameters.c_str(),
		std::ios::binary | std::ios::in | std::ios::out);

	return mIsConnected = mFile.is_open();
}

void BinaryConnection::disconnect()
{
	if (mIsConnected)
	{
		mIsConnected = false;

		mFile.close();
	}
}

bool BinaryConnection::deleteMaps()
{
	disconnect();

	mFile.open(
		mParameters.c_str(),
		std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);

	return mIsConnected = mFile.is_open();
}

bool BinaryConnection::deleteObjects()
{
	return true;
}

bool BinaryConnection::select()
{
	return mIsConnected && selectMaps();
}

bool BinaryConnection::selectMaps()
{
	unsigned int count = 0;
	mFile.read((char*)&count, sizeof(count));

	for (unsigned int i = 0; i < count; ++i)
	{
		unsigned int id;
		mFile.read((char*)&id, sizeof(id));

		if (Map* map = mDatabase->createMap(id))
		{
			char name[500];
			char description[5000];
			char texture[500];

			unsigned int sizeX, sizeZ;
			unsigned int terSizeX, terSizeZ;
			double lat, lon;
			float zoom;

			mFile.read(name, 500);
			mFile.read(description, 5000);
			mFile.read((char*)&sizeX, sizeof(sizeX));
			mFile.read((char*)&sizeZ, sizeof(sizeZ));
			mFile.read((char*)&terSizeX, sizeof(terSizeX));
			mFile.read((char*)&terSizeZ, sizeof(terSizeZ));
			mFile.read((char*)&lat, sizeof(lat));
			mFile.read((char*)&lon, sizeof(lon));
			mFile.read((char*)&zoom, sizeof(zoom));
			mFile.read(texture, 500);

			TerrainData data(terSizeX * terSizeZ);

			float* buffer = new float[terSizeX * terSizeZ];

			mFile.read((char*)buffer, terSizeX * terSizeZ * sizeof(float));

			for (unsigned int i = 0; i < terSizeX * terSizeZ; ++i)
			{
				data[i] = buffer[i];
			}

			delete[] buffer;

			map->setName(name);
			map->setDescription(description);
			map->setSize(Size(sizeX, sizeZ));
			map->setTerrainSize(TerrainSize(terSizeX, terSizeZ));
			map->setCenter(Point(lat, lon));
			map->setZoom(zoom);
			map->setTexture(texture);
			map->setTerrainData(data);
		}

		unsigned int objectCount;
		mFile.read((char*)&objectCount, sizeof(unsigned int));

		for (unsigned int i = 0; i < objectCount; ++i)
		{
			unsigned int objectType;
			mFile.read((char*)&objectType, sizeof(unsigned int));

			switch (objectType)
			{
				case OT_BUILDING:
				{
					if (selectBuilding() == false)
					{
						return false;
					}
					break;
				}

				case OT_JUNCTION:
				{
					if (selectJunction() == false)
					{
						return false;
					}
					break;
				}

				case OT_PLANT:
				{
					if (selectPlant() == false)
					{
						return false;
					}
					break;
				}

				case OT_ROUTE:
				{
					if (selectRoute() == false)
					{
						return false;
					}
					break;
				}

				case OT_STREET:
				{
					if (selectStreet() == false)
					{
						return false;
					}
					break;
				}

				case OT_WATER_OBJECT:
				{
					if (selectWaterObject() == false)
					{
						return false;
					}
					break;
				}
			}
		}
	}

	return true;
}

bool BinaryConnection::selectBuilding()
{
	unsigned int id, mapID;
	mFile.read((char*)&id, sizeof(unsigned int));
	mFile.read((char*)&mapID, sizeof(unsigned int));

	if (Building* building = mDatabase->getMap(mapID)->create<Building>(id))
	{
		unsigned int streetID = 0, number = 0;
		char name[500] = { '\0' }, description[5000] = { '\0' }, texture[500] = { '\0' };
		float height = 0.0f;

		mFile.read(name, 500);
		mFile.read(description, 5000);
		mFile.read((char*)&height, sizeof(height));
		mFile.read((char*)&number, sizeof(number));
		mFile.read(texture, 500);

		building->setName(name);
		building->setDescription(description);
		building->setNumber(number);
		building->setHeight(height);
		building->setTexture(texture);

		unsigned int pointCount;
		mFile.read((char*)&pointCount, sizeof(unsigned int));
		
		for (unsigned int j = 0; j < pointCount; ++j)
		{
			float x, y, z;

			mFile.read((char*)&x, sizeof(float));
			mFile.read((char*)&y, sizeof(float));
			mFile.read((char*)&z, sizeof(float));

			building->setPoint(j, Point3D(x, y, z));
		}
	}

	return true;
}

bool BinaryConnection::selectJunction()
{
	unsigned int id, mapID;
	mFile.read((char*)&id, sizeof(unsigned int));
	mFile.read((char*)&mapID, sizeof(unsigned int));

	if (Junction* junction = mDatabase->getMap(mapID)->create<Junction>(id))
	{
		char name[500] = { '\0' }, description[5000] = { '\0' };
		float x, y, z;

		mFile.read(name, 500);
		mFile.read(description, 5000);
		mFile.read((char*)&x, sizeof(float));
		mFile.read((char*)&y, sizeof(float));
		mFile.read((char*)&z, sizeof(float));

		junction->setName(name);
		junction->setDescription(description);
		junction->setCenter(Point3D(x, y, z));

		unsigned int routesCount;
		mFile.read((char*)&routesCount, sizeof(unsigned int));
		
		for (unsigned int j = 0; j < routesCount; ++j)
		{
			unsigned int id;

			mFile.read((char*)&id, sizeof(id));

			junction->addRoute(id);
		}
	}

	return true;
}

bool BinaryConnection::selectPlant()
{
	unsigned int id, mapID;
	mFile.read((char*)&id, sizeof(unsigned int));
	mFile.read((char*)&mapID, sizeof(unsigned int));

	if (Plant* plant = mDatabase->getMap(mapID)->create<Plant>(id))
	{
		unsigned int streetID = 0, number = 0;
		char name[500] = { '\0' }, description[5000] = { '\0' }, type[500] = { '\0' };
		float height = 0.0f;

		mFile.read(name, 500);
		mFile.read(description, 5000);
		mFile.read((char*)&height, sizeof(height));

		plant->setName(name);
		plant->setDescription(description);
		plant->setHeight(height);

		unsigned int pointCount;
		mFile.read((char*)&pointCount, sizeof(unsigned int));

		for (unsigned int j = 0; j < pointCount; ++j)
		{
			float x, y, z;

			mFile.read((char*)&x, sizeof(float));
			mFile.read((char*)&y, sizeof(float));
			mFile.read((char*)&z, sizeof(float));

			plant->setPoint(j, Point3D(x, y, z));
		}
	}

	return true;
}

bool BinaryConnection::selectRoute()
{
	unsigned int id, mapID;
	mFile.read((char*)&id, sizeof(unsigned int));
	mFile.read((char*)&mapID, sizeof(unsigned int));

	if (Route* route = mDatabase->getMap(mapID)->create<Route>(id))
	{
		unsigned int streetID = 0, number = 0;
		char name[500] = { '\0' }, description[5000] = { '\0' }, type[500] = { '\0' };
		float width = 0.0f;

		mFile.read(name, 500);
		mFile.read(description, 5000);
		mFile.read((char*)&width, sizeof(width));

		route->setName(name);
		route->setDescription(description);
		route->setWidth(width);

		unsigned int pointCount;
		mFile.read((char*)&pointCount, sizeof(unsigned int));

		for (unsigned int j = 0; j < pointCount; ++j)
		{
			float x, y, z;

			mFile.read((char*)&x, sizeof(float));
			mFile.read((char*)&y, sizeof(float));
			mFile.read((char*)&z, sizeof(float));

			route->setPoint(j, Point3D(x, y, z));
		}
	}

	return true;
}

bool BinaryConnection::selectStreet()
{
	return true;
}

bool BinaryConnection::selectWaterObject()
{
	unsigned int id, mapID;
	mFile.read((char*)&id, sizeof(unsigned int));
	mFile.read((char*)&mapID, sizeof(unsigned int));

	if (WaterObject* waterObject = mDatabase->getMap(mapID)->create<WaterObject>(id))
	{
		unsigned int streetID = 0, number = 0;
		char name[500] = { '\0' }, description[5000] = { '\0' };

		mFile.read(name, 500);
		mFile.read(description, 5000);

		waterObject->setName(name);
		waterObject->setDescription(description);

		unsigned int pointCount;
		mFile.read((char*)&pointCount, sizeof(unsigned int));

		for (unsigned int j = 0; j < pointCount; ++j)
		{
			float x, y, z;

			mFile.read((char*)&x, sizeof(float));
			mFile.read((char*)&y, sizeof(float));
			mFile.read((char*)&z, sizeof(float));

			waterObject->setPoint(j, Point3D(x, y, z));
		}
	}

	return true;
}

bool BinaryConnection::update()
{
	if (mIsConnected == false)
	{
		return false;
	}

	return deleteMaps() && updateMaps();
}

bool BinaryConnection::updateMaps()
{
	// Save map count
	unsigned int count = (unsigned int)mDatabase->getMaps().size();
	mFile.write((char*)&count, sizeof(count));

	// Save maps
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

bool BinaryConnection::updateMap(Map* map)
{
	char name[500];
	char description[5000];
	char texture[500];

	strcpy_s(name, 500, map->getName().c_str());
	strcpy_s(description, 5000, map->getDescription().c_str());
	strcpy_s(texture, 500, map->getTexture().c_str());

	unsigned int id = map->getID(), sizeX = map->getSize().x, sizeZ = map->getSize().z;
	unsigned int terSizeX = map->getTerrainSize().x, terSizeZ = map->getTerrainSize().z;
	double lat = map->getCenter().latitude, lon = map->getCenter().longitude;
	float zoom = map->getZoom();

	mFile.write((char*)&id, sizeof(id));
	mFile.write(name, 500);
	mFile.write(description, 5000);
	mFile.write((char*)&sizeX, sizeof(sizeX));
	mFile.write((char*)&sizeZ, sizeof(sizeZ));
	mFile.write((char*)&terSizeX, sizeof(terSizeX));
	mFile.write((char*)&terSizeZ, sizeof(terSizeZ));
	mFile.write((char*)&lat, sizeof(lat));
	mFile.write((char*)&lon, sizeof(lon));
	mFile.write((char*)&zoom, sizeof(zoom));
	mFile.write((char*)&texture, 500);

	float* buffer = new float[terSizeX * terSizeZ];

	for (unsigned int i = 0; i < terSizeX * terSizeZ; ++i)
	{
		buffer[i] = map->getTerrainData()[i];
	}

	mFile.write((char*)buffer, terSizeX * terSizeZ * sizeof(float));

	delete[] buffer;

	unsigned int objectCount = (unsigned int)map->getObjects().size();
	mFile.write((char*)&objectCount, sizeof(unsigned int));

	ObjectList::const_iterator i = map->getObjects().begin();
	for (; i != map->getObjects().end(); ++i)
	{
		unsigned int type = i->second->getType();
		mFile.write((char*)&type, sizeof(unsigned int));

		switch (i->second->getType())
		{
			case OT_BUILDING:
			{
				if (updateBuilding(static_cast<Building*>(i->second)) == false)
				{
					return false;
				}
				break;
			}

			case OT_JUNCTION:
			{
				if (updateJunction(static_cast<Junction*>(i->second)) == false)
				{
					return false;
				}
				break;
			}

			case OT_PLANT:
			{
				if (updatePlant(static_cast<Plant*>(i->second)) == false)
				{
					return false;
				}
				break;
			}

			case OT_ROUTE:
			{
				if (updateRoute(static_cast<Route*>(i->second)) == false)
				{
					return false;
				}
				break;
			}

			case OT_STREET:
			{
				if (updateStreet(static_cast<Street*>(i->second)) == false)
				{
					return false;
				}
				break;
			}

			case OT_WATER_OBJECT:
			{
				if (updateWaterObject(static_cast<WaterObject*>(i->second)) == false)
				{
					return false;
				}
				break;
			}
		}
	}

	return true;
}

bool BinaryConnection::updateBuilding(Building* building)
{
	unsigned int id = building->getID();
	unsigned int mapID = building->getMap()->getID();
	unsigned int number = building->getNumber();

	char name[500] = { '\0' };; strcpy_s(name, 500, building->getName().c_str());
	char description[5000] = { '\0' }; strcpy_s(description, 5000, building->getDescription().c_str());
	char texture[500] = { '\0' };; strcpy_s(texture, 500, building->getTexture().c_str());

	float height = building->getHeight();

	mFile.write((char*)&id, sizeof(unsigned int));
	mFile.write((char*)&mapID, sizeof(unsigned int));
	mFile.write(name, 500);
	mFile.write(description, 5000);
	mFile.write((char*)&height, sizeof(float));
	mFile.write((char*)&number, sizeof(unsigned int));
	mFile.write(texture, 500);

	unsigned int pointCount = building->getPointCount();
	mFile.write((char*)&pointCount, sizeof(unsigned int));

	for (unsigned int j = 0; j < pointCount; ++j)
	{
		float
			x = building->getPoint(j).x,
			y = building->getPoint(j).y,
			z = building->getPoint(j).z;

		mFile.write((char*)&x, sizeof(float));
		mFile.write((char*)&y, sizeof(float));
		mFile.write((char*)&z, sizeof(float));
	}

	return true;
}

bool BinaryConnection::updateJunction(Junction* junction)
{
	unsigned int id = junction->getID();
	unsigned int mapID = junction->getMap()->getID();

	char name[500] = { '\0' };; strcpy_s(name, 500, junction->getName().c_str());
	char description[5000] = { '\0' }; strcpy_s(description, 5000, junction->getDescription().c_str());
	float x = junction->getCenter().x, y = junction->getCenter().y, z = junction->getCenter().z;

	mFile.write((char*)&id, sizeof(unsigned int));
	mFile.write((char*)&mapID, sizeof(unsigned int));
	mFile.write(name, 500);
	mFile.write(description, 5000);
	mFile.write((char*)&x, sizeof(float));
	mFile.write((char*)&y, sizeof(float));
	mFile.write((char*)&z, sizeof(float));

	unsigned int routesCount = (unsigned int)junction->getRoutes().size();
	mFile.write((char*)&routesCount, sizeof(unsigned int));

	RouteList::const_iterator i = junction->getRoutes().begin();
	for (; i != junction->getRoutes().end(); ++i)
	{
		unsigned int id = i->first;
		mFile.write((char*)&id, sizeof(id));
	}

	return true;
}

bool BinaryConnection::updatePlant(Plant* plant)
{
	unsigned int id = plant->getID();
	unsigned int mapID = plant->getMap()->getID();

	char name[500] = { '\0' };; strcpy_s(name, 500, plant->getName().c_str());
	char description[5000] = { '\0' }; strcpy_s(description, 5000, plant->getDescription().c_str());

	float height = plant->getHeight();

	mFile.write((char*)&id, sizeof(unsigned int));
	mFile.write((char*)&mapID, sizeof(unsigned int));
	mFile.write(name, 500);
	mFile.write(description, 5000);
	mFile.write((char*)&height, sizeof(float));

	unsigned int pointCount = plant->getPointCount();
	mFile.write((char*)&pointCount, sizeof(unsigned int));

	for (unsigned int j = 0; j < pointCount; ++j)
	{
		float
			x = plant->getPoint(j).x,
			y = plant->getPoint(j).y,
			z = plant->getPoint(j).z;

		mFile.write((char*)&x, sizeof(float));
		mFile.write((char*)&y, sizeof(float));
		mFile.write((char*)&z, sizeof(float));
	}

	return true;
}

bool BinaryConnection::updateRoute(Route* route)
{
	unsigned int id = route->getID();
	unsigned int mapID = route->getMap()->getID();

	char name[500] = { '\0' };; strcpy_s(name, 500, route->getName().c_str());
	char description[5000] = { '\0' }; strcpy_s(description, 5000, route->getDescription().c_str());

	float width = route->getWidth();

	mFile.write((char*)&id, sizeof(unsigned int));
	mFile.write((char*)&mapID, sizeof(unsigned int));
	mFile.write(name, 500);
	mFile.write(description, 5000);
	mFile.write((char*)&width, sizeof(float));

	unsigned int pointCount = route->getPointCount();
	mFile.write((char*)&pointCount, sizeof(unsigned int));

	for (unsigned int j = 0; j < pointCount; ++j)
	{
		float
			x = route->getPoint(j).x,
			y = route->getPoint(j).y,
			z = route->getPoint(j).z;

		mFile.write((char*)&x, sizeof(float));
		mFile.write((char*)&y, sizeof(float));
		mFile.write((char*)&z, sizeof(float));
	}

	return true;
}

bool BinaryConnection::updateStreet(Street* street)
{
	return true;
}

bool BinaryConnection::updateWaterObject(WaterObject* waterObject)
{
	unsigned int id = waterObject->getID();
	unsigned int mapID = waterObject->getMap()->getID();

	char name[500] = { '\0' };; strcpy_s(name, 500, waterObject->getName().c_str());
	char description[5000] = { '\0' }; strcpy_s(description, 5000, waterObject->getDescription().c_str());

	mFile.write((char*)&id, sizeof(unsigned int));
	mFile.write((char*)&mapID, sizeof(unsigned int));
	mFile.write(name, 500);
	mFile.write(description, 5000);

	unsigned int pointCount = waterObject->getPointCount();
	mFile.write((char*)&pointCount, sizeof(unsigned int));

	for (unsigned int j = 0; j < pointCount; ++j)
	{
		float
			x = waterObject->getPoint(j).x,
			y = waterObject->getPoint(j).y,
			z = waterObject->getPoint(j).z;

		mFile.write((char*)&x, sizeof(float));
		mFile.write((char*)&y, sizeof(float));
		mFile.write((char*)&z, sizeof(float));
	}

	return true;
}

extern "C" __declspec(dllexport) Connection* CreateInstance(const std::string& params)
{
	return new BinaryConnection(params);
}

extern "C" __declspec(dllexport) const std::string& GetName()
{
	static std::string name = "Binary";

	return name;
}
