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
#include "OpenMapDialog.h"
#include "Main.h"
#include "Menu.h"
#include "CreateMapDialog.h"
#include "SearchDialog.h"
#include "RouteSearchDialog.h"

Menu::Menu(wxFrame* parent)
{
	mDatabase = new wxMenu();
	mMap = new wxMenu();
	mObject = new wxMenu();
	mEdit = new wxMenu();
	mRender = new wxMenu();
	mView = new wxMenu();
	mTools = new wxMenu();
	mHelp = new wxMenu();

	mDatabase->Append(DATABASE_CONNECT, "Connect...\tCtrl+Shift+C");
	mDatabase->Append(DATABASE_DISCONNECT, "Disconnect\tCtrl+Shift+D");
	mDatabase->AppendSeparator();
	mDatabase->Append(DATABASE_EXIT, "Exit\tAlt+F4");

	mMap->Append(MAP_NEW, "New...\tCtrl+N");
	mMap->Append(MAP_OPEN, "Open...\tCtrl+O");
//	mMap->Append(MAP_RELOAD, "Reload\tF5");
	mMap->Append(MAP_SAVE, "Save\tCtrl+S");
//	mMap->Append(MAP_SAVE_AS, "Save as...");
//	mMap->Append(MAP_SAVE_ALL, "Save all\tCtrl+Shift+S");
	mMap->Append(MAP_CLOSE, "Close");
	mMap->Append(MAP_DELETE, "Delete");

	mObject->Append(OBJECT_DELETE, "Delete\tDelete");

	mEdit->Append(EDIT_SEARCH, "Find objects...\tCtrl+F");
//	mEdit->Append(EDIT_ROUTE_SEARCH, "Find route...");

	mRender->Append(RENDER_START, "Start");
	mRender->Append(RENDER_STOP, "Stop");

	mView->AppendCheckItem(VIEW_FULLSCREEN, "Fullscreen\tAlt+Enter");
	mView->AppendSeparator();
	mView->AppendCheckItem(VIEW_TOOLS, "Tools\tAlt+T")->Check();
	mView->AppendCheckItem(VIEW_LOG, "Log\tAlt+L")->Check();

	mTools->Append(TOOLS_OPTIONS, "Display options");
	mTools->AppendSeparator();
	mTools->Append(TOOLS_CLEAR_CACHE, "Clear cache");

	mHelp->Append(HELP_DOCUMENTATION, "Help\tF1");
	mHelp->AppendSeparator();
	mHelp->Append(HELP_ABOUT, "About");

	Append(mDatabase, "Database");
	Append(mMap, "Map");
	Append(mObject, "Object");
	Append(mEdit, "Edit");
	Append(mRender, "Render");
	Append(mView, "View");
	Append(mTools, "Tools");
	Append(mHelp, "Help");

	parent->SetMenuBar(this);

	parent->Connect(DATABASE_CONNECT, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onDatabaseConnect));
	parent->Connect(DATABASE_DISCONNECT, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onDatabaseDisconnect));
	parent->Connect(DATABASE_EXIT, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onDatabaseExit));

	parent->Connect(EDIT_SEARCH, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onEditSearch));
	parent->Connect(EDIT_ROUTE_SEARCH, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onEditRouteSearch));

	parent->Connect(MAP_NEW, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onMapNew));
	parent->Connect(MAP_OPEN, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onMapOpen));
	parent->Connect(MAP_RELOAD, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onMapReload));
	parent->Connect(MAP_SAVE, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onMapSave));
	parent->Connect(MAP_SAVE_AS, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onMapSaveAs));
	parent->Connect(MAP_CLOSE, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onMapClose));
	parent->Connect(MAP_DELETE, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onMapDelete));

	parent->Connect(OBJECT_DELETE, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onObjectDelete));

	parent->Connect(RENDER_START, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onRenderStart));
	parent->Connect(RENDER_STOP, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onRenderStop));

	parent->Connect(VIEW_FULLSCREEN, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onViewFullscreen));
	parent->Connect(VIEW_TOOLS, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onViewTools));
	parent->Connect(VIEW_LOG, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onViewLog));

	parent->Connect(TOOLS_OPTIONS, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onToolsOptions));
	parent->Connect(TOOLS_CLEAR_CACHE, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onToolsCache));

	parent->Connect(HELP_DOCUMENTATION, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onHelpDocs));
	parent->Connect(HELP_ABOUT, wxEVT_COMMAND_MENU_SELECTED,
		wxCommandEventHandler(Menu::onHelpAbout));
}

void Menu::onDatabaseConnect(wxCommandEvent& event)
{
	ConnectDialog::getSingleton().ShowModal();
}

void Menu::onDatabaseDisconnect(wxCommandEvent& event)
{
	Explorer::getSingleton().disconnect();
}

void Menu::onDatabaseExit(wxCommandEvent& event)
{
	Main::getSingleton().Close();
}

void Menu::onMapNew(wxCommandEvent& event)
{
	CreateMapDialog::getSingleton().ShowModal();
}

void Menu::onMapOpen(wxCommandEvent& event)
{
	OpenMapDialog::getSingleton().ShowModal();
}

void Menu::onMapReload(wxCommandEvent& event)
{
}

void Menu::onMapSave(wxCommandEvent& event)
{
	const DatabaseList& db = City::getSingleton().getDatabases();

	for (DatabaseList::const_iterator i = db.begin(); i != db.end(); ++i)
	{
		if (i->second->getConnection()->update())
		{
			Log::getSingleton().info("Database " + i->second->getName() + " saved.");
		}
		else
		{
			Log::getSingleton().info("Error saving database " + i->second->getName() + ".");
		}
	}
}

void Menu::onMapSaveAs(wxCommandEvent& event)
{
	const DatabaseList& db = City::getSingleton().getDatabases();

	for (DatabaseList::const_iterator i = db.begin(); i != db.end(); ++i)
	{
		if (i->second->getConnection()->update())
		{
			Log::getSingleton().info("Database " + i->second->getName() + " saved.");
		}
		else
		{
			Log::getSingleton().info("Error saving database " + i->second->getName() + ".");
		}
	}
}

void Menu::onMapSaveAll(wxCommandEvent& event)
{
	const DatabaseList& db = City::getSingleton().getDatabases();

	for (DatabaseList::const_iterator i = db.begin(); i != db.end(); ++i)
	{
		if (i->second->getConnection()->update())
		{
			Log::getSingleton().info("Database " + i->second->getName() + " saved.");
		}
		else
		{
			Log::getSingleton().info("Error saving database " + i->second->getName() + ".");
		}
	}
}

void Menu::onMapClose(wxCommandEvent& event)
{
	Explorer::getSingleton().closeMaps();
}

void Menu::onMapDelete(wxCommandEvent& event)
{
	Explorer::getSingleton().deleteMaps();
}

void Menu::onObjectDelete(wxCommandEvent& event)
{
	Explorer::getSingleton().deleteObjects();
}

void Menu::onEditSearch(wxCommandEvent& event)
{
	SearchDialog::getSingleton().Show(
		!(SearchDialog::getSingleton().IsShown()));
}

void Menu::onEditRouteSearch(wxCommandEvent& event)
{
	RouteSearchDialog::getSingleton().Show(
		!(RouteSearchDialog::getSingleton().IsShown()));
}

void Menu::onRenderStart(wxCommandEvent& event)
{
	Render::getSingleton().start();
}

void Menu::onRenderStop(wxCommandEvent& event)
{
	Render::getSingleton().stop();
}

void Menu::onViewFullscreen(wxCommandEvent& event)
{
	Main::getSingleton().ShowFullScreen(
		!(Main::getSingleton().IsFullScreen()));
}

void Menu::onViewTools(wxCommandEvent& event)
{
	Main::getSingleton().setToolsVisible(
		!(Main::getSingleton().getToolsVisible()));
}

void Menu::onViewLog(wxCommandEvent& event)
{
	Main::getSingleton().setLogVisible(
		!(Main::getSingleton().getLogVisible()));
}

void Menu::onToolsOptions(wxCommandEvent& event)
{
	wxMessageDialog msg(
		this,
		"Changing display options requires aplication restart.\n\nDo you want to continue?",
		"Display options",
		wxYES_NO | wxICON_WARNING);

	if (msg.ShowModal() == wxID_YES)
	{
		DeleteFile("Ogre.cfg");

		Close();
	}
}

void Menu::onToolsCache(wxCommandEvent& event)
{
	WIN32_FIND_DATA wfd;

	if (HANDLE hFind = FindFirstFile("..\\cache\\*.*", &wfd))
	{
		do
		{
			std::string name("..\\cache\\");
			name += wfd.cFileName;

			DeleteFile(name.c_str());
		}
		while (FindNextFile(hFind, &wfd));
	}
}

void Menu::onHelpDocs(wxCommandEvent& event)
{
}

void Menu::onHelpAbout(wxCommandEvent& event)
{
}
