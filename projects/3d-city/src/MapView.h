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
#include "Convert.h"
#include "Graphics.h"
#include "Map.h"

typedef std::multimap<Object*, ObjectView*> ObjectViewList;

typedef std::vector<float> VertexVector;
typedef std::vector<unsigned short> IndexVector;

typedef std::vector<Ogre::Mesh*> MeshVector;
typedef std::vector<Ogre::Entity*> EntityVector;
typedef std::vector<Ogre::SceneNode*> SceneNodeVector;

//------------------------------------------------------------------------------

class MapView : public View
{
public:
	MapView(unsigned int id, Map* map, unsigned int objects = OT_NONE,
		bool autoLoad = true) : View(id), mMap(map), mObjectsLoaded(objects),
		mIsOrthographic(false), mIsShowRunning(false)
	{
		if (autoLoad)
		{
			load();
		}
	}

	virtual ~MapView()
	{
		unload();
	}

	virtual bool load();
	virtual void unload();
	virtual bool update();

	ObjectView* createView(Object* object);
	ObjectView* getObjectView(unsigned int objectViewID) const;
	void updateViews(Object* object);
	void destroyViews(Object* object);
	void destroyView(ObjectView* view);

	void lookAt(ObjectView* view)
	{
		lookAt(view->getObject()->getPosition());
	}
	void lookAt(const Point& point)
	{
		lookAt(getPoint3D(point));
	}
	void lookAt(const Point3D& point3D)
	{
		Ogre::Vector3 v(point3D.x, point3D.y, point3D.z);

		mCamera->setPosition(v + Ogre::Vector3::UNIT_SCALE * 100.0f);
		mCamera->lookAt(v);
	}

	float getScale() const
	{
		return mManager->getRootSceneNode()->getScale().x;
	}

	void setScale(float s) const
	{
		mManager->getRootSceneNode()->setScale(Ogre::Vector3(s));
	}

	Ogre::Entity* createEntity(const std::string& name, const std::string& mesh) const
	{
		return mManager->createEntity(name, mesh);
	}
	Ogre::SceneNode* createSceneNode() const
	{
		return mManager->getRootSceneNode()->createChildSceneNode();
	}
	Ogre::StaticGeometry* createStaticGeometry(const std::string& name) const
	{
		return mManager->createStaticGeometry(name);
	}

	void destroyEntity(Ogre::Entity* entity) const
	{
		mManager->destroyEntity(entity);
	}
	void destroySceneNode(Ogre::SceneNode* node) const
	{
		mManager->destroySceneNode(node);
	}
	void destroyStaticGeometry(Ogre::StaticGeometry* geometry) const
	{
		mManager->destroyStaticGeometry(geometry);
	}

	ObjectView* raycastObjectView(float x, float y);

	Point3D raycastPoint3D(float x, float y)
	{
		return raycastPoint3D(mCamera->getCameraToViewportRay(x, y));
	}

	float raycastTerrainHeight(float x, float z)
	{
		return 0.0f;
	}

	Point3D raycastPoint3D(const Point3D& p)
	{
		return raycastPoint3D(Ogre::Ray(Ogre::Vector3(
			p.x, p.y + 10000.0f, p.z), Ogre::Vector3::NEGATIVE_UNIT_Y));
	}

	Ogre::Vector3 raycastPoint3D(const Ogre::Vector3& v)
	{
		Point3D p = raycastPoint3D(Ogre::Ray(Ogre::Vector3(
			v.x, v.y + 10000.0f, v.z), Ogre::Vector3::NEGATIVE_UNIT_Y));

		return Ogre::Vector3(p.x, p.y, p.z);
	}

	Point3D raycastPoint3D(const Ogre::Ray& ray)
	{
		Ogre::Vector3 result;

		// create a query object
		mRayQuery->setRay(ray);

		// execute the query, returns a vector of hits
		if (mRayQuery->execute().size() <= 0)
		{
			// raycast did not hit an objects bounding box
			return Point3D(ray.getOrigin().x, 0.0f, ray.getOrigin().z);
		}

		// mesh data to retrieve         
		size_t vertex_count;
		size_t index_count;
		Ogre::Vector3 *vertices;
		unsigned long *indices;

		// at this point we have raycast to a series of different objects bounding boxes.
		// we need to test these different objects to see which is the first polygon hit.
		// there are some minor optimizations (distance based) that mean we wont have to
		// check all of the objects most of the time, but the worst case scenario is that
		// we need to test every triangle of every object.
		Ogre::Real closest_distance = -1.0f;
		Ogre::Vector3 closest_result;
		Ogre::RaySceneQueryResult &query_result = mRayQuery->getLastResults();
		for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
		{
			// stop checking if we have found a raycast hit that is closer
			// than all remaining entities
			if ((closest_distance >= 0.0f) &&
				(closest_distance < query_result[qr_idx].distance))
			{
				 break;
			}

			// only check this result if its a hit against an entity
			if ((query_result[qr_idx].movable != NULL) &&
				(query_result[qr_idx].movable->getMovableType().compare("Entity") == 0))
			{
				// get the entity to check
				Ogre::Entity *pentity = static_cast<Ogre::Entity*>(query_result[qr_idx].movable); 

				if (pentity->getUserAny().getType() != typeid(MapView*))
				{
					continue;
				}

				// get the mesh information
			Graphics::getMeshInformation(pentity->getMesh(), vertex_count, vertices, index_count, indices,             
			//	pentity->getParentNode()->getWorldPosition(),
			//	pentity->getParentNode()->getWorldOrientation(),
			//	pentity->getParentNode()->_getDerivedScale());
				pentity->getParentNode()->getPosition(),
				pentity->getParentNode()->getOrientation(),
				pentity->getParentNode()->_getDerivedScale());
		
				// test for hitting individual triangles on the mesh
				bool new_closest_found = false;
				for (int i = 0; i < static_cast<int>(index_count); i += 3)
				{
					// check for a hit against this triangle
					std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
						vertices[indices[i+1]], vertices[indices[i+2]], true, false);

					// if it was a hit check if its the closest
					if (hit.first)
					{
						if ((closest_distance < 0.0f) ||
							(hit.second < closest_distance))
						{
							// this is the closest so far, save it off
							closest_distance = hit.second;
							new_closest_found = true;
						}
					}
				}

			 // free the verticies and indicies memory
				delete[] vertices;
				delete[] indices;

				// if we found a new closest raycast for this object, update the
				// closest_result before moving on to the next object.
				if (new_closest_found)
				{
					closest_result = ray.getPoint(closest_distance);               
				}
			}       
		}

		// return the result
		if (closest_distance >= 0.0f)
		{
			// raycast success
			result = closest_result;
			return Point3D(result.x, result.y, result.z);
		}

		return Point3D(ray.getOrigin().x, 0.0f, ray.getOrigin().z);
	}

	Point getPoint(const Point3D& point3D) const
	{
		double centerLat = mMap->getCenter().latitude;
		double centerLon = mMap->getCenter().longitude;
		double zoom = mMap->getZoom();

		double scale = 1.0 / pow(2.0, zoom - 1.0);

		double bottom = centerLat - 90.0 * scale;
		double top = centerLat + 90.0  * scale;
		double left = centerLon - 180.0 * scale;
		double right = centerLon + 180.0 * scale;

		double latitudeLength = mMap->getSize().z * mManager->getRootSceneNode()->getScale().z;
		double longitudeLength = mMap->getSize().x * mManager->getRootSceneNode()->getScale().x;

		double latitude = bottom + (latitudeLength - point3D.z) * (top - bottom) / latitudeLength;
		double longitude = left + point3D.x * (right - left) / longitudeLength;

		return Point(latitude, longitude, point3D.y);
	}
	Point3D getPoint3D(const Point& point) const
	{
		double centerLat = mMap->getCenter().latitude;
		double centerLon = mMap->getCenter().longitude;
		double zoom = mMap->getZoom();

		double scale = 1.0 / pow(2.0, zoom - 1.0);

		double bottom = centerLat - 90.0 * scale;
		double top = centerLat + 90.0  * scale;
		double left = centerLon - 180.0 * scale;
		double right = centerLon + 180.0 * scale;

		double latitudeLength = mMap->getSize().z * mManager->getRootSceneNode()->getScale().z;
		double longitudeLength = mMap->getSize().x * mManager->getRootSceneNode()->getScale().x;

		double z = latitudeLength * (1.0f - (point.latitude - bottom) / (top - bottom));
		double x = longitudeLength * (point.longitude - left) / (right - left);

		return Point3D(x, point.height, z);
	}

	Map* getMap() const
	{
		return mMap;
	}
	ObjectViewList& getObjectViews()
	{
		return mObjectViews;
	}
	Ogre::SceneManager* getSceneManager() const
	{
		return mManager;
	}
	Ogre::Light* getLight() const
	{
		return mLight;
	}
	Ogre::Camera* getCamera() const
	{
		return mCamera;
	}
	Ogre::RaySceneQuery* getRayQuery() const
	{
		return mRayQuery;
	}

	Point3D raycastTerrainPoint3D(float x, float y) const
	{
		return raycastTerrainPoint3D(mCamera->getCameraToViewportRay(x, y));
	}
	Ogre::Vector3 raycastTerrainPoint3D(const Ogre::Vector3& v) const
	{
		const Point3D& point = raycastTerrainPoint3D(Ogre::Ray(
			v + Ogre::Vector3::UNIT_Y * 1000.0f,
			Ogre::Vector3::NEGATIVE_UNIT_Y));

		return Ogre::Vector3(point.x, point.y, point.z);
	}
	Ogre::Vector3 raycastTerrainPoint3D(const Point3D& p) const
	{
		const Ogre::Vector3 v(p.x, p.y, p.z);

		const Point3D& point = raycastTerrainPoint3D(Ogre::Ray(
			v + Ogre::Vector3::UNIT_Y * 1000.0f,
			Ogre::Vector3::NEGATIVE_UNIT_Y));

		return Ogre::Vector3(point.x, point.y, point.z);
	}

	Point3D raycastTerrainPoint3D(const Ogre::Ray& ray) const;

	const Ogre::SceneQueryResult& raycast(
		const Ogre::PlaneBoundedVolumeList& volumnes) const;

	bool getIsOrthographic() const
	{
		return mIsOrthographic;
	}
	void setIsOrthographic(bool orthographic)
	{
		mIsOrthographic = orthographic;

		if (orthographic)
		{
			// TODO: Implement orthographic view.
			mCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
			mCamera->setPosition(0, 200, 0 + 0.001);
			mCamera->lookAt(0, 200, 0);
			mCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
			mCamera->setAspectRatio(1.0f);
		}
		else
		{
			mCamera->setProjectionType(Ogre::PT_PERSPECTIVE);
			mCamera->setDirection(0.0f, -1.0f, -1.0f);
			mCamera->setPosition(500.0f, 200.0f, 500.0f);
			mCamera->setNearClipDistance(2.0f);
			mCamera->setFarClipDistance(1024.0f);
			mCamera->setFOVy(Ogre::Degree(60.0f));
		}
	}

	template <class T>
	T* getView(Object* object) const
	{
		ObjectViewList::const_iterator i = mObjectViews.find(object);

		if (i != mObjectViews.end())
		{
			return dynamic_cast<T*>(i->second);
		}

		return NULL;
	}
protected:
	ObjectViewList			mObjectViews;
	Map*					mMap;

	Ogre::SceneManager*		mManager;
	Ogre::Light*			mLight;
	Ogre::Camera*			mCamera;

	Ogre::RaySceneQuery*					mRayQuery;
	Ogre::PlaneBoundedVolumeListSceneQuery*	mVolumeQuery;

	template <class ViewType, class ObjectType>
	ViewType* createView(ObjectType* object);
protected:
	unsigned int		mObjectsLoaded;
	Ogre::MaterialPtr	mMaterial;
	bool				mIsOrthographic;

	unsigned int	mTileX;
	unsigned int	mTileZ;

	MeshVector		mMeshes;
	EntityVector	mEntities;
	SceneNodeVector	mSceneNodes;

	void createTerrain();
	void createTerrainFromFile();
	void destroyTerrain();

	void createObjects();
	void destroyObjects();
public:
		void addCameraPoint(const Ogre::Vector3& v)
		{
			mPointList.push_back(v);
		}
		void deleteCameraPoint(unsigned int n)
		{
			unsigned int counter = 0;

			std::vector<Ogre::Vector3>::iterator i = mPointList.begin();
			for (; i != mPointList.end(); ++i)
			{
				if (n == counter)
				{
					mPointList.erase(i);

					break;
				}

				++counter;
			}
		}

		void startShow();
		void stopShow()
		{
			mIsShowRunning = false;
		}

		void setCameraSpeed(float speed)
		{
			mCameraSpeed = speed;
		}

		void update(float time);

		const std::vector<Ogre::Vector3>& getPoints() const
		{
			return mPointList;
		}
protected:
	bool mIsShowRunning;
	std::vector<Ogre::Vector3> mPointList;
	Ogre::SimpleSpline mSpline;

	float mCameraSpeed;
	unsigned int mCounter;

	Ogre::Vector3 mCurrent, mNext, mDir;
};
