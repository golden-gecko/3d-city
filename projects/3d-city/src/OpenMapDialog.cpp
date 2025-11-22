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
#include "OpenMapDialog.h"
#include "City.h"
#include "Explorer.h"
#include "Log.h"
#include "Render.h"
#include "TreeItemData.h"

IMPLEMENT_SINGLETON(OpenMapDialog)

OpenMapDialog::OpenMapDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, "Open map",
	wxDefaultPosition, wxSize(500, 400), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	mSizeLatitude = new wxTextCtrl(this, wxID_ANY, "1000");
	mSizeLongitude = new wxTextCtrl(this, wxID_ANY, "1000");

	mBuildings = new wxCheckBox(this, wxID_ANY, "");
	mRoutes = new wxCheckBox(this, wxID_ANY, "");
	mPlants = new wxCheckBox(this, wxID_ANY, "");
	mWaterObjects = new wxCheckBox(this, wxID_ANY, "");

	mBuildings->SetValue(true);
	mRoutes->SetValue(true);
	mPlants->SetValue(true); 
	mWaterObjects->SetValue(true);

//	mBuildingTypes = new wxChoice(this, wxID_ANY);
//	mBuildingColour = new wxTextCtrl(this, wxID_ANY);
//	mRouteTypes = new wxChoice(this, wxID_ANY);
//	mRouteColour = new wxTextCtrl(this, wxID_ANY);

	mGrid = new wxFlexGridSizer(6, 2, 5, 5);

	mGrid->Add(new wxStaticText(this, wxID_ANY, "Width (X = longitude)"), 1, wxEXPAND);
	mGrid->Add(mSizeLongitude, 1, wxEXPAND);
	mGrid->Add(new wxStaticText(this, wxID_ANY, "Height (Y = latitude)"), 1, wxEXPAND);
	mGrid->Add(mSizeLatitude, 1, wxEXPAND);

	mGrid->Add(new wxPanel(this));
	mGrid->Add(new wxPanel(this));

	mGrid->Add(new wxStaticText(this, wxID_ANY, "Buildings"), 1, wxEXPAND);
	mGrid->Add(mBuildings, 1, wxEXPAND);
	mGrid->Add(new wxStaticText(this, wxID_ANY, "Plants"), 1, wxEXPAND);
	mGrid->Add(mPlants, 1, wxEXPAND);
	mGrid->Add(new wxStaticText(this, wxID_ANY, "Routes"), 1, wxEXPAND);
	mGrid->Add(mRoutes, 1, wxEXPAND);
	mGrid->Add(new wxStaticText(this, wxID_ANY, "Water objects"), 1, wxEXPAND);
	mGrid->Add(mWaterObjects, 1, wxEXPAND);

	mGrid->Add(new wxPanel(this));
	mGrid->Add(new wxPanel(this));

//	mGrid->Add(mBuildingTypes, 1, wxEXPAND);
//	mGrid->Add(mBuildingColour, 1, wxEXPAND);
//	mGrid->Add(mRouteTypes, 1, wxEXPAND);
//	mGrid->Add(mRouteColour, 1, wxEXPAND);

	mGrid->AddGrowableCol(0, 1);
	mGrid->AddGrowableCol(1, 1);

	mMaps = new wxTreeCtrl(this, wxID_ANY, wxPoint(), wxSize(),
		wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_HIDE_ROOT | wxTR_MULTIPLE);
	mMaps->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxTreeEventHandler(OpenMapDialog::onMapSelect), NULL, this);
	mMaps->AddRoot("");

	mMain = new wxBoxSizer(wxHORIZONTAL);
	mMain->Add(mMaps, 1, wxEXPAND | wxRIGHT, 10);
	mMain->Add(mGrid, 2, wxEXPAND);

	mOk = new wxButton(this, wxID_ANY, "Open");
	mCancel = new wxButton(this, wxID_ANY, "Cancel");

	mOk->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(OpenMapDialog::onOk), NULL, this);
	mCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(OpenMapDialog::onCancel), NULL, this);

	mButtons = new wxBoxSizer(wxHORIZONTAL);
	mButtons->Add(mOk, 0, wxRIGHT, 5);
	mButtons->Add(mCancel);

	mSizer = new wxBoxSizer(wxVERTICAL);
	mSizer->Add(mMain, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
	mSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
	mSizer->Add(mButtons, 0, wxALIGN_RIGHT | wxALL, 5);

	SetMinSize(wxSize(500, 400));
	SetSizer(mSizer);
}

int OpenMapDialog::ShowModal()
{
	// Clear map list.
	mMaps->DeleteChildren(mMaps->GetRootItem());

	// Refresh database list.
	const DatabaseList& databases = City::getSingleton().getDatabases();

	for (DatabaseList::const_iterator d = databases.begin(); d != databases.end(); ++d)
	{
		std::string name =
			d->second->getName() + " (" +
			Convert::ToString(d->second->getID()) + ")";

		wxTreeItemId databaseID = mMaps->AppendItem(mMaps->GetRootItem(),
			name, -1, -1, new TreeItemData(Identity(d->second)));

		// Refresh map list.
		const MapList& maps = d->second->getMaps();

		for (MapList::const_iterator m = maps.begin(); m != maps.end(); ++m)
		{
			std::string name =
				m->second->getName() + " (" +
				Convert::ToString(m->second->getID()) + ")";

			mMaps->AppendItem(databaseID, name, -1, -1,
				new TreeItemData(Identity(m->second)));
		}

		mMaps->SortChildren(databaseID);
		mMaps->Expand(databaseID);
	}

	mMaps->SortChildren(mMaps->GetRootItem());

	return wxDialog::Show();
}

void OpenMapDialog::onMapSelect(wxTreeEvent& event)
{
	// Clear colours.
	mBuildingTypes->Clear();
	mBuildingColour->SetValue("");
	mRouteTypes->Clear();
	mRouteColour->SetValue("");

	// Refresh colours.
	TreeItemData* itemData = static_cast<TreeItemData*>
		(mMaps->GetItemData(event.GetItem()));

	if (Map* map = itemData->identity.getMap())
	{
	}
}

void OpenMapDialog::onOk(wxCommandEvent& event)
{
	try
	{
		wxArrayTreeItemIds selection;
		mMaps->GetSelections(selection);

		if (selection.size())
		{
			for (wxArrayTreeItemIds::size_type i = 0; i < selection.size(); ++i)
			{
				TreeItemData* itemData = static_cast<TreeItemData*>
					(mMaps->GetItemData(selection[i]));

				if (itemData == NULL)
				{
					continue;
				}

				Map* map = itemData->identity.getMap();

				if (map == NULL)
				{
					continue;
				}

				double latScale = Convert::Parse<double>(mSizeLatitude->GetValue().c_str());
				double lonScale = Convert::Parse<double>(mSizeLongitude->GetValue().c_str());

				unsigned int buildings = mBuildings->GetValue() ? OT_BUILDING : 0;
				unsigned int plants = mPlants->GetValue() ? OT_PLANT : 0;
				unsigned int routes = mRoutes->GetValue() ? OT_JUNCTION | OT_ROUTE | OT_STREET : 0;
				unsigned int waterObjects = mWaterObjects->GetValue() ? OT_WATER_OBJECT : 0;

				MapView* mapView = City::getSingleton().createView(map,
					buildings | plants | routes | waterObjects);

				if (mapView == NULL)
				{
					Log::getSingleton().error("Error opening map \"" + map->getName() + "\".");

					continue;
				}

				Log::getSingleton().info("Map \"" + map->getName() + "\" opened.");

				// Render selected map.
				Render::getSingleton().select(mapView);
			}

			Explorer::getSingleton().refreshView();

			Close();
		}
		else
		{
			Log::getSingleton().error("There is no map selected.");
		}
	}
	catch (...)
	{
		Log::getSingleton().error("OpenMapDialog::onOk error.");
	}
}

void OpenMapDialog::onCancel(wxCommandEvent& event)
{
	Close();
}
