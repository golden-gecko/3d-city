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
#include "JunctionView.h"
#include "MapView.h"
#include "RouteView.h"

bool JunctionView::load()
{
	if (mIsLoaded)
	{
		return false;			
	}

	if (mJunction->getMap() != mMapView->getMap())
	{
		return false;
	}

	// Verify routes.
	mJunction->updateRoutes();

	// We need three or more routes to create junction.
	if (mJunction->getRouteCount() < 3)
	{
		return false;
	}

	// Allocate memory for mesh data.
	unsigned int verticesCount = mJunction->getRouteCount() * 2 + 1;
	unsigned int verticesSize = verticesCount * 5;
	unsigned int indicesSize = mJunction->getRouteCount() * 6;

	VertexVector vertices(verticesSize);
	IndexVector indices(indicesSize);

	Ogre::Vector3 center(Ogre::Vector3::ZERO), *p;
	unsigned int index = 0;

	// Get nearest point from each route.
	RouteList::const_iterator i = mJunction->getRoutes().begin();
	for (; i != mJunction->getRoutes().end(); ++i)
	{
		p = mMapView->getView<RouteView>(i->second)->getNearest(mJunction->getCenter());

		center += p[1];

		p[0] = mMapView->raycastTerrainPoint3D(p[0]);
		p[2] = mMapView->raycastTerrainPoint3D(p[2]);

		vertices[index++] = p[2].x;
		vertices[index++] = p[2].y + 0.2f;
		vertices[index++] = p[2].z;

		vertices[index++] = p[2].x;
		vertices[index++] = p[2].z;

		vertices[index++] = p[0].x;
		vertices[index++] = p[0].y + 0.2f;
		vertices[index++] = p[0].z;

		vertices[index++] = p[0].x;
		vertices[index++] = p[0].z;
	}

	// Add center point.
	center /= mJunction->getRouteCount();

	center = mMapView->raycastTerrainPoint3D(center);

	vertices[index++] = center.x;
	vertices[index++] = center.y + 0.2f;
	vertices[index++] = center.z;

	vertices[index++] = center.x;
	vertices[index++] = center.z;

	index = 0;

	// Create indices.
	for (unsigned int i = 0; i < verticesCount - 3; i += 2)
	{
		indices[index++] = (Ogre::uint16)i;
		indices[index++] = (Ogre::uint16)i + 1;
		indices[index++] = (Ogre::uint16)verticesCount - 1;

		//indices[index++] = (Ogre::uint16)i + 1;
		//indices[index++] = (Ogre::uint16)i + 2;
		//indices[index++] = (Ogre::uint16)verticesCount - 1;
	}

	// Last triangles.
	indices[index++] = (Ogre::uint16)verticesCount - 3;
	indices[index++] = (Ogre::uint16)verticesCount - 2;
	indices[index++] = (Ogre::uint16)verticesCount - 1;

	//indices[index++] = (Ogre::uint16)verticesCount - 2;
	//indices[index++] = (Ogre::uint16)0;
	//indices[index++] = (Ogre::uint16)verticesCount - 1;

	indicesSize /= 2;

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
			offset, verticesCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	/// Upload the vertex data to the card
	vbuf->writeData(0, vbuf->getSizeInBytes(), &(vertices[0]), true);

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
	ibuf->writeData(0, ibuf->getSizeInBytes(), &(indices[0]), true);

	/// Set parameters of the submesh.
	sub->useSharedVertices = true;
	sub->indexData->indexBuffer = ibuf;
	sub->indexData->indexCount = indicesSize;
	sub->indexData->indexStart = 0;
	sub->setMaterialName("junction");

	float xMin = vertices[0];
	float yMin = vertices[1];
	float zMin = vertices[2];

	float xMax = vertices[0];
	float yMax = vertices[1];
	float zMax = vertices[2];

	for (unsigned int i = 0; i < verticesSize; i += verticesSize / verticesCount)
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

	mEntity = mMapView->createEntity(mMesh->getName(), mMesh->getName());
	mEntity->setUserAny(Ogre::Any(this));

	mSceneNode = mMapView->createSceneNode();
	mSceneNode->attachObject(mEntity);

	return mIsLoaded = true;
}

void JunctionView::unload()
{
	if (mIsLoaded)
	{
		mIsLoaded = false;
	
		mMapView->destroySceneNode(mSceneNode);
		mMapView->destroyEntity(mEntity);

		Ogre::MeshManager::getSingleton().remove(mMesh->getHandle());
	}
}

bool JunctionView::update()
{
	unload();

	return load();
}
