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

class CreateMapDialog : public wxDialog, public Ogre::Singleton<CreateMapDialog>
{
public:
	CreateMapDialog(wxWindow* parent);

	virtual ~CreateMapDialog()
	{
		// Is this correct?
		delete mFileDialog;
	}

	/// Loads database list and displays dialog.
	virtual int ShowModal();
protected:
	wxChoice*			mDatabases;
	wxTextCtrl*			mName;
	wxTextCtrl*			mDescription;

	wxTextCtrl*			mLatitude;
	wxTextCtrl*			mLongitude;
	wxChoice*			mZoom;

	wxTextCtrl*			mFileName;
	wxButton*			mBrowse;
	wxFileDialog*		mFileDialog;

	wxTextCtrl*			mMapSize;
	wxCheckBox*			mImageOrRaw;
	wxCheckBox*			mIntOrFloat;
	wxTextCtrl*			mTerrainSize;
	wxTextCtrl*			mBytesPerValue;

	wxButton*			mOk;
	wxButton*			mCancel;

	wxBoxSizer*			mSizer[2];
	wxFlexGridSizer*	mTextBoxes;
	wxSizer*			mCenter;
	wxSizer*			mTerrainData;
	wxSizer*			mButtons;
	wxSizer*			mMain;

	void onBrowse(wxCommandEvent& event);
	void onOk(wxCommandEvent& event);
	void onCancel(wxCommandEvent& event);
};
