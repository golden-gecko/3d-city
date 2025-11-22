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
#include "Explorer.h"
#include "GUI.h"
#include "Info.h"
#include "Log.h"
#include "Menu.h"
#include "Properties.h"
#include "Render.h"
#include "Tools.h"
#include "TreeItemData.h"

IMPLEMENT_SINGLETON(Explorer)

Explorer::Explorer(wxWindow* parent) : wxPanel(parent)
{
	mLocal = new wxTreeCtrl(this, wxID_ANY, wxPoint(), wxSize(),
		wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_HIDE_ROOT | wxTR_MULTIPLE);
	mView = new wxTreeCtrl(this, wxID_ANY, wxPoint(), wxSize(),
		wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_HIDE_ROOT | wxTR_MULTIPLE);

	mLocal->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED,
		wxTreeEventHandler(Explorer::onLocalItemSelected), NULL, this);
	mLocal->Connect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,
		wxTreeEventHandler(Explorer::onItemRightClick), NULL, this);
	mView->Connect(wxEVT_COMMAND_TREE_SEL_CHANGED,
		wxTreeEventHandler(Explorer::onViewItemSelected), NULL, this);
	mView->Connect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,
		wxTreeEventHandler(Explorer::onItemRightClick), NULL, this);

	mLocal->AddRoot("");
	mView->AddRoot("");

	mRefreshLocal = new wxButton(this, wxID_ANY, "Refresh");
	mRefreshView = new wxButton(this, wxID_ANY, "Refresh");

	mRefreshLocal->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxTreeEventHandler(Explorer::onRefreshLocalClick), NULL, this);
	mRefreshView->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxTreeEventHandler(Explorer::onRefreshViewClick), NULL, this);

	mSizer = new wxBoxSizer(wxVERTICAL);
	mSizer->Add(mLocal, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
	mSizer->Add(mRefreshLocal, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
	mSizer->Add(mView, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
	mSizer->Add(mRefreshView, 0, wxEXPAND | wxALL, 5);

	SetSizer(mSizer);

	// Causes memory leaks.
	//mMenu = new wxMenu();
	//mMenu->Append(DATABASE_CONNECT, "Connect...");
	//mMenu->Append(DATABASE_DISCONNECT, "Disconnect");
	//mMenu->AppendSeparator();
	//mMenu->Append(wxID_ANY, "Select");
	//mMenu->Append(wxID_ANY, "Delete");
	//mMenu->Append(wxID_ANY, "Update");
	//mMenu->AppendSeparator();
	//mMenu->Append(MAP_OPEN, "Open...");
	//mMenu->Append(MAP_CLOSE, "Close");
}

void Explorer::disconnect()
{
	wxArrayTreeItemIds selection;
	mLocal->GetSelections(selection);

	for (unsigned int i = 0; i < selection.size(); ++i)
	{
		TreeItemData* itemData = static_cast<TreeItemData*>(mLocal->GetItemData(selection[0]));

		if (Database* database = itemData->identity.getDatabase())
		{
			City::getSingleton().closeDatabase(database->getID());
		}
	}

	refresh();
}

void Explorer::openMaps()
{
	wxArrayTreeItemIds selection;
	mLocal->GetSelections(selection);

	for (unsigned int i = 0; i < selection.size(); ++i)
	{
		TreeItemData* itemData = static_cast<TreeItemData*>(mLocal->GetItemData(selection[0]));

		if (Map* map = itemData->identity.getMap())
		{
			Render::getSingleton().select(
				City::getSingleton().createView(map));
		}
	}

	refreshView();
}

void Explorer::closeMaps()
{
	wxArrayTreeItemIds selection;
	mView->GetSelections(selection);

	for (unsigned int i = 0; i < selection.size(); ++i)
	{
		TreeItemData* itemData = static_cast<TreeItemData*>(mView->GetItemData(selection[0]));

		if (MapView* mapView = itemData->identity.getMapView())
		{
			// Unselect view from render window.
			if (Render::getSingleton().getActiveMapView() == mapView)
			{
				Render::getSingleton().unselect();
			}

			// Destroy view.
			City::getSingleton().destroyView(mapView);
		}
	}

	refreshView();
}

void Explorer::deleteMaps()
{
	wxArrayTreeItemIds selection;
	mLocal->GetSelections(selection);

	for (unsigned int i = 0; i < selection.size(); ++i)
	{
		TreeItemData* itemData = static_cast<TreeItemData*>
			(mLocal->GetItemData(selection[0]));

		if (Map* map = itemData->identity.getMap())
		{
			// Unselect map from render window.
			if (MapView* mapView = Render::getSingleton().getActiveMapView())
			{
				if (mapView->getMap() == map)
				{
					Render::getSingleton().unselect();
				}
			}

			// Destroy all map views.
			City::getSingleton().destroyViews(map);	

			// Destroy map.
			map->getDatabase()->destroyMap(map->getID());
		}
	}

	GUI::getSingleton().refresh();
}

void Explorer::deleteObjects()
{
	wxArrayTreeItemIds selection;
	mLocal->GetSelections(selection);

	for (unsigned int i = 0; i < selection.size(); ++i)
	{
		TreeItemData* itemData = static_cast<TreeItemData*>
			(mLocal->GetItemData(selection[0]));

		if (Object* object = itemData->identity.getObject())
		{
			// Destroy all map views.
			City::getSingleton().destroyViews(object);

			// Destroy map.
			object->getMap()->destroyObject(object->getID());
		}
	}

	GUI::getSingleton().refresh();
}

void Explorer::refreshLocal()
{
	// Clear list.
	mLocal->DeleteChildren(mLocal->GetRootItem());

	// Refresh database list.
	const DatabaseList& databases = City::getSingleton().getDatabases();

	for (DatabaseList::const_iterator d = databases.begin(); d != databases.end(); ++d)
	{
		wxTreeItemId databaseID = mLocal->AppendItem(mLocal->GetRootItem(),
			d->second->getName() + " (" + Convert::ToString(d->second->getID()) + ")",
			-1, -1, new TreeItemData(Identity(d->second)));

		// Refresh map list in each database.
		const MapList& maps = d->second->getMaps();

		for (MapList::const_iterator m = maps.begin(); m != maps.end(); ++m)
		{
		wxTreeItemId mapID = mLocal->AppendItem(databaseID,
			m->second->getName() + " (" + Convert::ToString(m->second->getID()) + ")",
			-1, -1, new TreeItemData(Identity(m->second)));

			// Refresh object list in each map.
			const ObjectList& objects = m->second->getObjects();

			std::map<ObjectType, wxTreeItemId> types;

			types.insert(std::make_pair(OT_BUILDING, mLocal->AppendItem(mapID,
				"Buildings", -1, -1, new TreeItemData())));
			types.insert(std::make_pair(OT_PLANT, mLocal->AppendItem(mapID,
				"Plants", -1, -1, new TreeItemData())));
			types.insert(std::make_pair(OT_ROUTE, mLocal->AppendItem(mapID, 
				"Routes", -1, -1, new TreeItemData())));
			types.insert(std::make_pair(OT_JUNCTION, mLocal->AppendItem(mapID,
				"Junctions", -1, -1, new TreeItemData())));
			types.insert(std::make_pair(OT_WATER_OBJECT, mLocal->AppendItem(mapID,
				"Water objects", -1, -1, new TreeItemData())));
			types.insert(std::make_pair(OT_STREET, mLocal->AppendItem(mapID,
				"Streets", -1, -1, new TreeItemData())));

			for (ObjectList::const_iterator o = objects.begin(); o != objects.end(); ++o)
			{
				mLocal->AppendItem(types[o->second->getType()],
					o->second->getName() + " (" + Convert::ToString(o->second->getID()) + ")",
					-1, -1, new TreeItemData(Identity(o->second)));
			}

			std::map<ObjectType, wxTreeItemId>::const_iterator i;

			for (i = types.begin(); i != types.end(); ++i)
			{
				mLocal->SortChildren(i->second);
			}

			mLocal->SortChildren(mapID);
			mLocal->Expand(mapID);
		}

		// Sort and expand maps.
		mLocal->SortChildren(databaseID);
		mLocal->Expand(databaseID);
	}

	// Sort databases.
	mLocal->SortChildren(mLocal->GetRootItem());
}

void Explorer::refreshView()
{
	// Clear list.
	mView->DeleteChildren(mLocal->GetRootItem());

	// Refresh list.
	const MapViewList& mapViews = City::getSingleton().getMapViews();

	for (MapViewList::const_iterator m = mapViews.begin(); m != mapViews.end(); ++m)
	{
		std::string name =
			m->second->getMap()->getName() + " (" +
			Convert::ToString(m->second->getMap()->getID()) + ")";

		wxTreeItemId mapViewID = mView->AppendItem(mView->GetRootItem(),
			name, -1, -1, new TreeItemData(Identity(m->second)));

		const ObjectViewList& objects = m->second->getObjectViews();

		std::map<ObjectType, wxTreeItemId> types;

			types.insert(std::make_pair(OT_BUILDING, mLocal->AppendItem(mapViewID,
				"Buildings", -1, -1, new TreeItemData())));
			types.insert(std::make_pair(OT_PLANT, mLocal->AppendItem(mapViewID,
				"Plants", -1, -1, new TreeItemData())));
			types.insert(std::make_pair(OT_ROUTE, mLocal->AppendItem(mapViewID, 
				"Routes", -1, -1, new TreeItemData())));
			types.insert(std::make_pair(OT_JUNCTION, mLocal->AppendItem(mapViewID,
				"Junctions", -1, -1, new TreeItemData())));
			types.insert(std::make_pair(OT_WATER_OBJECT, mLocal->AppendItem(mapViewID,
				"Water objects", -1, -1, new TreeItemData())));
			types.insert(std::make_pair(OT_STREET, mLocal->AppendItem(mapViewID,
				"Streets", -1, -1, new TreeItemData())));

		for (ObjectViewList::const_iterator o = objects.begin(); o != objects.end(); ++o)
		{
			std::string name =
				o->second->getObject()->getName() + " (" +
				Convert::ToString(o->second->getObject()->getID()) + ")";

			mView->AppendItem(types[o->second->getObject()->getType()],
				name, -1, -1, new TreeItemData(Identity(o->second)));
		}

		mView->SortChildren(mapViewID);
	}

	mView->SortChildren(mView->GetRootItem());
}

void Explorer::refresh()
{
	refreshLocal();
	refreshView();
}

void Explorer::onLocalItemSelected(wxTreeEvent& event)
{
	if (event.GetItem().IsOk())
	{
		if (TreeItemData* itemData = static_cast<TreeItemData*>(mLocal->GetItemData(event.GetItem())))
		{
			Info::getSingleton().setSelected(itemData->identity);

			try
			{
				Properties::getSingleton().refresh();
			}
			catch (...)
			{
			}

			Tools::getSingleton().refresh();
		}
	}
}

void Explorer::onViewItemSelected(wxTreeEvent& event)
{
	if (event.GetItem().IsOk())
	{
		wxTreeItemId item = event.GetItem();

		if (item.IsOk())
		{
			if (TreeItemData* itemData = static_cast<TreeItemData*>(mView->GetItemData(item)))
			{
				Info::getSingleton().setSelected(itemData->identity);

				Properties::getSingleton().refresh();
				Tools::getSingleton().refresh();
			}
		}
	}

	//

	wxArrayTreeItemIds selection;
	mView->GetSelections(selection);

	if (selection.size() && selection[0].IsOk())
	{
		TreeItemData* itemData = static_cast<TreeItemData*>(mView->GetItemData(selection[0]));

		// Attach map view to render window.
		if (MapView* view = itemData->identity.getMapView())
		{
			Render::getSingleton().select(view);
		}
		// Attach map view to render window and look at object view.
		else if (ObjectView* view = itemData->identity.getObjectView())
		{
			Render::getSingleton().select(view->getMapView());

			view->getMapView()->lookAt(view);
		}
	}
}

void Explorer::onItemRightClick(wxTreeEvent& event)
{
}

void Explorer::onRefreshLocalClick(wxTreeEvent& event)
{
	refreshLocal();
}

void Explorer::onRefreshViewClick(wxTreeEvent& event)
{
	refreshView();
}
