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
#include "WaterObject.h"
#include "WaterObjectView.h"
#include "Database.h"
#include "Map.h"
#include "MapView.h"

bool WaterObjectView::load()
{
	if (mIsLoaded)
	{
		return false;			
	}

	if (mWaterObject->getMap() != mMapView->getMap())
	{
		return false;
	}

	if (mWaterObject->getPointCount() == 1)
	{
		createPoint();
	}
	else if (mWaterObject->getIsRiver() || (mWaterObject->getPointCount() == 2))
	{
		createLine();
	}
	else
	{
		createPolygon();
	}

	mEntity = mMapView->createEntity(mMesh->getName(), mMesh->getName());
	mEntity->setUserAny(Ogre::Any(this));

	mSceneNode = mMapView->createSceneNode();
	mSceneNode->attachObject(mEntity);

	return mIsLoaded = true;
}

void WaterObjectView::unload()
{
	if (mIsLoaded)
	{
		mIsLoaded = false;

		mMapView->destroySceneNode(mSceneNode);
		mMapView->destroyEntity(mEntity);

		Ogre::MeshManager::getSingleton().remove(mMesh->getHandle());
	}
}

bool WaterObjectView::update()
{
	unload();

	return load();
}

void WaterObjectView::createPoint()
{
}

void WaterObjectView::createLine()
{
	size_t verticesCount = ((mWaterObject->getPointCount() - 1) * mSegments + 1) * 2;
	size_t verticesSize = verticesCount * 5;
	size_t indicesSize = ((mWaterObject->getPointCount() - 1) * mSegments) * 6;

	verticesCount = mWaterObject->getPointCount() * 2;
	verticesSize = verticesCount * 5;
	indicesSize = (mWaterObject->getPointCount() - 1) * 6;

	Ogre::Real* vertices  = new Ogre::Real[verticesSize];
	Ogre::uint16* indices = new Ogre::uint16[indicesSize];

	unsigned int i, index = 0;

	// Prepare vertices.
	for (i = 0; i < mWaterObject->getPointCount(); ++i)
	{
		const Point3D& point3D = mWaterObject->getPoint(i);

		vertices[index++] = point3D.x - mWaterObject->getWidth() * 0.5f;
		vertices[index++] = point3D.y;
		vertices[index++] = point3D.z + 1.0f;

		vertices[index++] = point3D.x - mWaterObject->getWidth() * 0.5f;;
		vertices[index++] = point3D.z;

		vertices[index++] = point3D.x + mWaterObject->getWidth() * 0.5f;
		vertices[index++] = point3D.y;
		vertices[index++] = point3D.z + 1.0f;

		vertices[index++] = point3D.x + mWaterObject->getWidth() * 0.5f;
		vertices[index++] = point3D.z;
	}

	index = 0;

	// Prepare indices.
	for (i = 0; i < verticesCount - 2; i += 2)
	{
		indices[index++] = (Ogre::uint16)i;
		indices[index++] = (Ogre::uint16)i + 2;
		indices[index++] = (Ogre::uint16)i + 1;

		indices[index++] = (Ogre::uint16)i + 2;
		indices[index++] = (Ogre::uint16)i + 3;
		indices[index++] = (Ogre::uint16)i + 1;
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

	/// Allocate vertex buffer of the requested number of vertices (vertexCount) 
	/// and bytes per vertex (offset)
	Ogre::HardwareVertexBufferSharedPtr vbuf =
		Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		offset, mMesh->sharedVertexData->vertexCount,
		Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	/// Upload the vertex data to the card
	vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);

	/// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
	Ogre::VertexBufferBinding* bind =
		mMesh->sharedVertexData->vertexBufferBinding; 
	bind->setBinding(0, vbuf);

	/// Allocate index buffer of the requested number of vertices (ibufCount).
	Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().
		createIndexBuffer(
		Ogre::HardwareIndexBuffer::IT_16BIT, 
		indicesSize, 
		Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	/// Upload the index data to the card.
	ibuf->writeData(0, ibuf->getSizeInBytes(), indices, true);

	/// Set parameters of the submesh.
	sub->useSharedVertices = true;
	sub->indexData->indexBuffer = ibuf;
	sub->indexData->indexCount = indicesSize;
	sub->indexData->indexStart = 0;
	sub->setMaterialName("Ocean2_Cg");

	float xMin = vertices[0];
	float yMin = vertices[1];
	float zMin = vertices[2];

	float xMax = vertices[0];
	float yMax = vertices[1];
	float zMax = vertices[2];

	for (i = 0; i < verticesSize; i += 5)
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

	delete [] vertices;
	delete [] indices;
}

void WaterObjectView::createPolygon()
{
	size_t verticesCount = mWaterObject->getPointCount();
	size_t verticesSize = verticesCount * 8;
	size_t indicesSize = (mWaterObject->getPointCount() - 2) * 3;

	Ogre::Real* vertices  = new Ogre::Real[verticesSize];
	Ogre::uint16* indices = new Ogre::uint16[indicesSize];

	unsigned int index = 0;

	// Prepare vertices.
	for (unsigned int i = 0; i < mWaterObject->getPointCount(); ++i)
	{
		const Point3D& point3D = mWaterObject->getPoint(i);

		vertices[index++] = point3D.x;
		vertices[index++] = point3D.y + 0.5f;
		vertices[index++] = point3D.z;

		vertices[index++] = point3D.x * 0.01f;
		vertices[index++] = point3D.z * 0.01f;

		vertices[index++] = 0;
		vertices[index++] = 0;
		vertices[index++] = 1;
	}

	index = 0;

	for (Ogre::uint16 i = 1; i < mWaterObject->getPointCount() - 1; ++i)
	{
		indices[index++] = 0;
		indices[index++] = i;
		indices[index++] = i + 1;
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
	sub->indexData->indexCount = indicesSize;
	sub->indexData->indexStart = 0;
	sub->setMaterialName("Ocean2_Cg");

	float xMin = vertices[0];
	float yMin = vertices[1];
	float zMin = vertices[2];

	float xMax = vertices[0];
	float yMax = vertices[1];
	float zMax = vertices[2];

	for (size_t i = 0; i < verticesSize; i += verticesSize / verticesCount)
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

	// Build egde list for shadows.
	// mMesh->buildEdgeList();

	delete [] vertices;
	delete [] indices;
}
