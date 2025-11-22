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

enum MenuItem
{
	DATABASE_CONNECT = 100,
	DATABASE_DISCONNECT,
	DATABASE_EXIT,

	MAP_NEW,
	MAP_OPEN,
	MAP_RELOAD,
	MAP_SAVE,
	MAP_SAVE_AS,
	MAP_SAVE_ALL,
	MAP_CLOSE,
	MAP_DELETE,

	EDIT_SEARCH,
	EDIT_ROUTE_SEARCH,

	OBJECT_DELETE,

	RENDER_START,
	RENDER_STOP,

	VIEW_FULLSCREEN,
	VIEW_TOOLS,
	VIEW_LOG,

	TOOLS_OPTIONS,
	TOOLS_CLEAR_CACHE,

	HELP_DOCUMENTATION,
	HELP_ABOUT
};

class Menu : public wxMenuBar
{
public:
	Menu(wxFrame* parent);
protected:
	wxMenu* mDatabase;
	wxMenu* mMap;
	wxMenu* mObject;
	wxMenu* mEdit;
	wxMenu* mRender;
	wxMenu* mView;
	wxMenu* mTools;
	wxMenu* mHelp;

	void onDatabaseConnect(wxCommandEvent& event);
	void onDatabaseDisconnect(wxCommandEvent& event);
	void onDatabaseReconnect(wxCommandEvent& event);
	void onDatabaseRemove(wxCommandEvent& event);
	void onDatabaseExit(wxCommandEvent& event);

	void onMapNew(wxCommandEvent& event);
	void onMapOpen(wxCommandEvent& event);
	void onMapReload(wxCommandEvent& event);
	void onMapSave(wxCommandEvent& event);
	void onMapSaveAs(wxCommandEvent& event);
	void onMapSaveAll(wxCommandEvent& event);
	void onMapClose(wxCommandEvent& event);
	void onMapDelete(wxCommandEvent& event);

	void onObjectDelete(wxCommandEvent& event);

	void onEditSearch(wxCommandEvent& event);
	void onEditRouteSearch(wxCommandEvent& event);

	void onRenderStart(wxCommandEvent& event);
	void onRenderStop(wxCommandEvent& event);

	void onViewFullscreen(wxCommandEvent& event);
	void onViewTools(wxCommandEvent& event);
	void onViewLog(wxCommandEvent& event);

	void onToolsOptions(wxCommandEvent& event);
	void onToolsCache(wxCommandEvent& event);

	void onHelpDocs(wxCommandEvent& event);
	void onHelpAbout(wxCommandEvent& event);
};
