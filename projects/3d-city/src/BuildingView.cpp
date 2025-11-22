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
#include "Building.h"
#include "BuildingView.h"
#include "Database.h"
#include "Map.h"
#include "MapView.h"

bool BuildingView::load()
{
	if (mIsLoaded)
	{
		return false;
	}

	if (mBuilding->getTexture().size())
	{
		mMaterial = (Ogre::MaterialPtr)
			Ogre::MaterialManager::getSingleton().getByName(mBuilding->getTexture());

		if (mMaterial.isNull())
		{
			mMaterial = (Ogre::MaterialPtr)
				Ogre::MaterialManager::getSingleton().create(mBuilding->getTexture(), "General");
			mMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(mBuilding->getTexture());
			mMaterial->getTechnique(0)->getPass(0)->setCullingMode(Ogre::CULL_NONE);
		}
	}
	else
	{
		mMaterial = (Ogre::MaterialPtr)
			Ogre::MaterialManager::getSingleton().getByName("building");
	}

	unsigned int verticesCount = mBuilding->getPointCount() * 2 + 2;
	unsigned int verticesSize = verticesCount * 8;
	unsigned int indicesSize = (mBuilding->getPointCount() * 2 + mBuilding->getPointCount() - 2) * 3;

	Ogre::Real* vertices  = new Ogre::Real[verticesSize];
	Ogre::uint16* indices = new Ogre::uint16[indicesSize];

	unsigned int i, index = 0;
	int u = 0;

	// Prepare vertices.
	for (i = 0; i < mBuilding->getPointCount(); ++i)
	{
		const Point3D& point3D = mBuilding->getPoint(i);

		vertices[index++] = point3D.x;
		vertices[index++] = point3D.y;
		vertices[index++] = point3D.z;

		vertices[index++] = (float)u;
		vertices[index++] = 1.0f;

		vertices[index++] = 1;
		vertices[index++] = 0;
		vertices[index++] = 1;

		vertices[index++] = point3D.x;
		vertices[index++] = point3D.y + mBuilding->getHeight();
		vertices[index++] = point3D.z;

		vertices[index++] = (float)u;
		vertices[index++] = 0;//1.0f - 1.0f / (float)mBuilding->getPointCount();

		vertices[index++] = 0;
		vertices[index++] = 1;
		vertices[index++] = 0;

		//u += 1.0f / mBuilding->getPointCount();
		u = (u == 1 ? 0 : 1);
	}

	const Point3D& point3D = mBuilding->getPoint(0);

	vertices[index++] = point3D.x;
	vertices[index++] = point3D.y;
	vertices[index++] = point3D.z;

	vertices[index++] = (float)u;
	vertices[index++] = 1.0f;

	vertices[index++] = 1;
	vertices[index++] = 0;
	vertices[index++] = 1;

	vertices[index++] = point3D.x;
	vertices[index++] = point3D.y + mBuilding->getHeight();
	vertices[index++] = point3D.z;

	vertices[index++] = (float)u;
	vertices[index++] = 0;//1.0f - 1.0f / (float)mBuilding->getPointCount();

	vertices[index++] = 0;
	vertices[index++] = 1;
	vertices[index++] = 0;

	index = 0;

	// Create indices.
	for (i = 0; i < verticesCount - 4; i += 2)
	{
		indices[index++] = (Ogre::uint16)i + 2;
		indices[index++] = (Ogre::uint16)i;
		indices[index++] = (Ogre::uint16)i + 1;

		indices[index++] = (Ogre::uint16)i + 3;
		indices[index++] = (Ogre::uint16)i + 2;
		indices[index++] = (Ogre::uint16)i + 1;
	}

	// Last wall.
	indices[index++] = (Ogre::uint16)verticesCount - 4;
	indices[index++] = (Ogre::uint16)verticesCount - 3;
	indices[index++] = (Ogre::uint16)0;

	indices[index++] = (Ogre::uint16)1;
	indices[index++] = (Ogre::uint16)0;
	indices[index++] = (Ogre::uint16)verticesCount - 3;

	// Roof.
	unsigned short v = 3;

	for (i = 0; i < mBuilding->getPointCount() - 2; ++i)
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

	decl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);

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
	sub->indexData->indexCount = mBuilding->getPointCount() * 6;
	sub->indexData->indexStart = 0;
	sub->setMaterialName(mMaterial->getName());

	/// Set parameters of the submesh.
	sub = mMesh->createSubMesh();
	sub->useSharedVertices = true;
	sub->indexData->indexBuffer = ibuf;
	sub->indexData->indexCount = (mBuilding->getPointCount() - 2) * 3;
	sub->indexData->indexStart = mBuilding->getPointCount() * 6;
	sub->setMaterialName("building_roof");

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

	// Set bounding information (for culling).
	mMesh->_setBounds(Ogre::AxisAlignedBox(xMin, yMin, zMin, xMax, yMax, zMax));
	mMesh->_setBoundingSphereRadius(1.0f);

	// Notify Mesh object that it has been loaded.
	mMesh->load();
	
	Ogre::MeshSerializer serializer;
	serializer.exportMesh(
		mMesh, "..\\cache\\building_view_" + Convert::ToString(mID) + ".mesh");

	// Build egde list for shadows.
	// mMesh->buildEdgeList();

	delete [] vertices;
	delete [] indices;

	mEntity = mMapView->createEntity(mMesh->getName(), mMesh->getName());
	mEntity->setUserAny(Ogre::Any(this));

	mSceneNode = mMapView->createSceneNode();
	mSceneNode->attachObject(mEntity);

	return mIsLoaded = true;
}

void BuildingView::unload()
{
	if (mIsLoaded)
	{
		mIsLoaded = false;

		mMapView->destroySceneNode(mSceneNode);
		mMapView->destroyEntity(mEntity);

		Ogre::MeshManager::getSingleton().remove(mMesh->getHandle());
	}
}

bool BuildingView::update()
{
	unload();

	return load();
}
