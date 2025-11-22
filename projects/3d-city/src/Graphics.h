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

#include "Dependencies.h"
#include "Coordinates.h"

class Math
{
public:
	static bool PointInTriangle(
		const Ogre::Vector3& A,
		const Ogre::Vector3& B,
		const Ogre::Vector3& C,
		const Ogre::Vector3& P)
	{
		// Compute vectors
		Ogre::Vector3 v0 = C - A;
		Ogre::Vector3 v1 = B - A;
		Ogre::Vector3 v2 = P - A;

		// Compute dot products
		float dot00 = v0.dotProduct(v0);
		float dot01 = v0.dotProduct(v1);
		float dot02 = v0.dotProduct(v2);
		float dot11 = v1.dotProduct(v1);
		float dot12 = v1.dotProduct(v2);

		// Compute barycentric coordinates
		float invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
		float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

		// Check if point is in triangle
		return (u > 0) && (v > 0) && (u + v < 1);
	}

	static bool PointInPolygon(const PointList& points, const Point3D& p)
	{
		PointList::size_type j = points.size() - 1;
		bool oddNodes = false;

		for (PointList::size_type i = 0; i < points.size(); ++i)
		{
			if (points[i].z < p.z && points[j].z >= p.z || points[j].z < p.z && points[i].z >= p.z)
			{
					if (points[i].x + (p.z - points[i].z) / (points[j].z - points[i].z) * (points[j].x - points[i].x) < p.x)
					{
						oddNodes = !oddNodes;
					}
			}

			j = i;
		}

		return oddNodes;
	}
};

class Graphics : public Ogre::Singleton<Graphics>
{
public:
	Graphics();

	virtual ~Graphics()
	{
		delete mRoot;
	}

	void createIntro();

	Ogre::SceneManager* getSceneManager() const
	{
		return mManager;
	}
	Ogre::Camera* getCamera() const
	{
		return mCamera;
	}

	void initResources()
	{
		Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}

	static void getMeshInformation(const Ogre::MeshPtr mesh,
									size_t &vertex_count,
									Ogre::Vector3* &vertices,
									size_t &index_count,
									unsigned long* &indices,
									const Ogre::Vector3 &position,
									const Ogre::Quaternion &orient,
									const Ogre::Vector3 &scale)
	{
		bool added_shared = false;
		size_t current_offset = 0;
		size_t shared_offset = 0;
		size_t next_offset = 0;
		size_t index_offset = 0;

		vertex_count = index_count = 0;

		// Calculate how many vertices and indices we're going to need
		for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
		{
			Ogre::SubMesh* submesh = mesh->getSubMesh( i );

			// We only need to add the shared vertices once
			if(submesh->useSharedVertices)
			{
				if( !added_shared )
				{
					vertex_count += mesh->sharedVertexData->vertexCount;
					added_shared = true;
				}
			}
			else
			{
				vertex_count += submesh->vertexData->vertexCount;
			}

			// Add the indices
			index_count += submesh->indexData->indexCount;
		}


		// Allocate space for the vertices and indices
		vertices = new Ogre::Vector3[vertex_count];
		indices = new unsigned long[index_count];

		added_shared = false;

		// Run through the submeshes again, adding the data into the arrays
		for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
		{
			Ogre::SubMesh* submesh = mesh->getSubMesh(i);

			Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

			if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
			{
				if(submesh->useSharedVertices)
				{
					added_shared = true;
					shared_offset = current_offset;
				}

				const Ogre::VertexElement* posElem =
					vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

				Ogre::HardwareVertexBufferSharedPtr vbuf =
					vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

				unsigned char* vertex =
					static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

				// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
				//  as second argument. So make it float, to avoid trouble when Ogre::Real will
				//  be comiled/typedefed as double:
				//      Ogre::Real* pReal;
				float* pReal;

				for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
				{
					posElem->baseVertexPointerToElement(vertex, &pReal);

					Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

					vertices[current_offset + j] = (orient * (pt * scale)) + position;
				}

				vbuf->unlock();
				next_offset += vertex_data->vertexCount;
			}


			Ogre::IndexData* index_data = submesh->indexData;
			size_t numTris = index_data->indexCount / 3;
			Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

			bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

			unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
			unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);


			size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

			if ( use32bitindexes )
			{
				for ( size_t k = 0; k < numTris*3; ++k)
				{
					indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
				}
			}
			else
			{
				for ( size_t k = 0; k < numTris*3; ++k)
				{
					indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
						static_cast<unsigned long>(offset);
				}
			}

			ibuf->unlock();
			current_offset = next_offset;
		}
	}
protected:
	Ogre::Root*			mRoot;
	Ogre::SceneManager*	mManager;
	Ogre::Camera*		mCamera;
};
