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
#include "Log.h"
#include "Main.h"
#include "Menu.h"
#include "Notebook.h"
#include "Render.h"

class Main : public wxFrame, public Ogre::Singleton<Main>
{
public:
	Main();

	virtual ~Main()
	{
		mPaneManager->UnInit();
	}

	virtual bool ShowFullScreen(bool show, long style =
		wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION)
	{
		mToolbar->Show(!(show));

		setLogVisible(!(show));
		setToolsVisible(!(show));

		return wxFrame::ShowFullScreen(show, style);
	}

	bool getLogVisible() const
	{
		return mPaneManager->GetPane(mLog).IsShown();
	}
	bool getToolsVisible() const
	{
		return mPaneManager->GetPane(mNotebook).IsShown();
	}

	void setLogVisible(bool visible)
	{
		if (visible)
		{
			mPaneManager->GetPane(mLog).Show();
		}
		else
		{
			mPaneManager->GetPane(mLog).Hide();
		}

		mPaneManager->Update();
	}
	void setToolsVisible(bool visible)
	{
		if (visible)
		{
			mPaneManager->GetPane(mNotebook).Show();
		}
		else
		{
			mPaneManager->GetPane(mNotebook).Hide();
		}

		mPaneManager->Update();
	}
protected:
	Menu*			mMenu;
	wxToolBar*		mToolbar;
	Render*			mRender;
	Notebook*		mNotebook;
	Log*			mLog;

	wxBoxSizer*		mSizer;
	wxAuiManager*	mPaneManager;

	void onButtonClick(wxCommandEvent& event);
	void onClose(wxCloseEvent& event);
};
