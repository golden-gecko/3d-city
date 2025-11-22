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
#include "Debug.h"
#include "Info.h"
#include "Log.h"
#include "MapView.h"
#include "Notebook.h"
#include "Properties.h"
#include "Render.h"
#include "RouteSearchDialog.h"

IMPLEMENT_SINGLETON(Render)

void Timer::Notify()
{
	Ogre::Root::getSingleton().renderOneFrame();
}

Render::Render(wxWindow* parent) : wxPanel(parent,
	wxID_ANY, wxPoint(), wxSize(100, 100)),
	mMapView(NULL), mWindow(NULL), mViewport(NULL), mActive(true)
{
	// Create Ogre render window.
	Ogre::NameValuePairList params;

	params["externalWindowHandle"] =
		Ogre::StringConverter::toString((size_t)GetHandle());

	Ogre::ConfigFile config;
	config.load("Ogre.cfg");

	if (config.getSetting("Render System") == "Direct3D9 Rendering Subsystem")
	{
		std::string aa = config.getSetting("Anti aliasing", "Direct3D9 Rendering Subsystem");
		std::string vs = config.getSetting("VSync", "Direct3D9 Rendering Subsystem");

		if (aa == "None") params["FSAA"] = "0";
		else if (aa == "Level 2") params["FSAA"] = "2";
		else if (aa == "Level 4") params["FSAA"] = "4";

		if (vs == "Yes") params["vsync"] = "1";
		else params["vsync"] = "0";
	}
	else if (config.getSetting("Render System") == "OpenGL Rendering Subsystem")
	{
		params["FSAA"] = config.getSetting("FSAA", "OpenGL Rendering Subsystem", "0");
		std::string vs = config.getSetting("VSync", "OpenGL Rendering Subsystem");

		if (vs == "Yes") params["vsync"] = "1";
		else params["vsync"] = "0";
	}

	// Create render window (Ogre).
	mWindow = Ogre::Root::getSingleton().createRenderWindow("3d-city", 1600, 1200, false, &params);
	mWindow->setActive(false);

	Ogre::Root::getSingleton().addFrameListener(this);

	// Connect events to wxWidgets window.
	Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(Render::onEnterWindow), NULL, this);
	Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(Render::onLeftDown), NULL, this);
	Connect(wxEVT_LEFT_UP, wxMouseEventHandler(Render::onLeftUp), NULL, this);
	Connect(wxEVT_MOTION, wxMouseEventHandler(Render::onMotion), NULL, this);
	Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(Render::onRightDown), NULL, this);
	Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(Render::onRightUp), NULL, this);
	Connect(wxEVT_SIZE, wxSizeEventHandler(Render::onSize), NULL, this);
	Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(Render::onWheel), NULL, this);

	// Set background colour to black.
	SetBackgroundColour(wxColour(0, 0, 0));

	// Start rendering.
	start();
}

void Render::select(MapView* mapView)
{
	if (mapView)
	{
		if (mapView->isLoaded() == false)
		{
			Log::getSingleton().error("Map is not loaded. Attempting to load...");

			if (mapView->load() == false)
			{
				Log::getSingleton().error("Erroring loading map.");

				return;
			}
			else
			{
				Log::getSingleton().info("Map \"" + mapView->getMap()->getName() + "\" loaded.");
			}
		}

		mMapView = mapView;

		if (mViewport == NULL)
		{
			mViewport = mWindow->addViewport(mMapView->getCamera());
		}
		else
		{
			mViewport->setCamera(mMapView->getCamera());
		}

		mViewport->setBackgroundColour(mMapView->getSceneManager()->getFogColour());
		mWindow->setActive(true);
	}

	//}
	//else if (ObjectView* objectView = dynamic_cast<ObjectView*>(view))
	//{
	//	if (objectView->getMapView()->isLoaded() == false)
	//	{
	//		Log::getSingleton().error("Map is not loaded. Attempting to load...");

	//		if (objectView->getMapView()->load() == false)
	//		{
	//			Log::getSingleton().error("Erroring loading map.");

	//			return;
	//		}
	//		else
	//		{
	//			Log::getSingleton().info("Map \"" + mapView->getMap()->getName() + "\" loaded.");
	//		}
	//	}

	//	mMapView = objectView->getMapView();

	//	if (mViewport == NULL)
	//	{
	//		mViewport = mWindow->addViewport(mMapView->getCamera());
	//	}
	//	else
	//	{
	//		mViewport->setCamera(mMapView->getCamera());
	//	}

	//	mViewport->setBackgroundColour(mMapView->getSceneManager()->getFogColour());
	//	mWindow->setActive(true);

	//	if (objectView->isLoaded() == false)
	//	{
	//		Log::getSingleton().error("Object is not loaded. Attempting to load...");

	//		if (objectView->load() == false)
	//		{
	//			Log::getSingleton().error("Erroring loading object.");

	//			return;
	//		}
	//		else
	//		{
	//			Log::getSingleton().info("Object \"" + objectView->getObject()->getName() + "\" loaded.");
	//		}
	//	}

	//	mMapView->lookAt(objectView->getObject()->getPosition());
	//}
}

void Render::unselect()
{
	// Clear active map view.
	mMapView = NULL;

	// Deactivate Ogre window.
	mWindow->setActive(false);

	// Refresh wxWindow to make it black.
	Refresh();
}

void Render::onSize(wxSizeEvent& event)
{
	mWindow->windowMovedOrResized();
}

void Render::onMotion(wxMouseEvent& event)
{
	if (mMapView && mMapView->isLoaded())
	{
		float x = (float)event.GetX() / (float)GetRect().GetWidth();
		float y = (float)event.GetY() / (float)GetRect().GetHeight();

		const Point3D& point3D = mMapView->raycastTerrainPoint3D(x, y);
		const Point& point = mMapView->getPoint(point3D);

		Info::getSingleton().setPoint3D(point3D);
		Info::getSingleton().setPoint(point);

		//

		if (event.RightIsDown())
		{
			float fov = mMapView->getCamera()->getFOVy().valueRadians() * 0.2f;

			mMapView->getCamera()->yaw(
				Ogre::Degree((mMotion.x - event.GetX()) * fov));
			mMapView->getCamera()->pitch(
				Ogre::Degree((mMotion.y - event.GetY()) * fov));
		}
		else if (event.MiddleIsDown())
		{
			mMapView->getCamera()->moveRelative(
				Ogre::Vector3::UNIT_X * (event.GetX() - mMotion.x));
			mMapView->getCamera()->moveRelative(
				Ogre::Vector3::UNIT_Z * (event.GetY() - mMotion.y));

			Ogre::Vector3 pos = mMapView->getCamera()->getPosition();

			float xLimit = mMapView->getMap()->getSize().x * mMapView->getScale() - 1.0f;
			float yLimit = mMapView->raycastTerrainPoint3D(pos).y + 10.0f;
			float zLimit = mMapView->getMap()->getSize().z * mMapView->getScale() - 1.0f;

			if (pos.x < 1.0f) pos.x = 1.0f;
			else if (pos.x > xLimit) pos.x = xLimit;

		//	if (pos.y < yLimit) pos.y = yLimit;
		//	else if (pos.y > 1000.0f) pos.y = 1000.0f;

			if (pos.z < 1.0f) pos.z = 1.0f;
			else if (pos.z > zLimit) pos.z = zLimit;

			mMapView->getCamera()->setPosition(pos);
		}

		//

		if (event.LeftIsDown() && Notebook::getSingleton().getActivePage() == PAGE_TOOLS)
		{
			switch (Tools::getSingleton().getActiveTool())
			{
				case TOOL_OBJECTS_MOVE:
				{
					float x1 = (float)mMotion.x / (float)GetRect().GetWidth();
					float y1 = (float)mMotion.y / (float)GetRect().GetHeight();

					float x2 = (float)event.GetX() / (float)GetRect().GetWidth();
					float y2 = (float)event.GetY() / (float)GetRect().GetHeight();

					Point3D vector =
						mMapView->raycastTerrainPoint3D(x2, y2) -
						mMapView->raycastTerrainPoint3D(x1, y1);

					Tools::getSingleton().moveObject(vector);

					break;
				}
			}
		}
	}

	mMotion = event.GetPosition();
}

void Render::onWheel(wxMouseEvent& event)
{
	if (mMapView && mMapView->isLoaded())
	{
		float fov = mMapView->getCamera()->getFOVy().valueDegrees();

		if (event.GetWheelRotation() < 0)
		{
			fov *= 1.1f;
		}
		else if (event.GetWheelRotation() > 0)
		{
			fov *= 0.9f;
		}

		if (fov > 60.0f)
		{
			fov = 60.0f;
		}
		else if (fov < 1.0f)
		{
			fov = 1.0f;
		}

		mMapView->getCamera()->setFOVy(Ogre::Degree(fov));
	}
}

void Render::onLeftDown(wxMouseEvent& event)
{
	SetFocus();

	if (mMapView && mMapView->isLoaded())
	{
		RouteSearchDialog& dialog = RouteSearchDialog::getSingleton();

		if (dialog.IsShown() && dialog.isButtonPressed())
		{
			float x = (float)event.GetX() / (float)GetRect().GetWidth();
			float y = (float)event.GetY() / (float)GetRect().GetHeight();

			dialog.insertPoint(mMapView->raycastTerrainPoint3D(x, y));
		}
		else if (Notebook::getSingleton().getActivePage() == PAGE_TOOLS)
		{
			switch (Tools::getSingleton().getActiveTool())
			{
				case TOOL_OBJECTS_SELECT:
				{
					float x = (float)event.GetX() / (float)GetRect().GetWidth();
					float y = (float)event.GetY() / (float)GetRect().GetHeight();

					if (ObjectView* object = mMapView->raycastObjectView(x, y))
					{
						Info::getSingleton().setSelected(Identity(object));

						Properties::getSingleton().refresh();
						Tools::getSingleton().refresh();
					}

					break;
				}

				case TOOL_OBJECTS_ADD_POINT:
				{
					if (ObjectView* object = Info::getSingleton().getSelected().getObjectView())
					{
						if (Shape* shape = dynamic_cast<Shape*>(object))
						{
							float x = (float)event.GetX() / (float)GetRect().GetWidth();
							float y = (float)event.GetY() / (float)GetRect().GetHeight();

							shape->insertPoint(mMapView->raycastTerrainPoint3D(x, y));

							mMapView->updateViews(shape);

							Properties::getSingleton().refresh();
							Tools::getSingleton().refresh();
						}
					}

					break;
				}

				case TOOL_CAMERA_ADD_POINT:
				{
					float x = (float)event.GetX() / (float)GetRect().GetWidth();
					float y = (float)event.GetY() / (float)GetRect().GetHeight();

					Point3D& p = mMapView->raycastTerrainPoint3D(x, y);
					mMapView->addCameraPoint(Ogre::Vector3(p.x, p.y, p.z));
					Tools::getSingleton().refresh();

					break;
				}
			}
		}
	}

	mLeft = event.GetPosition();
}

void Render::onLeftUp(wxMouseEvent& event)
{
	if (mMapView && mMapView->isLoaded())
	{
		if (Notebook::getSingleton().getActivePage() == PAGE_TOOLS)
		{
			switch (Tools::getSingleton().getActiveTool())
			{
				case TOOL_OBJECTS_CREATE:
				{
					float x1 = (float)mLeft.x / (float)GetRect().GetWidth();
					float y1 = (float)mLeft.y / (float)GetRect().GetHeight();
					
					float x2 = (float)event.GetX() / (float)GetRect().GetWidth();
					float y2 = (float)event.GetY() / (float)GetRect().GetHeight();

					Tools::getSingleton().createObject(x1, y1, x2, y2);

					break;
				}
			}
		}
	}
}

void Render::onRightDown(wxMouseEvent& event)
{
	SetFocus();

	mRight = event.GetPosition();
}

void Render::onRightUp(wxMouseEvent& event)
{
}

void Render::onEnterWindow(wxMouseEvent& event)
{
	SetFocus();
}

bool Render::frameStarted(const Ogre::FrameEvent& evt)
{
	if (mMapView)
	{
		mMapView->update(evt.timeSinceLastFrame);
	}

	Info::getSingleton().setFPS(mWindow->getAverageFPS());

	return true;
}

bool Render::frameEnded(const Ogre::FrameEvent& evt)
{
	return mActive;
}
