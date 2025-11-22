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
#include "Route.h"
#include "RouteView.h"
#include "MapView.h"

bool RouteView::load()
{
	if (mIsLoaded)
	{
		return false;			
	}

	if (mRoute->getMap() != mMapView->getMap())
	{
		return false;
	}

	if (mSegments < mRoute->getPointCount())
	{
		mSegments = mRoute->getPointCount();
	}

	mLightStatic = mMapView->createStaticGeometry(
		"RouteView_" + Convert::ToString(mID) + "_Static");
	mLightStatic->setRegionDimensions(Ogre::Vector3(100.0f));

	mLightEntity = mMapView->createEntity(
		"RouteView_" + Convert::ToString(mID) + "_Entity", "s-light2.mesh");

	unsigned int verticesCount = mSegments * 2;
	unsigned int verticesSize = verticesCount * 5;
	unsigned int indicesSize = mSegments * 6;

	VertexVector vertices(verticesSize);
	IndexVector indices(indicesSize);

	unsigned int i, index = 0;
	Ogre::Vector3 curr, next, segment, leftPoint, rightPoint;

	int u = 0;

	// Prepare vertices.
	Ogre::SimpleSpline spline;

	for (i = 0; i < mRoute->getPointCount(); ++i)
	{
		const Point3D& p = mRoute->getPoint(i);

		spline.addPoint(Ogre::Vector3(p.x, p.y, p.z));
	}

	mSceneNode = mMapView->createSceneNode();

	bool firstSet = false;

	for (i = 0; i < (unsigned short)(mSegments - 1); ++i)
	{
		curr = spline.interpolate((float)i / (float)mSegments);
		next = spline.interpolate((float)(i + 1) / (float)mSegments);

		segment = next - curr;
		segment.normalise();

		leftPoint = segment.crossProduct(Ogre::Vector3::NEGATIVE_UNIT_Y);
		leftPoint.normalise();
		leftPoint *= mRoute->getWidth() * 0.5f;
		leftPoint += curr;

		rightPoint = segment.crossProduct(Ogre::Vector3::UNIT_Y);
		rightPoint.normalise();
		rightPoint *= mRoute->getWidth() * 0.5f;
		rightPoint += curr;

		leftPoint = mMapView->raycastTerrainPoint3D(leftPoint);
		rightPoint = mMapView->raycastTerrainPoint3D(rightPoint);

		if (firstSet == false)
		{
			firstSet = true;

			mFirst[0] = leftPoint;
			mFirst[1] = curr;
			mFirst[2] = rightPoint;
		}

		vertices[index++] = leftPoint.x;
		vertices[index++] = leftPoint.y + 0.2f;
		vertices[index++] = leftPoint.z;

		vertices[index++] = (float)u;
		vertices[index++] = 0.0f;

		vertices[index++] = rightPoint.x;
		vertices[index++] = rightPoint.y + 0.2f;
		vertices[index++] = rightPoint.z;

		vertices[index++] = (float)u;
		vertices[index++] = 1.0f;

		u++;

		if (i != 0 && i % 5 == 0)
		{
			mSceneNode->setOrientation(Ogre::Quaternion::IDENTITY);
			mSceneNode->setFixedYawAxis(true, Ogre::Vector3::UNIT_Y);
			mSceneNode->setDirection(Ogre::Vector3(segment.x, 0.0f, segment.z));

			mLightStatic->addEntity(
				mLightEntity,
				mMapView->raycastTerrainPoint3D(curr),
				mSceneNode->getOrientation(),
				Ogre::Vector3(0.04f));
		}
	}

	curr = spline.interpolate(1.0f);

	leftPoint = segment.crossProduct(Ogre::Vector3::NEGATIVE_UNIT_Y);
	leftPoint.normalise();
	leftPoint *= mRoute->getWidth() * 0.5f;
	leftPoint += curr;

	rightPoint = segment.crossProduct(Ogre::Vector3::UNIT_Y);
	rightPoint.normalise();
	rightPoint *= mRoute->getWidth() * 0.5f;
	rightPoint += curr;

	leftPoint = mMapView->raycastTerrainPoint3D(leftPoint);
	rightPoint = mMapView->raycastTerrainPoint3D(rightPoint);

	mLast[0] = leftPoint;
	mLast[1] = curr;
	mLast[2] = rightPoint;

	vertices[index++] = leftPoint.x;
	vertices[index++] = leftPoint.y + 0.2f;
	vertices[index++] = leftPoint.z;

	vertices[index++] = (float)u;
	vertices[index++] = 0.0f;

	vertices[index++] = rightPoint.x;
	vertices[index++] = rightPoint.y + 0.2f;
	vertices[index++] = rightPoint.z;

	vertices[index++] = (float)u;
	vertices[index++] = 1.0f;

	index = 0;

	// Prepare indices.
	for (i = 0; i < verticesCount - 2; i += 2)
	{
		indices[index++] = (Ogre::uint16)i;
		indices[index++] = (Ogre::uint16)i + 1;
		indices[index++] = (Ogre::uint16)i + 2;

		indices[index++] = (Ogre::uint16)i + 1;
		indices[index++] = (Ogre::uint16)i + 3;
		indices[index++] = (Ogre::uint16)i + 2;
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
	vbuf->writeData(0, vbuf->getSizeInBytes(), &(vertices.front()), true);

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
	ibuf->writeData(0, ibuf->getSizeInBytes(), &(indices.front()), true);

	/// Set parameters of the submesh.
	sub->useSharedVertices = true;
	sub->indexData->indexBuffer = ibuf;
	sub->indexData->indexCount = indicesSize;
	sub->indexData->indexStart = 0;

	sub->setMaterialName("route");

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

	Ogre::MeshSerializer serializer;
	serializer.exportMesh(
		mMesh, "..\\cache\\route_view_" + Convert::ToString(mID) + ".mesh");

	mEntity = mMapView->createEntity(mMesh->getName(), mMesh->getName());
	mEntity->setCastShadows(false);
	mEntity->setUserAny(Ogre::Any(this));

//	mSceneNode = mMapView->createSceneNode();
	mSceneNode->setOrientation(Ogre::Quaternion::IDENTITY);
	mSceneNode->attachObject(mEntity);

	mLightStatic->addSceneNode(mSceneNode);
	mLightStatic->build();

	return mIsLoaded = true;
}

void RouteView::unload()
{
	if (mIsLoaded)
	{
		mIsLoaded = false;
	
		mMapView->destroySceneNode(mSceneNode);
		mMapView->destroyEntity(mLightEntity);
		
		mMapView->destroyStaticGeometry(mLightStatic);
		mMapView->destroyEntity(mEntity);

		Ogre::MeshManager::getSingleton().remove(mMesh->getHandle());
	}
}

bool RouteView::update()
{
	unload();

	return load();
}
