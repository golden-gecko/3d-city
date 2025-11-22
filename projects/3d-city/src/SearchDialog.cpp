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
#include "GUI.h"
#include "Info.h"
#include "SearchDialog.h"

IMPLEMENT_SINGLETON(SearchDialog)

SearchDialog::SearchDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY,
	"Find objects", wxDefaultPosition, wxDefaultSize,
	wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	mMask = new wxTextCtrl(this, wxID_ANY);
	mSearch = new wxButton(this, wxID_ANY, "Search");
	mCaseSensitive = new wxCheckBox(this, wxID_ANY, "Case sensitive");
	mResults = new wxListBox(this, wxID_ANY, wxPoint(), wxSize(), 0, 0, wxLB_SORT);

	mSearch->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(SearchDialog::onSearchClick), NULL, this);
	mResults->Connect(wxEVT_COMMAND_LISTBOX_SELECTED,
		wxCommandEventHandler(SearchDialog::onResultSelect), NULL, this);

	mSizer = new wxFlexGridSizer(2, 2, 5, 5);
	mSizer->Add(mMask, 1, wxEXPAND | wxLEFT | wxTOP, 5);
	mSizer->Add(mSearch, 1, wxEXPAND | wxRIGHT | wxTOP, 5);
	mSizer->Add(mCaseSensitive, 1, wxEXPAND | wxLEFT, 5);
	mSizer->Add(new wxPanel(this));
	mSizer->Add(mResults, 1, wxEXPAND | wxBOTTOM | wxLEFT, 5);
	mSizer->Add(new wxPanel(this));
	mSizer->AddGrowableCol(0, 1);
	mSizer->AddGrowableRow(2, 1);

	Connect(wxEVT_ENTER_WINDOW,
		wxMouseEventHandler(SearchDialog::onEnterWindow), NULL, this);

	SetSizer(mSizer);
}

void SearchDialog::onEnterWindow(wxMouseEvent& event)
{
	SetFocus();
}

void SearchDialog::onSearchClick(wxCommandEvent& event)
{
	// Trim mask from whitespace.
	wxString wxMask = mMask->GetValue();

	wxMask.Trim();
	wxMask.Trim(false);

	// Convert to lower case.
	std::string name, mask = wxMask;

	if (mCaseSensitive->GetValue() == false)
	{
		std::transform(mask.begin(), mask.end(), mask.begin(), tolower);
	}

	// Clear last results.
	mResults->Clear();

	// Find objects.
	const DatabaseList& databases = City::getSingleton().getDatabases();

	for (DatabaseList::const_iterator d = databases.begin(); d != databases.end(); ++d)
	{
		const MapList& maps = d->second->getMaps();

		for (MapList::const_iterator m = maps.begin(); m != maps.end(); ++m)
		{
			const ObjectList& objects = m->second->getObjects();

			for (ObjectList::const_iterator o = objects.begin(); o != objects.end(); ++o)
			{
				name = o->second->getName();

				if (mCaseSensitive->GetValue() == false)
				{
					std::transform(name.begin(), name.end(), name.begin(), tolower);
				}

				if (name.find(mask) != std::string::npos)
				{
					mResults->Append(o->second->getName(), o->second);
				}
			}
		}
	}
}

void SearchDialog::onResultSelect(wxCommandEvent& event)
{
	int selection = mResults->GetSelection();
	void* clientData = mResults->GetClientData(selection);

	if (Object* object = static_cast<Object*>(clientData))
	{
		Info::getSingleton().setSelected(object);
		GUI::getSingleton().refresh();
	}
}
