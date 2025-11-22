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
#include "RouteSearchDialog.h"

IMPLEMENT_SINGLETON(RouteSearchDialog)

RouteSearchDialog::RouteSearchDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY,
	"Find route", wxDefaultPosition, wxDefaultSize,
	wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	mPoints = new wxListBox(this, wxID_ANY, wxPoint(), wxSize(), 0, 0, wxLB_SORT);
	mAdd = new wxToggleButton(this, wxID_ANY, "+", wxPoint(), wxSize(25, 25));
	mDelete = new wxButton(this, wxID_ANY, "-", wxPoint(), wxSize(25, 25));

	mMaxSpeed = new wxRadioButton(this, wxID_ANY, "Maximal speed");
	mAvgSpeed = new wxRadioButton(this, wxID_ANY, "Avarage speed");
	mLenght = new wxRadioButton(this, wxID_ANY, "Lenght");

	mSearch = new wxButton(this, wxID_ANY, "Find");
	mResults = new wxListBox(this, wxID_ANY, wxPoint(), wxSize(), 0, 0, wxLB_SORT);

	mSearch->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(RouteSearchDialog::onSearchClick), NULL, this);
	mResults->Connect(wxEVT_COMMAND_LISTBOX_SELECTED,
		wxCommandEventHandler(RouteSearchDialog::onResultSelect), NULL, this);

	mButtonSizer = new wxBoxSizer(wxVERTICAL);
	mButtonSizer->Add(mAdd, 0, wxBOTTOM, 5);
	mButtonSizer->Add(mDelete);

	mListSizer = new wxBoxSizer(wxHORIZONTAL);
	mListSizer->Add(mPoints, 1, wxEXPAND | wxRIGHT, 5);
	mListSizer->Add(mButtonSizer);

	mSizer = new wxBoxSizer(wxVERTICAL);
	mSizer->Add(mListSizer, 1, wxEXPAND | wxALL, 5);
	mSizer->Add(new wxStaticLine(this, wxHORIZONTAL), 0, wxEXPAND | wxRIGHT | wxLEFT | wxBOTTOM, 5);
	mSizer->Add(mMaxSpeed, 0, wxEXPAND | wxRIGHT | wxLEFT | wxBOTTOM, 5);
	mSizer->Add(mAvgSpeed, 0, wxEXPAND | wxRIGHT | wxLEFT | wxBOTTOM, 5);
	mSizer->Add(mLenght, 0, wxEXPAND | wxRIGHT | wxLEFT | wxBOTTOM, 5);
	mSizer->Add(mSearch, 0, wxEXPAND | wxRIGHT | wxLEFT | wxBOTTOM, 5);
	mSizer->Add(mResults, 1, wxEXPAND | wxRIGHT | wxLEFT | wxBOTTOM, 5);

	Connect(wxEVT_ENTER_WINDOW,
		wxMouseEventHandler(RouteSearchDialog::onEnterWindow), NULL, this);
	mDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(RouteSearchDialog::onDeleteClick), NULL, this);

	SetSizer(mSizer);
}

void RouteSearchDialog::onEnterWindow(wxMouseEvent& event)
{
	SetFocus();
}

void RouteSearchDialog::onDeleteClick(wxCommandEvent& event)
{
	wxArrayInt selections;
	mPoints->GetSelections(selections);

	for (int i = (int)selections.size() - 1; i >= 0; --i)
	{
		mPoints->Delete(selections[i]);
	}
}

void RouteSearchDialog::onSearchClick(wxCommandEvent& event)
{
}

void RouteSearchDialog::onResultSelect(wxCommandEvent& event)
{
}
