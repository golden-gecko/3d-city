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

#include "Dependencies.h"
#include "BuildingView.h"
#include "City.h"
#include "Log.h"
#include "MapView.h"
#include "JunctionView.h"
#include "PlantView.h"
#include "Render.h"
#include "RouteView.h"
#include "WaterObjectView.h"

ObjectView* MapView::createView(Object* object)
{
	if (mIsLoaded && object && mMap == object->getMap())
	{
		switch (object->getType())
		{
			case OT_BUILDING:
				return createView<BuildingView, Building>(static_cast<Building*>(object));

			case OT_JUNCTION:
				return createView<JunctionView, Junction>(static_cast<Junction*>(object));

			case OT_PLANT:
				return createView<PlantView, Plant>(static_cast<Plant*>(object));

			case OT_ROUTE:
				return createView<RouteView, Route>(static_cast<Route*>(object));

			case OT_WATER_OBJECT:
				return createView<WaterObjectView, WaterObject>(static_cast<WaterObject*>(object));
		}
	}

	return NULL;
}

ObjectView* MapView::getObjectView(unsigned int objectViewID) const
{
	ObjectViewList::const_iterator i = mObjectViews.begin();

	for (; i != mObjectViews.end(); ++i)
	{
		if (objectViewID == i->second->getID())
		{
			return i->second;
		}
	}

	return NULL;
}

void MapView::updateViews(Object* object)
{
	ObjectViewList::const_iterator i = mObjectViews.lower_bound(object);

	for (; i != mObjectViews.upper_bound(object); ++i)
	{
		i->second->update();
	}
}

void MapView::destroyViews(Object* object)
{
	ObjectViewList::const_iterator i = mObjectViews.lower_bound(object);

	for (; i != mObjectViews.upper_bound(object); ++i)
	{
		delete i->second;
	}

	mObjectViews.erase(
		mObjectViews.lower_bound(object),
		mObjectViews.upper_bound(object));
}

void MapView::destroyView(ObjectView* view)
{
	ObjectViewList::iterator i = mObjectViews.begin();

	for (; i != mObjectViews.end(); ++i)
	{
		if (i->second == view)
		{
			mObjectViews.erase(i);

			break;
		}
	}
}

bool MapView::load()
{
	// Create scene manager.
	mManager = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_EXTERIOR_CLOSE);

	// Create fog.
	mManager->setFog(
		Ogre::FOG_LINEAR,
		Ogre::ColourValue(0.54f, 0.62f, 0.98f),
		0.0f, 768.0f, 1024.0f);

	// Create camera.
	mCamera = mManager->createCamera(mManager->getName());
	setIsOrthographic(mIsOrthographic);

	// Create light.
	mLight = mManager->createLight(mManager->getName());
	mLight->setType(Ogre::Light::LT_DIRECTIONAL);
	mLight->setDirection(Ogre::Vector3(0.1, -0.7, 0.3));
	mLight->setDiffuseColour(0.35, 0.35, 0.38);
	mLight->setSpecularColour(0.2, 0.2, 1);

	// Create ray query (mouse selection).
	mRayQuery = mManager->createRayQuery(Ogre::Ray());
	mRayQuery->setSortByDistance(true);

	// Create volume query (box selection).
	mVolumeQuery = mManager->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());

	mIsLoaded = true;

//	createTerrain();

	// Faster, but not editable.
	createTerrainFromFile();

	createObjects();

	return true;
}

void MapView::unload()
{
	if (mIsLoaded)
	{
		mIsLoaded = false;

		destroyObjects();
		destroyTerrain();

		Ogre::Root::getSingleton().destroySceneManager(mManager);
	}
}

bool MapView::update()
{
	unload();

	return load();
}

template <class ViewType, class ObjectType>
ViewType* MapView::createView(ObjectType* object)
{
	City::getSingleton().setLastViewID(
		City::getSingleton().getLastViewID() + 1);

	ViewType* view = new ViewType(
		City::getSingleton().getLastViewID(),
		this, object);

	mObjectViews.insert(std::make_pair(object, view));

	return view;
}

ObjectView* MapView::raycastObjectView(float x, float y)
{
	mRayQuery->setRay(mCamera->getCameraToViewportRay(x, y));

	if (mRayQuery->execute().size())
	{
		const Ogre::RaySceneQueryResult& results = mRayQuery->getLastResults();

		for (Ogre::RaySceneQueryResult::const_iterator i = results.begin(); i != results.end(); i++)
		{
			if (i->movable && i->movable->getUserAny().isEmpty() == false)
			{
				if (i->movable->getUserAny().getType() == typeid(BuildingView*))
				{		
					return Ogre::any_cast<BuildingView*>(i->movable->getUserAny());
				}
				else if (i->movable->getUserAny().getType() == typeid(PlantView*))
				{
					return Ogre::any_cast<PlantView*>(i->movable->getUserAny());
				}
				else if (i->movable->getUserAny().getType() == typeid(RouteView*))
				{
					return Ogre::any_cast<RouteView*>(i->movable->getUserAny());
				}
				else if (i->movable->getUserAny().getType() == typeid(WaterObjectView*))
				{
					return Ogre::any_cast<WaterObjectView*>(i->movable->getUserAny());
				}
			}
		}
	}

	return NULL;
}

Point3D MapView::raycastTerrainPoint3D(const Ogre::Ray& ray) const
{
	mRayQuery->setRay(ray);

	if (mRayQuery->execute().size())
	{
		Ogre::RaySceneQueryResult::const_iterator i = mRayQuery->getLastResults().begin();

		for (; i != mRayQuery->getLastResults().end(); i++)
		{
			if (i->worldFragment)
			{
				return Point3D(
					i->worldFragment->singleIntersection.x,
					i->worldFragment->singleIntersection.y,
					i->worldFragment->singleIntersection.z);
			}
		}
	}

	return Point3D();
}

void MapView::createTerrain()
{
	unsigned int sizeX = mMap->getTerrainSize().x;
	unsigned int sizeZ = mMap->getTerrainSize().z;

	mTileX = 33;
	mTileZ = 33;

	unsigned int tileCount = ((sizeX - 1) / (mTileX - 1)) * ((sizeZ - 1) / (mTileZ - 1));
	unsigned int vertexPerTile = mTileX * mTileZ;
	unsigned int trianglesPerTile = (mTileX - 1) * (mTileZ - 1) * 2;

	mMeshes.resize(tileCount);
	mEntities.resize(tileCount);
	mSceneNodes.resize(tileCount);

	// vertexPerTile * 3 vertices * 2 texture coordinates * 3 colours * 3 normals
	VertexVector vertices(vertexPerTile * 11);

	// trianglesPerTile * 3 indices per each triangle
	IndexVector indices[3] =
	{
		IndexVector(trianglesPerTile * 3    ),
		IndexVector(trianglesPerTile * 3 / 4),
		IndexVector(trianglesPerTile * 3 / 8)
	};

	unsigned int vertexIndex, indexIndex = 0, tileIndex = 0;

	for (unsigned int zIndex = 0; zIndex < mTileZ - 1; ++zIndex)
	{
		for (unsigned int xIndex = 0; xIndex < mTileX - 1; ++xIndex)
		{
			indices[0][indexIndex++] =  zIndex      * mTileX + xIndex;
			indices[0][indexIndex++] = (zIndex + 1) * mTileX + xIndex;
			indices[0][indexIndex++] =  zIndex      * mTileX + xIndex + 1;

			indices[0][indexIndex++] = (zIndex + 1) * mTileX + xIndex;
			indices[0][indexIndex++] = (zIndex + 1) * mTileX + xIndex + 1;
			indices[0][indexIndex++] =  zIndex      * mTileX + xIndex + 1;
		}
	}

	indexIndex = 0;

	for (unsigned int zIndex = 0; zIndex < mTileZ - 1; zIndex += 2)
	{
		for (unsigned int xIndex = 0; xIndex < mTileX - 1; xIndex += 2)
		{
			indices[1][indexIndex++] =  zIndex      * mTileX + xIndex;
			indices[1][indexIndex++] = (zIndex + 2) * mTileX + xIndex;
			indices[1][indexIndex++] =  zIndex      * mTileX + xIndex + 2;

			indices[1][indexIndex++] = (zIndex + 2) * mTileX + xIndex;
			indices[1][indexIndex++] = (zIndex + 2) * mTileX + xIndex + 2;
			indices[1][indexIndex++] =  zIndex      * mTileX + xIndex + 2;
		}
	}

	indexIndex = 0;

	for (unsigned int zIndex = 0; zIndex < mTileZ - 1; zIndex += 4)
	{
		for (unsigned int xIndex = 0; xIndex < mTileX - 1; xIndex += 4)
		{
			indices[2][indexIndex++] =  zIndex      * mTileX + xIndex;
			indices[2][indexIndex++] = (zIndex + 4) * mTileX + xIndex;
			indices[2][indexIndex++] =  zIndex      * mTileX + xIndex + 4;

			indices[2][indexIndex++] = (zIndex + 4) * mTileX + xIndex;
			indices[2][indexIndex++] = (zIndex + 4) * mTileX + xIndex + 4;
			indices[2][indexIndex++] =  zIndex      * mTileX + xIndex + 4;
		}
	}

	float scaleX = mMap->getSize().x / (float)(sizeX - 1);
	float scaleZ = mMap->getSize().z / (float)(sizeZ - 1);

	for (unsigned int zTile = 0; zTile < (sizeZ - 1); zTile += (mTileZ - 1))
	{
		for (unsigned int xTile = 0; xTile < (sizeX - 1); xTile += (mTileX - 1))
		{
			vertexIndex = 0;

			for (unsigned int zVertex = zTile; zVertex < zTile + mTileZ; ++zVertex)
			{
				for (unsigned int xVertex = xTile; xVertex < xTile + mTileX; ++xVertex)
				{
					float yVertex = mMap->getTerrainData()[zVertex * sizeX + xVertex];

					vertices[vertexIndex++] = (float)xVertex * scaleX;
					vertices[vertexIndex++] = (float)yVertex;
					vertices[vertexIndex++] = (float)zVertex * scaleZ;

					vertices[vertexIndex++] = (float)xVertex / (float)(sizeX - 1);
					vertices[vertexIndex++] = (float)zVertex / (float)(sizeZ - 1);

					vertices[vertexIndex++] = 1.0f;
					vertices[vertexIndex++] = 1.0f;
					vertices[vertexIndex++] = 1.0f;

					vertices[vertexIndex++] = 0.0f;
					vertices[vertexIndex++] = 1.0f;
					vertices[vertexIndex++] = 0.0f;
				}
			}

			std::string name =
				"MapView_" + Convert::ToString(mID) +
				"_Tile_" + Convert::ToString(tileIndex);

			// Create mesh.
			mMeshes[tileIndex] = Ogre::MeshManager::getSingleton().createManual(
				name, "General").get();

			// Create one submesh.
			Ogre::SubMesh* subMesh = mMeshes[tileIndex]->createSubMesh();

			// Create vertex data structure for vertices shared between submeshes.
			mMeshes[tileIndex]->sharedVertexData = new Ogre::VertexData();
			mMeshes[tileIndex]->sharedVertexData->vertexCount = vertexPerTile;

			// Create declaration (memory format) of vertex data.
			Ogre::VertexDeclaration* decl =
				mMeshes[tileIndex]->sharedVertexData->vertexDeclaration;

			size_t offset = 0;

			// 1st buffer
			decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
			offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

			decl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
			offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);

			decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_DIFFUSE);
			offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

			decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
			offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

			// Allocate vertex buffer of the requested number of vertices (vertexPerTile)
			// and bytes per vertex (offset).
			Ogre::HardwareVertexBufferSharedPtr vbuf =
				Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
				offset, vertexPerTile, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

			// Upload the vertex data to the card
			vbuf->writeData(0, vbuf->getSizeInBytes(), &(vertices.front()), true);

			// Set vertex buffer binding so buffer 0 is bound to our vertex buffer.
			Ogre::VertexBufferBinding* bind =
				mMeshes[tileIndex]->sharedVertexData->vertexBufferBinding; 
			bind->setBinding(0, vbuf);

			// Allocate index buffer of the requested number of vertices .
			Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().
				createIndexBuffer(
				Ogre::HardwareIndexBuffer::IT_16BIT, 
				trianglesPerTile * 3,
				Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

			// Upload the index data to the card.
			ibuf->writeData(0, ibuf->getSizeInBytes(), &(indices[0].front()), true);

			/// Set parameters of the submesh
			subMesh->useSharedVertices = true;
			subMesh->indexData->indexBuffer = ibuf;
			subMesh->indexData->indexCount = indices[0].size();
			subMesh->indexData->indexStart = 0;
			subMesh->setMaterialName("terrain");

			float xMin = vertices[0];
			float yMin = vertices[1];
			float zMin = vertices[2];

			float xMax = vertices[0];
			float yMax = vertices[1];
			float zMax = vertices[2];

			for (unsigned int i = 0; i < vertexPerTile * 11; i += 11)
			{
				if (vertices[i    ] < xMin) xMin = vertices[i    ]; else
				if (vertices[i    ] > xMax) xMax = vertices[i    ];

				if (vertices[i + 1] < yMin) yMin = vertices[i + 1]; else
				if (vertices[i + 1] > yMax) yMax = vertices[i + 1];

				if (vertices[i + 2] < zMin) zMin = vertices[i + 2]; else
				if (vertices[i + 2] > zMax) zMax = vertices[i + 2];
			}

			// Set bounding information (for culling).
			mMeshes[tileIndex]->_setBounds(Ogre::AxisAlignedBox(xMin, yMin, zMin, xMax, yMax, zMax));
			mMeshes[tileIndex]->_setBoundingSphereRadius(1.0f);

			// Notify Mesh object that it has been loaded.
			mMeshes[tileIndex]->load();

			// Create entity.
			mEntities[tileIndex] = createEntity(name, name);
			mEntities[tileIndex]->setCastShadows(false);
			mEntities[tileIndex]->setUserAny(Ogre::Any(this));

			// Create scene node.
			mSceneNodes[tileIndex] = createSceneNode();
			mSceneNodes[tileIndex]->attachObject(mEntities[tileIndex]);

			// Advance to next tile.
			tileIndex++;
		}
	}
}

void MapView::createTerrainFromFile()
{
	if (mMap->getTexture().size())
	{
		mMaterial = (Ogre::MaterialPtr)
			Ogre::MaterialManager::getSingleton().getByName(mMap->getTexture());

		if (mMaterial.isNull())
		{
			mMaterial = (Ogre::MaterialPtr)
				Ogre::MaterialManager::getSingleton().create(mMap->getTexture(), "General");
			mMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(mMap->getTexture());
			mMaterial->getTechnique(0)->getPass(0)->setVertexColourTracking(Ogre::TVC_AMBIENT);
		}
	}
	else
	{
		mMaterial = (Ogre::MaterialPtr)
			Ogre::MaterialManager::getSingleton().getByName("terrain");
	}

	Ogre::Image img;
	Ogre::uchar* data = new Ogre::uchar[mMap->getTerrainData().size()];

	float min = mMap->getTerrainData()[0];
	float max = mMap->getTerrainData()[0];

	for (size_t i = 1; i < mMap->getTerrainData().size(); ++i)
	{
		if (mMap->getTerrainData()[i] < min) min = mMap->getTerrainData()[i];
		else if (mMap->getTerrainData()[i] > max) max = mMap->getTerrainData()[i];
	}

	for (size_t i = 0; i < mMap->getTerrainData().size(); ++i)
	{
		data[i] = mMap->getTerrainData()[i];
	}

	img.loadDynamicImage(
		data,
		mMap->getTerrainSize().x,
		mMap->getTerrainSize().z,
		1, Ogre::PF_L8);

	// Save bitmap file.
	std::string name(Convert::ToString(mMap->getID()));
	img.save("..\\cache\\" + name + ".png");

	// Create config file.
	std::ofstream config(
		("..\\cache\\" + name + ".cfg").c_str(),
		std::ios::out | std::ios::trunc);

	config << "DetailTile=3" << "\n";
	config << "PageSource=Heightmap" << "\n";
	config << "Heightmap.image=" << "..\\cache\\" << name << ".png" << "\n";
	config << "PageSize=" << mMap->getTerrainSize().x << "\n";
	config << "TileSize=33" << "\n";
	config << "MaxPixelError=3" << "\n";
	config << "PageWorldX=" << mMap->getSize().x << "\n";
	config << "PageWorldZ=" << mMap->getSize().z << "\n";
	config << "MaxHeight=255" << "\n";
	config << "MaxMipMapLevel=5" << "\n";
	config << "VertexNormals=yes" << "\n";
	config << "VertexColors=yes" << "\n";
	config << "VertexProgramMorph=yes" << "\n";
	config << "LODMorphStart=0.2" << "\n";
	config << "CustomMaterialName=" << mMaterial->getName() << "\n";

	config.close();

	delete[] data;

	mManager->setWorldGeometry("..\\cache\\" + name + ".cfg");
}

void MapView::destroyTerrain()
{
	mMeshes.clear();
	mEntities.clear();
	mSceneNodes.clear();
}

void MapView::createObjects()
{
	Render::getSingleton().select(this);
	Ogre::Root::getSingleton().renderOneFrame();

	ObjectList::const_iterator i = mMap->getObjects().begin();
	for (; i != mMap->getObjects().end(); ++i)
	{
		const ObjectType types[5] =
		{
			OT_STREET,
			OT_BUILDING,
			OT_PLANT,
			OT_WATER_OBJECT,
			OT_ROUTE
		};

		for (int j = 0; j < 5; ++j)
		{
			if (i->second->is(types[j]) && ((mObjectsLoaded & types[j]) == types[j]))
			{
				createView(i->second);

				break;
			}
		}
	}

	// Create junctions. Have to be loaded after routes.
	{
		Ogre::Root::getSingleton().renderOneFrame();

		ObjectList::const_iterator i = mMap->getObjects().begin();
		for (; i != mMap->getObjects().end(); ++i)
		{
			const ObjectType types[1] =
			{
				OT_JUNCTION
			};

			for (int j = 0; j < 1; ++j)
			{
				if (i->second->is(types[j]) && ((mObjectsLoaded & types[j]) == types[j]))
				{
					createView(i->second);

					break;
				}
			}
		}
	}
}

void MapView::destroyObjects()
{
	ObjectViewList::const_iterator i = mObjectViews.begin();

	for (; i != mObjectViews.end(); ++i)
	{
		delete i->second;
	}

	mObjectViews.clear();
}

const Ogre::SceneQueryResult& MapView::raycast(
	const Ogre::PlaneBoundedVolumeList& volumes) const
{
	mVolumeQuery->setVolumes(volumes);
	return mVolumeQuery->execute();
}

void MapView::startShow()
{
	if (mPointList.size() > 1)
	{
		std::vector<Ogre::Vector3>::iterator i = mPointList.begin();
		for (; i != mPointList.end(); ++i)
		{
			mSpline.addPoint(*i);
		}

		mCameraSpeed = 10.0f;
		mCounter = 1;
		mIsShowRunning = true;

		mCurrent = mSpline.interpolate(0);
		mNext = mSpline.interpolate(1.0f / 100.0f);
	}
}

void MapView::update(float time)
{
	if (mIsShowRunning)
	{
		if (mCounter > 100)
		{
			mIsShowRunning = false;
		}

		if (mCurrent.distance(mNext) < 1.0f)
		{
			++mCounter;

			mNext = mSpline.interpolate(mCounter / 100.0f);
		}

		mCurrent += (mNext - mCurrent).normalisedCopy() * mCameraSpeed * time;

		mCamera->setPosition(
			mCurrent + Ogre::Vector3(0, 10.0f, 0.0f));
	}
}
