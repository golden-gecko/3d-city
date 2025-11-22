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
#include "City.h"
#include "Explorer.h"
#include "Info.h"
#include "Log.h"
#include "Render.h"
#include "Tools.h"

IMPLEMENT_SINGLETON(Tools)

TerrainPage::TerrainPage(wxWindow* parent) : wxPanel(parent)
{
	mRaise = new wxToggleButton(this, TOOL_TERRAIN_RAISE, "Raise");
	mLower = new wxToggleButton(this, TOOL_TERRAIN_LOWER, "Lower");
	mFlatten = new wxToggleButton(this, TOOL_TERRAIN_FLATTEN, "Flatten");
	mLevel = new wxSlider(this, wxID_ANY, 0, -1000, 1000);
	mInnerRadius = new wxSlider(this, wxID_ANY, 50, 0, 100);
	mOuterRadius = new wxSlider(this, wxID_ANY, 70, 0, 100);
	mStrength = new wxSlider(this, wxID_ANY, 1000, 0, 100000);

	mRaise->Connect(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,
		wxCommandEventHandler(TerrainPage::onButtonClick), NULL, this);
	mLower->Connect(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,
		wxCommandEventHandler(TerrainPage::onButtonClick), NULL, this);
	mFlatten->Connect(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,
		wxCommandEventHandler(TerrainPage::onButtonClick), NULL, this);

	mFlexSizer = new wxFlexGridSizer(4, 2, 5, 5);
	mFlexSizer->Add(new wxStaticText(this, wxID_ANY, "Level (-1 km : 1 km)"), 1, wxEXPAND);
	mFlexSizer->Add(mLevel, 1, wxEXPAND);
	mFlexSizer->Add(new wxStaticText(this, wxID_ANY, "Inner radius (0 m : 100 m)"), 1, wxEXPAND);
	mFlexSizer->Add(mInnerRadius, 1, wxEXPAND);
	mFlexSizer->Add(new wxStaticText(this, wxID_ANY, "Outer radius (0 m : 100 m)"), 1, wxEXPAND);
	mFlexSizer->Add(mOuterRadius, 1, wxEXPAND);
	mFlexSizer->Add(new wxStaticText(this, wxID_ANY, "Strength (1 mm : 100 m)"), 1, wxEXPAND);
	mFlexSizer->Add(mStrength, 1, wxEXPAND);
	mFlexSizer->AddGrowableCol(1, 1);

	mBoxSizer = new wxBoxSizer(wxVERTICAL);
	mBoxSizer->Add(mRaise, 0, wxEXPAND | wxALL, 5);
	mBoxSizer->Add(mLower, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);
	mBoxSizer->Add(mFlatten, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);
	mBoxSizer->Add(mFlexSizer, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);

	SetSizer(mBoxSizer);
}

size_t TerrainPage::getActiveTool() const
{
	if (mRaise->GetValue())
	{
		return TOOL_TERRAIN_RAISE;
	}
	else if (mLower->GetValue())
	{
		return TOOL_TERRAIN_LOWER;
	}
	else if (mFlatten->GetValue())
	{
		return TOOL_TERRAIN_FLATTEN;
	}			

	return TOOL_NONE;
}

void TerrainPage::onButtonClick(wxCommandEvent& event)
{
	switch (event.GetId())
	{
		case TOOL_TERRAIN_RAISE:
		{
			if (mRaise->GetValue())
			{
				mLower->SetValue(false);
				mFlatten->SetValue(false);
			}

			break;
		}

		case TOOL_TERRAIN_LOWER:
		{
			if (mLower->GetValue())
			{
				mRaise->SetValue(false);
				mFlatten->SetValue(false);
			}

			break;
		}

		case TOOL_TERRAIN_FLATTEN:
		{
			if (mFlatten->GetValue())
			{
				mRaise->SetValue(false);
				mLower->SetValue(false);
			}

			break;
		}
	}
}

ObjectsPage::ObjectsPage(wxWindow* parent) : wxPanel(parent)
{
	mButtons[0] = new wxToggleButton(this,
		TOOL_OBJECTS_CREATE, "Create building");
	mButtons[1] = new wxToggleButton(this,
		TOOL_OBJECTS_CREATE, "Create plant");
	mButtons[2] = new wxToggleButton(this,
		TOOL_OBJECTS_CREATE, "Create route");
	mButtons[3] = new wxToggleButton(this,
		TOOL_OBJECTS_CREATE, "Create water object");
	mButtons[4] = new wxToggleButton(this,
		TOOL_OBJECTS_SELECT, "Select");
	mButtons[5] = new wxToggleButton(this,
		TOOL_OBJECTS_MOVE, "Move");
	mButtons[6] = new wxToggleButton(this,
		TOOL_OBJECTS_CREATE, "Create junction");
	mPoints = new wxListBox(this, wxID_ANY, wxDefaultPosition,
		wxDefaultSize, 0, 0, wxLB_MULTIPLE);

	for (int i = 0; i < 7; ++i)
	{
		mButtons[i]->Connect(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED,
			wxCommandEventHandler(ObjectsPage::onButtonClick), NULL, this);
	}

	mAdd = new wxButton(this, wxID_ANY, "+", wxPoint(), wxSize(25, 25));
	mDelete = new wxButton(this, wxID_ANY, "-", wxPoint(), wxSize(25, 25));

	mAdd->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(ObjectsPage::onAddClick), NULL, this);
	mDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(ObjectsPage::onDeleteClick), NULL, this);

	mButtonSizer = new wxBoxSizer(wxVERTICAL);
	mButtonSizer->Add(mAdd, 0, wxBOTTOM, 5);
	mButtonSizer->Add(mDelete);

	mPointSizer = new wxBoxSizer(wxHORIZONTAL);
	mPointSizer->Add(mPoints, 1, wxEXPAND | wxRIGHT, 5);
	mPointSizer->Add(mButtonSizer, 0, wxEXPAND);

	mSizer = new wxBoxSizer(wxVERTICAL);
	mSizer->Add(mButtons[0], 0, wxEXPAND | wxALL, 5);
	mSizer->Add(mButtons[1], 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);
	mSizer->Add(mButtons[2], 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);
	mSizer->Add(mButtons[3], 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);
	mSizer->Add(mButtons[6], 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);
	mSizer->Add(mPointSizer, 1, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);
	mSizer->Add(mButtons[4], 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);
	mSizer->Add(mButtons[5], 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);

	SetSizer(mSizer);
}

size_t ObjectsPage::getActiveTool() const
{
	for (int i = 0; i < 7; ++i)
	{
		if (mButtons[i]->GetValue())
		{
			return mButtons[i]->GetId();
		}
	}

	return TOOL_NONE;
}

void ObjectsPage::onButtonClick(wxCommandEvent& event)
{
	wxToggleButton* button = dynamic_cast<wxToggleButton*>(event.GetEventObject());

	if (button && button->GetValue())
	{
		for (int i = 0; i < 7; ++i)
		{
			if (button != mButtons[i])
			{
				mButtons[i]->SetValue(false);
			}
		}
	}
}

void ObjectsPage::onAddClick(wxCommandEvent& event)
{
	// Find object.
	if (ObjectView* view = Info::getSingleton().getSelected().getObjectView())
	{
		if (Shape* object = dynamic_cast<Shape*>(view->getObject()))
		{
			unsigned int n = object->getPointCount();

			// Add new point to object.
			if (n > 1)
			{
				Point3D a = object->getPoint(n - 1);
				Point3D b = object->getPoint(0);

				object->setPoint(n, b + (a - b) * 0.5);
			}

			// Update object views.
			City::getSingleton().updateViews(object);
		}
	}

	Tools::getSingleton().refresh();
}

void ObjectsPage::onDeleteClick(wxCommandEvent& event)
{
	// Find object.
	if (ObjectView* view = Info::getSingleton().getSelected().getObjectView())
	{
		if (Shape* object = dynamic_cast<Shape*>(view->getObject()))
		{
			wxArrayInt selections;
			mPoints->GetSelections(selections);

			for (int i = (int)selections.size() - 1; i >= 0; --i)
			{
				// Delete points from object.
				object->deletePoint(selections[i]);
			}

			// Update object views.
			City::getSingleton().updateViews(object);
		}
	}

	Tools::getSingleton().refresh();
}

CameraPage::CameraPage(wxWindow* parent) : wxPanel(parent)
{
	mPoints = new wxListBox(this, wxID_ANY, wxDefaultPosition,
		wxDefaultSize, 0, 0, wxLB_MULTIPLE);

	mCameraSpeed = new wxTextCtrl(this, wxID_ANY, "15");
	mFreeLook = new wxCheckBox(this, wxID_ANY, "Free look");
	mFreeLook->SetValue(true);

	mSetSpeed = new wxButton(this, wxID_ANY, "Set speed");

	mSpeedSizer = new wxBoxSizer(wxHORIZONTAL);
	mSpeedSizer->Add(mCameraSpeed, 1, wxLEFT, 5);
	mSpeedSizer->Add(mSetSpeed);

	mStart = new wxButton(this, wxID_ANY, "Start");
	mStop = new wxButton(this, wxID_ANY, "Stop");

	mAdd = new wxToggleButton(this, TOOL_CAMERA_ADD_POINT, "+", wxPoint(), wxSize(25, 25));
	mDelete = new wxButton(this, wxID_ANY, "-", wxPoint(), wxSize(25, 25));

	mStart->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(CameraPage::onStartClick), NULL, this);
	mStop->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(CameraPage::onStopClick), NULL, this);
	mAdd->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(CameraPage::onAddClick), NULL, this);
	mDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(CameraPage::onDeleteClick), NULL, this);
	mSetSpeed->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(CameraPage::onSetSpeedClick), NULL, this);

	mButtonSizer = new wxBoxSizer(wxVERTICAL);
	mButtonSizer->Add(mAdd, 0, wxBOTTOM, 5);
	mButtonSizer->Add(mDelete);

	mPointSizer = new wxBoxSizer(wxHORIZONTAL);
	mPointSizer->Add(mPoints, 1, wxEXPAND | wxRIGHT, 5);
	mPointSizer->Add(mButtonSizer, 0, wxEXPAND);

	mSizer = new wxBoxSizer(wxVERTICAL);
	mSizer->Add(mSpeedSizer, 0, wxEXPAND | wxALL, 5);
	mSizer->Add(mFreeLook, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);
	mSizer->Add(mPointSizer, 1, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);
	mSizer->Add(mStart, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);
	mSizer->Add(mStop, 0, wxEXPAND | wxBOTTOM | wxLEFT | wxRIGHT, 5);

	SetSizer(mSizer);
}

void CameraPage::onSetSpeedClick(wxCommandEvent& event)
{
	if (MapView* mapView = Render::getSingleton().getActiveMapView())
	{
		std::string speed = mCameraSpeed->GetValue();
		float realSpeed = Ogre::StringConverter::parseReal(speed);

		if (realSpeed < 1.0f || realSpeed > 100.0f)
		{
			realSpeed = 20.0f;
		}

		mapView->setCameraSpeed(realSpeed);
	}
}

void CameraPage::onStartClick(wxCommandEvent& event)
{
	if (MapView* mapView = Render::getSingleton().getActiveMapView())
	{
		mapView->startShow();
	}
}

void CameraPage::onStopClick(wxCommandEvent& event)
{
	if (MapView* mapView = Render::getSingleton().getActiveMapView())
	{
		mapView->stopShow();
	}
}

void CameraPage::onAddClick(wxCommandEvent& event)
{
	Tools::getSingleton().refresh();
}

void CameraPage::onDeleteClick(wxCommandEvent& event)
{
	Tools::getSingleton().refresh();
}

size_t CameraPage::getActiveTool() const
{
	if (mAdd->GetValue())
	{
		return TOOL_CAMERA_ADD_POINT;
	}

	return TOOL_NONE;
}

Tools::Tools(wxWindow* parent) : wxAuiNotebook(parent)
{
	SetWindowStyle(
		wxAUI_NB_SCROLL_BUTTONS |
		wxAUI_NB_TAB_MOVE |
		wxAUI_NB_TAB_SPLIT |
		wxAUI_NB_TOP |
		wxAUI_NB_WINDOWLIST_BUTTON);

	mObjects = new ObjectsPage(this);
	mCamera = new CameraPage(this);

	AddPage(mObjects, "Objects");
	AddPage(mCamera, "Camera");
}

size_t Tools::getActiveTool() const
{
	if (GetPage(GetSelection()) == mTerrain)
	{
		return mTerrain->getActiveTool();
	}
	else if (GetPage(GetSelection()) == mObjects)
	{
		return mObjects->getActiveTool();
	}
	else if (GetPage(GetSelection()) == mCamera)
	{
		return mCamera->getActiveTool();
	}

	return TOOL_NONE;
}

void Tools::refresh()
{
	// Clear tools.
	mObjects->mPoints->Clear();
	mCamera->mPoints->Clear();

	// Refresh tools.
	if (ObjectView* objectView = Info::getSingleton().getSelected().getObjectView())
	{
		if (Shape* object = dynamic_cast<Shape*>(objectView->getObject()))
		{
			for (unsigned int i = 0; i < object->getPointCount(); ++i)
			{
				Ogre::Vector3 v(
					object->getPoint(i).x,
					object->getPoint(i).y,
					object->getPoint(i).z);

				mObjects->mPoints->Append(
					Convert::ToString(i) + ": " +
					Ogre::StringConverter::toString(v));
				mObjects->mPoints->SetSelection(i);
			}
		}
	}

	if (MapView* mapView = Render::getSingleton().getActiveMapView())
	{
		for (unsigned int i = 0; i < mapView->getPoints().size(); ++i)
		{
			mCamera->mPoints->Append(
				Convert::ToString(i) + ": " +
				Ogre::StringConverter::toString(mapView->getPoints()[i]));
		}
	}
}

void Tools::createObject(float x1, float y1, float x2, float y2)
{
	// Get active map view.
	MapView* mapView = Render::getSingleton().getActiveMapView();

	if (mapView == NULL)
	{
		return;
	}

	Point3D a = mapView->raycastTerrainPoint3D(x1, y1);
	Point3D b = mapView->raycastTerrainPoint3D(x2, y2);

	// Prepare point list.
	PointList points;

	if (mObjects->mButtons[2]->GetValue())
	{
		points.push_back(a);
		points.push_back(b);
	}
	else
	{
		float y = a.y > b.y ? a.y : b.y;

		points.push_back(Point3D(a.x, y, a.z));
		points.push_back(Point3D(a.x, y, b.z));
		points.push_back(Point3D(b.x, y, b.z));
		points.push_back(Point3D(b.x, y, a.z));
	}

	// Create object.
	if (mObjects->mButtons[0]->GetValue())
	{
		Building* building = mapView->getMap()->create<Building>();

		building->setPoints(points);

		mapView->createView(building);
	}
	else if (mObjects->mButtons[1]->GetValue())
	{
		Plant* plant = mapView->getMap()->create<Plant>();

		plant->setPoints(points);

		mapView->createView(plant);
	}
	else if (mObjects->mButtons[2]->GetValue())
	{
		Route* route = mapView->getMap()->create<Route>();

		route->setPoints(points);

		mapView->createView(route);
	}
	else if (mObjects->mButtons[3]->GetValue())
	{
		WaterObject* waterObject = mapView->getMap()->create<WaterObject>();

		waterObject->setPoints(points);

		mapView->createView(waterObject);
	}
	else if (mObjects->mButtons[6]->GetValue())
	{
		float top = 0.0f;
		float bottom = 0.0f;
		float right = 0.0f;
		float left = 0.0f;

		float mLeftButtonDownScreenX = x1;
		float mLeftButtonDownScreenY = y1;

		float mLastMouseScreenX = x2;
		float mLastMouseScreenY = y2;

		if(mLeftButtonDownScreenX < mLastMouseScreenX)
		{
			left = mLeftButtonDownScreenX;
			right = mLastMouseScreenX;
		}
		else
		{
			left = mLastMouseScreenX;
			right = mLeftButtonDownScreenX;
		}
		if(mLeftButtonDownScreenY < mLastMouseScreenY)
		{
			top = mLeftButtonDownScreenY;
			bottom = mLastMouseScreenY;
		}
		else
		{
			top = mLastMouseScreenY;
			bottom = mLeftButtonDownScreenY;
		}

		// cast a ray into the screen for each of the four points
		// Convert screen positions into rays in world space
		Ogre::Camera* mCamera = mapView->getCamera();

		Ogre::Ray topLeft = mCamera->getCameraToViewportRay(left, top);
		Ogre::Ray topRight = mCamera->getCameraToViewportRay(right, top);
		Ogre::Ray bottomLeft = mCamera->getCameraToViewportRay(left, bottom);
		Ogre::Ray bottomRight = mCamera->getCameraToViewportRay(right, bottom);

		Ogre::PlaneBoundedVolume vol;
		vol.planes.push_back(Ogre::Plane(
			topLeft.getOrigin(), topRight.getOrigin(), bottomLeft.getOrigin())); //front plane
		vol.planes.push_back(Ogre::Plane(
			topLeft.getPoint(1000), topLeft.getOrigin(), bottomLeft.getPoint(1000)));   // left plane
		vol.planes.push_back(Ogre::Plane(
			topRight.getPoint(1000), topLeft.getPoint(1000), bottomLeft.getPoint(1000)));   // back plane
		vol.planes.push_back(Ogre::Plane(
			topRight.getOrigin(), topRight.getPoint(1000), bottomRight.getPoint(1000)));   // right plane
		vol.planes.push_back(Ogre::Plane(
			topLeft.getPoint(1000), topRight.getPoint(1000), topLeft.getOrigin()));   // top plane
		vol.planes.push_back(Ogre::Plane(
			bottomLeft.getPoint(1000), bottomLeft.getOrigin(), bottomRight.getPoint(1000)));   // bottom plane

		Ogre::PlaneBoundedVolumeList volumes;
		volumes.push_back(vol); 

		const Ogre::SceneQueryResult& r = mapView->raycast(volumes);
		RouteList routes;

		for (Ogre::SceneQueryResultMovableList::const_iterator i = r.movables.begin(); i != r.movables.end(); i++)
		{
			const Ogre::Any& a = (*i)->getUserAny();

			// TODO: improve it.
			if (a.isEmpty() == false)
			{
				if (a.getType() == typeid(RouteView*))
				{
					if (RouteView* routeView = Ogre::any_cast<RouteView*>(a))
					{
						routes[routeView->getRoute()->getID()] = routeView->getRoute();
					}
				}
			}
		}

		if (routes.size() > 2)
		{
			Junction* junction = mapView->getMap()->create<Junction>();

			junction->setCenter((a + b) * 0.5f);
			junction->setRoutes(routes);

			mapView->createView(junction);
		}
	}

	Explorer::getSingleton().refresh();
}

void Tools::moveObject(const Point3D& v)
{
	if (ObjectView* view = Info::getSingleton().getSelected().getObjectView())
	{
		if (Shape* object = dynamic_cast<Shape*>(view->getObject()))
		{
			wxArrayInt selections;
			mObjects->mPoints->GetSelections(selections);

			for (unsigned int i = 0; i < selections.size(); ++i)
			{
				object->movePoint(selections[i], Point3D(v.x, 0.0f, v.z));
			}

			City::getSingleton().updateViews(object);

			if (Route* route = dynamic_cast<Route*>(object))
			{
				JunctionList::iterator i = route->getJunctions().begin();
				for (; i != route->getJunctions().end(); ++i)
				{
					City::getSingleton().updateViews(*i);
				}
			}
		}
	}
}

void Tools::addCameraPoint(const Ogre::Vector3& v)
{
}

void Tools::setActiveTool(size_t tool)
{
	SetSelection(GetPageIndex(mObjects));

	for (int i = 0; i < 7; ++i)
	{
		mObjects->mButtons[i]->SetValue(false);
	}

	if (tool == TOOL_OBJECTS_SELECT)
	{
		mObjects->mButtons[4]->SetValue(true);
	}
	else if (tool == TOOL_OBJECTS_MOVE)
	{
		mObjects->mButtons[5]->SetValue(true);
	}
}
