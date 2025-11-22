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

class OpenMapDialog : public wxDialog, public Ogre::Singleton<OpenMapDialog>
{
public:
	OpenMapDialog(wxWindow* parent);

	virtual int ShowModal();
protected:
	wxTreeCtrl*			mMaps;

	wxTextCtrl*			mSizeLatitude;
	wxTextCtrl*			mSizeLongitude;
	
	wxCheckBox*			mBuildings;
	wxCheckBox*			mRoutes;
	wxCheckBox*			mPlants;
	wxCheckBox*			mWaterObjects;

	wxChoice*			mBuildingTypes;
	wxTextCtrl*			mBuildingColour;
	wxChoice*			mRouteTypes;
	wxTextCtrl*			mRouteColour;

	wxButton*			mOk;
	wxButton*			mCancel;

	wxFlexGridSizer*	mGrid;
	wxBoxSizer*			mButtons;
	wxBoxSizer*			mMain;
	wxBoxSizer*			mSizer;

	void onMapSelect(wxTreeEvent& event);
	void onOk(wxCommandEvent& event);
	void onCancel(wxCommandEvent& event);
};
