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
#include "ConnectDialog.h"
#include "Main.h"
#include "CreateMapDialog.h"
#include "OpenMapDialog.h"
#include "SearchDialog.h"
#include "RouteSearchDialog.h"

IMPLEMENT_SINGLETON(Main)

Main::Main() : wxFrame(NULL, wxID_ANY, "3d-city v1.0 alpha",
	wxPoint(0, 0), wxSize(1250, 950))
{
	// Create toolbar.
	mToolbar = new wxToolBar(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxTB_HORIZONTAL);

	unsigned int ids[] =
	{
		DATABASE_CONNECT,
		DATABASE_DISCONNECT,

		MAP_OPEN,
		MAP_SAVE,
		MAP_CLOSE,

		TOOL_NONE,
		TOOL_OBJECTS_SELECT,
		TOOL_OBJECTS_MOVE,

		VIEW_FULLSCREEN,

		HELP_DOCUMENTATION
	};

	wxBitmap bitmaps[] =
	{
		wxBitmap("..\\gui\\tango-icon-theme-0.8.1\\32x32\\network-idle.png", wxBITMAP_TYPE_PNG),
		wxBitmap("..\\gui\\tango-icon-theme-0.8.1\\32x32\\network-offline.png", wxBITMAP_TYPE_PNG),

		wxBitmap("..\\gui\\tango-icon-theme-0.8.1\\32x32\\document-open.png", wxBITMAP_TYPE_PNG),
		wxBitmap("..\\gui\\tango-icon-theme-0.8.1\\32x32\\document-save.png", wxBITMAP_TYPE_PNG),
		wxBitmap("..\\gui\\tango-icon-theme-0.8.1\\32x32\\emblem-unreadable.png", wxBITMAP_TYPE_PNG),

		wxBitmap("..\\gui\\tango-icon-theme-0.8.1\\32x32\\go-jump.png", wxBITMAP_TYPE_PNG),
		wxBitmap("..\\gui\\tango-icon-theme-0.8.1\\32x32\\user-desktop.png", wxBITMAP_TYPE_PNG),
		wxBitmap("..\\gui\\tango-icon-theme-0.8.1\\32x32\\go-last.png", wxBITMAP_TYPE_PNG),

		wxBitmap("..\\gui\\tango-icon-theme-0.8.1\\32x32\\view-fullscreen.png", wxBITMAP_TYPE_PNG),

		wxBitmap("..\\gui\\tango-icon-theme-0.8.1\\32x32\\help-browser.png", wxBITMAP_TYPE_PNG)
	};

	std::string captions[] =
	{
		"Connect",
		"Disconnect",

		"Open",
		"Save",
		"Close",

		"Pointer",
		"Select",
		"Move",

		"Fullscreen",

		"Help"
	};

	for (int i = 0; i < 2; ++i)
	{
		mToolbar->AddTool(ids[i], bitmaps[i], captions[i]);
	}

	mToolbar->AddSeparator();

	for (int i = 2; i < 5; ++i)
	{
		mToolbar->AddTool(ids[i], bitmaps[i], captions[i]);
	}

	mToolbar->AddSeparator();

	for (int i = 5; i < 8; ++i)
	{
		mToolbar->AddTool(ids[i], bitmaps[i], captions[i]);
	}

	mToolbar->AddSeparator();
	mToolbar->AddTool(ids[8], bitmaps[8], captions[8]);
	mToolbar->AddSeparator();
	mToolbar->AddTool(ids[9], bitmaps[9], captions[9]);
	mToolbar->SetToolBitmapSize(wxSize(32, 32));
	mToolbar->Realize();

	// Create menu.
	mMenu = new Menu(this);

	// Create panes.
	wxPanel* panel = new wxPanel(this);
	mRender = new Render(panel);
	mNotebook = new Notebook(panel);
	mLog = new Log(panel);

	// Setup panes.
	mPaneManager = new wxAuiManager(panel);

	mPaneManager->AddPane(mRender, wxCENTER);
	mPaneManager->AddPane(mNotebook, wxRIGHT);
	mPaneManager->AddPane(mLog, wxBOTTOM);

	mPaneManager->GetPane(mRender).CaptionVisible(false);
	mPaneManager->GetPane(mNotebook).CaptionVisible(false);
	mPaneManager->GetPane(mLog).CaptionVisible(false);

	mPaneManager->GetPane(mNotebook).MinSize(250, 400);
	mPaneManager->GetPane(mLog).MinSize(200, 150);

	mPaneManager->Update();

	// Setup sizer.
	mSizer = new wxBoxSizer(wxVERTICAL);
	mSizer->Add(mToolbar, 0, wxEXPAND);
	mSizer->Add(panel, 1, wxEXPAND);
	SetSizer(mSizer);

	// Create dialogs.
	new ConnectDialog(this);
	new CreateMapDialog(this);
	new OpenMapDialog(this);
	new SearchDialog(this);
	new RouteSearchDialog(this);

	// Events
	Connect(TOOL_NONE, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Main::onButtonClick), NULL, this);
	Connect(TOOL_OBJECTS_SELECT, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Main::onButtonClick), NULL, this);
	Connect(TOOL_OBJECTS_MOVE, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Main::onButtonClick), NULL, this);

	Connect(wxEVT_CLOSE_WINDOW,
		wxCloseEventHandler(Main::onClose), NULL, this);
}

void Main::onButtonClick(wxCommandEvent& event)
{
	if (event.GetId() != TOOL_NONE)
	{
		Notebook::getSingleton().setActivePage(PAGE_TOOLS);
	}

	Tools::getSingleton().setActiveTool(event.GetId());
}

void Main::onClose(wxCloseEvent& event)
{


	// Stop rendering
//	Render::getSingleton().stop();

	// Shutdown 3D
//	Ogre::Root::getSingleton().shutdown();

	// Disconnect events (to avoid errors during shutdown)
	Explorer::getSingleton().disconnectEvents();

	// Close main window
	Destroy();
}
