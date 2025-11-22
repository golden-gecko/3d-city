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

class Tools;

enum
{
	TOOL_NONE = 200,

	TOOL_TERRAIN_RAISE,
	TOOL_TERRAIN_LOWER,
	TOOL_TERRAIN_FLATTEN,

	TOOL_OBJECTS_CREATE,
	TOOL_OBJECTS_ADD_POINT,
	TOOL_OBJECTS_SELECT,
	TOOL_OBJECTS_MOVE,

	TOOL_CAMERA_ADD_POINT
};

class TerrainPage : public wxPanel
{
	friend Tools;
public:
	TerrainPage(wxWindow* parent);

	size_t getActiveTool() const;
protected:
	wxToggleButton*		mRaise;
	wxToggleButton*		mLower;
	wxToggleButton*		mFlatten;

	wxSlider*			mLevel;
	wxSlider*			mInnerRadius;
	wxSlider*			mOuterRadius;
	wxSlider*			mStrength;

	wxBoxSizer*			mBoxSizer;
	wxFlexGridSizer*	mFlexSizer;

	void onButtonClick(wxCommandEvent& event);
};

class ObjectsPage : public wxPanel
{
	friend Tools;
public:
	ObjectsPage(wxWindow* parent);

	size_t getActiveTool() const;
protected:
	wxToggleButton*	mButtons[7];
	wxButton*		mAdd;
	wxButton*		mDelete;
	wxListBox*		mPoints;

	wxBoxSizer*		mButtonSizer;
	wxBoxSizer*		mPointSizer;
	wxBoxSizer*		mSizer;

	void onButtonClick(wxCommandEvent& event);
	void onAddClick(wxCommandEvent& event);
	void onDeleteClick(wxCommandEvent& event);
};

class CameraPage : public wxPanel
{
	friend Tools;
public:
	CameraPage(wxWindow* parent);

	size_t getActiveTool() const;
protected:
	wxTextCtrl*		mCameraSpeed;
	wxButton*		mSetSpeed;
	wxCheckBox*		mFreeLook;

	wxButton*		mStart;
	wxButton*		mStop;

	wxToggleButton*	mAdd;
	wxButton*		mDelete;
	wxListBox*		mPoints;

	wxBoxSizer*		mSpeedSizer;
	wxBoxSizer*		mButtonSizer;
	wxBoxSizer*		mPointSizer;
	wxBoxSizer*		mSizer;

	void onStartClick(wxCommandEvent& event);
	void onStopClick(wxCommandEvent& event);

	void onAddClick(wxCommandEvent& event);
	void onDeleteClick(wxCommandEvent& event);

	void onSetSpeedClick(wxCommandEvent& event);
};

class Tools : public wxAuiNotebook, public Ogre::Singleton<Tools>
{
public:
	Tools(wxWindow* parent);
	
	size_t getActiveTool() const;

	void setActiveTool(size_t tool);

	void refresh();

	double getTerrainBrushLevel() const
	{
		return (double)mTerrain->mLevel->GetValue();
	}
	double getTerrainBrushInnerRadius() const
	{
		return (double)mTerrain->mInnerRadius->GetValue();
	}
	double getTerrainBrushOuterRadius() const
	{
		return (double)mTerrain->mOuterRadius->GetValue();
	}
	double getTerrainBrushStrength() const
	{
		return (double)mTerrain->mStrength->GetValue() * 0.001f;
	}

	void createObject(float x1, float y1, float x2, float y2);
	void moveObject(const Point3D& v);

	void addCameraPoint(const Ogre::Vector3& v);
protected:
	TerrainPage*	mTerrain;
	ObjectsPage*	mObjects;
	CameraPage*		mCamera;
};
