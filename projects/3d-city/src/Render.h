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
#include "Debug.h"

class Timer : public wxTimer
{
protected:
	virtual void Notify();
};

class Render :
	public wxPanel,
	public Ogre::FrameListener,
	public Ogre::Singleton<Render>
{
public:
	Render(wxWindow* parent);

	void init()
	{
	}
	void shutdown()
	{
		mActive = false;
	}

	void start()
	{
		mTimer.Start(20);
	}
	void stop()
	{
		mTimer.Stop();
	}

	MapView* getActiveMapView() const
	{
		return mMapView;
	}

	void select(MapView* mapView);
	void unselect();

	void onEnterWindow(wxMouseEvent& event);
	void onLeftDown(wxMouseEvent& event);
	void onLeftUp(wxMouseEvent& event);
	void onMotion(wxMouseEvent& event);
	void onRightDown(wxMouseEvent& event);
	void onRightUp(wxMouseEvent& event);
	void onWheel(wxMouseEvent& event);
	void onSize(wxSizeEvent& event);
protected:
	Timer				mTimer;

	Ogre::RenderWindow*	mWindow;
	Ogre::Viewport*		mViewport;

	MapView*			mMapView;

	wxPoint				mLeft;
	wxPoint				mMotion;
	wxPoint				mRight;

	bool				mActive;

	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual bool frameEnded(const Ogre::FrameEvent& evt);
};
