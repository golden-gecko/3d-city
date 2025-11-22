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
#include "Plant.h"
#include "PlantView.h"
#include "Database.h"
#include "Map.h"
#include "MapView.h"

bool PlantView::load()
{
	if (mIsLoaded)
	{
	//	throw EXCEPTION("View is already loaded.");

		return false;
	}

	if (mPlant->getPointCount() < 3)
	{
		return false;
	}

	mPlant->setHeight(1.0f);

	unsigned int verticesCount = mPlant->getPointCount() * 2;
	unsigned int verticesSize = verticesCount * 6;
	unsigned int indicesSize = (mPlant->getPointCount() * 2 + mPlant->getPointCount() - 2) * 3;

	Ogre::Real* vertices  = new Ogre::Real[verticesSize];
	Ogre::uint16* indices = new Ogre::uint16[indicesSize];

	unsigned int i, index = 0;

	// Prepare vertices.
	for (i = 0; i < mPlant->getPointCount(); ++i)
	{
		const Point3D& point3D = mPlant->getPoint(i);

		vertices[index++] = point3D.x;
		vertices[index++] = point3D.y;
		vertices[index++] = point3D.z;

		vertices[index++] = 1;
		vertices[index++] = 0;
		vertices[index++] = 1;

		vertices[index++] = point3D.x;
		vertices[index++] = point3D.y + mPlant->getHeight();
		vertices[index++] = point3D.z;

		vertices[index++] = 0;
		vertices[index++] = 1;
		vertices[index++] = 0;
	}

	index = 0;

	// Create indices.
	for (i = 0; i < verticesCount - 2; i += 2)
	{
		indices[index++] = (Ogre::uint16)i + 2;
		indices[index++] = (Ogre::uint16)i;
		indices[index++] = (Ogre::uint16)i + 1;

		indices[index++] = (Ogre::uint16)i + 3;
		indices[index++] = (Ogre::uint16)i + 2;
		indices[index++] = (Ogre::uint16)i + 1;
	}

	// Last wall.
	indices[index++] = (Ogre::uint16)verticesCount - 2;
	indices[index++] = (Ogre::uint16)verticesCount - 1;
	indices[index++] = (Ogre::uint16)0;

	indices[index++] = (Ogre::uint16)1;
	indices[index++] = (Ogre::uint16)0;
	indices[index++] = (Ogre::uint16)verticesCount - 1;

	// Roof.
	unsigned short v = 3;

	for (i = 0; i < mPlant->getPointCount() - 2; ++i)
	{
		indices[index++] = v + 2;
		indices[index++] = v;
		indices[index++] = 1;

		v += 2;
	}

	// Create mesh.
	mMesh = Ogre::MeshManager::getSingleton().createManual(
		Ogre::StringConverter::toString(mID), "General").get();

	// Create one submesh.
	Ogre::SubMesh* sub = mMesh->createSubMesh();

	// Create vertex data structure for 8 vertices shared between submeshes.
	mMesh->sharedVertexData = new Ogre::VertexData();
	mMesh->sharedVertexData->vertexCount = verticesCount;

	// Create declaration (memory format) of vertex data.
	Ogre::VertexDeclaration* decl = mMesh->sharedVertexData->vertexDeclaration;
	size_t offset = 0;

	// 1st buffer
	decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	/// Allocate vertex buffer of the requested number of vertices (vertexCount) 
	/// and bytes per vertex (offset)
	Ogre::HardwareVertexBufferSharedPtr vbuf =
		Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			offset, verticesCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	/// Upload the vertex data to the card
	vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);

	/// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
	Ogre::VertexBufferBinding* bind =
		mMesh->sharedVertexData->vertexBufferBinding; 
	bind->setBinding(0, vbuf);

	/// Allocate index buffer of the requested number of vertices (ibufCount).
	Ogre::HardwareIndexBufferSharedPtr ibuf =
		Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
			Ogre::HardwareIndexBuffer::IT_16BIT, 
			indicesSize, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	/// Upload the index data to the card.
	ibuf->writeData(0, ibuf->getSizeInBytes(), indices, true);

	/// Set parameters of the submesh.
	sub->useSharedVertices = true;
	sub->indexData->indexBuffer = ibuf;
	sub->indexData->indexCount = indicesSize;
	sub->indexData->indexStart = 0;
	sub->setMaterialName("green");

	float xMin = vertices[0];
	float yMin = vertices[1];
	float zMin = vertices[2];

	float xMax = vertices[0];
	float yMax = vertices[1];
	float zMax = vertices[2];

	for (i = 0; i < verticesSize; i += verticesSize / verticesCount)
	{
		if (vertices[i    ] < xMin) xMin = vertices[i    ]; else
		if (vertices[i    ] > xMax) xMax = vertices[i    ];

		if (vertices[i + 1] < yMin) yMin = vertices[i + 1]; else
		if (vertices[i + 1] > yMax) yMax = vertices[i + 1];

		if (vertices[i + 2] < zMin) zMin = vertices[i + 2]; else
		if (vertices[i + 2] > zMax) zMax = vertices[i + 2];
	}

	/// Set bounding information (for culling).
	mMesh->_setBounds(Ogre::AxisAlignedBox(xMin, yMin, zMin, xMax, yMax, zMax));
	mMesh->_setBoundingSphereRadius(1.0f);

	/// Notify Mesh object that it has been loaded.
	mMesh->load();

	mEntity = mMapView->createEntity(mMesh->getName(), mMesh->getName());
	mEntity->setUserAny(Ogre::Any(this));

	mSceneNode = mMapView->createSceneNode();

	if (mShowShape)
	{
		mSceneNode->attachObject(mEntity);
	}

	//

	mStaticGeometry = mMapView->createStaticGeometry(Convert::ToString(mID));

	mEntities[0] = mMapView->createEntity(Convert::ToString(mID) + "_1", "bigtree1.mesh");
	mEntities[1] = mMapView->createEntity(Convert::ToString(mID) + "_2", "bigtree2.mesh");
	mEntities[2] = mMapView->createEntity(Convert::ToString(mID) + "_3", "pinetree.mesh");

	mEntities[0]->setUserAny(Ogre::Any(this));
	mEntities[1]->setUserAny(Ogre::Any(this));
	mEntities[2]->setUserAny(Ogre::Any(this));

	mStaticGeometry->setRegionDimensions(Ogre::Vector3(100, 100, 100));
	mStaticGeometry->addSceneNode(mSceneNode);

	for (int z = zMin + 5.0f; z < zMax - 5.0f; z += 20)
	{
		for (int x = xMin + 5.0f; x < xMax - 5.0f; x += 20)
		{
			int r = rand() % 3;

			float xx = x + Ogre::Math::RangeRandom(-5, 5);
			float zz = z + Ogre::Math::RangeRandom(-5, 5);

			Ogre::Vector3 p(xx, mMapView->raycastTerrainPoint3D(Point3D(xx, 0.0f, zz)).y, zz);

			Ogre::Vector3 scale
				(20.0f/*mPlant->getHeight()*/ / mEntities[r]->getBoundingBox().getSize().y);

			for (size_t i = indicesSize - (mPlant->getPointCount() - 2) * 3; i < indicesSize; i += 3)
			{
				size_t vertexSize = verticesSize / verticesCount;

				Ogre::Vector3 a, b, c;

				a.y = b.y = c.y = 0.0f;

				a.x = vertices[indices[i    ] * vertexSize    ];
				a.z = vertices[indices[i    ] * vertexSize + 2];

				b.x = vertices[indices[i + 1] * vertexSize    ];
				b.z = vertices[indices[i + 1] * vertexSize + 2];

				c.x = vertices[indices[i + 2] * vertexSize    ];
				c.z = vertices[indices[i + 2] * vertexSize + 2];

				//if (Math::PointInTriangle(a, b, c, position))
				if (Math::PointInPolygon(mPlant->getPoints(), Point3D(p.x, p.y, p.z)))
				{
					mStaticGeometry->addEntity(
						mEntities[r], p, Ogre::Quaternion::IDENTITY, scale);

					break;
				}
			}
		}
	}

	mStaticGeometry->build();

	//

	delete [] vertices;
	delete [] indices;

	//

	return mIsLoaded = true;
}

void PlantView::unload()
{
	if (mIsLoaded)
	{
		mIsLoaded = false;

		mMapView->destroyStaticGeometry(mStaticGeometry);
		mMapView->destroySceneNode(mSceneNode);
		mMapView->destroyEntity(mEntity);

		mMapView->destroyEntity(mEntities[0]);
		mMapView->destroyEntity(mEntities[1]);
		mMapView->destroyEntity(mEntities[2]);

		Ogre::MeshManager::getSingleton().remove(mMesh->getHandle());
	}
}

bool PlantView::update()
{
	unload();

	return load();
}
