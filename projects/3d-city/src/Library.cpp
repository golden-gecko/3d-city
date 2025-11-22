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
#include "Library.h"

IMPLEMENT_SINGLETON(Library)

Library::Library(wxWindow* parent) : wxPanel(parent)
{
	mTemplates = new wxTreeCtrl(this, wxID_ANY, wxPoint(), wxSize(),
		wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_HIDE_ROOT);
	
	mRoot = mTemplates->AddRoot("Templates");

	mBuildings = mTemplates->AppendItem(mTemplates->GetRootItem(), "Buildings");
	mRoutes = mTemplates->AppendItem(mTemplates->GetRootItem(), "Routes");
	mPlants = mTemplates->AppendItem(mTemplates->GetRootItem(), "Plants");
	mOthers = mTemplates->AppendItem(mTemplates->GetRootItem(), "Others");

	mSizer = new wxBoxSizer(wxVERTICAL);
	mSizer->Add(mTemplates, 1, wxEXPAND);

	SetSizer(mSizer);
}
