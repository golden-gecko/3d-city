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
#include "Info.h"

IMPLEMENT_SINGLETON(Info)

Info::Info(wxWindow* parent) : wxPanel(parent)
{
	mLatitude	= new wxStaticText(this, wxID_ANY, "0.0");
	mLongitude	= new wxStaticText(this, wxID_ANY, "0.0");
	mHeight		= new wxStaticText(this, wxID_ANY, "0.0");

	mX = new wxStaticText(this, wxID_ANY, "0.0");
	mY = new wxStaticText(this, wxID_ANY, "0.0");
	mZ = new wxStaticText(this, wxID_ANY, "0.0");

	mFPS = new wxStaticText(this, wxID_ANY, "0.0");

	mSizer = new wxFlexGridSizer(15, 2, 5, 5);

	mSizer->Add(new wxStaticText(this, wxID_ANY, "Latitude"), 1, wxEXPAND | wxLEFT | wxTOP, 10);
	mSizer->Add(mLatitude, 1, wxEXPAND | wxRIGHT | wxTOP, 10);
	mSizer->Add(new wxStaticText(this, wxID_ANY, "Longitude"), 1, wxEXPAND | wxLEFT, 10);
	mSizer->Add(mLongitude, 1, wxEXPAND | wxRIGHT, 10);
	mSizer->Add(new wxStaticText(this, wxID_ANY, "Height"), 1, wxEXPAND | wxLEFT, 10);
	mSizer->Add(mHeight, 1, wxEXPAND | wxRIGHT, 10);

	mSizer->Add(new wxPanel(this, wxID_ANY, wxPoint(), wxSize(10, 10)));
	mSizer->Add(new wxPanel(this, wxID_ANY, wxPoint(), wxSize(10, 10)));

	mSizer->Add(new wxStaticText(this, wxID_ANY, "X"), 1, wxEXPAND | wxLEFT, 10);
	mSizer->Add(mX, 1, wxEXPAND | wxRIGHT, 10);
	mSizer->Add(new wxStaticText(this, wxID_ANY, "Y"), 1, wxEXPAND | wxLEFT , 10);
	mSizer->Add(mY, 1, wxEXPAND | wxRIGHT, 10);
	mSizer->Add(new wxStaticText(this, wxID_ANY, "Z"), 1, wxEXPAND | wxLEFT, 10);
	mSizer->Add(mZ, 1, wxEXPAND | wxRIGHT, 10);

	mSizer->Add(new wxPanel(this, wxID_ANY, wxPoint(), wxSize(10, 10)));
	mSizer->Add(new wxPanel(this, wxID_ANY, wxPoint(), wxSize(10, 10)));

	mSizer->Add(new wxStaticText(this, wxID_ANY, "Average FPS"), 1, wxEXPAND | wxLEFT, 10);
	mSizer->Add(mFPS, 1, wxEXPAND | wxRIGHT, 10);

	mSizer->AddGrowableCol(1, 1);

	SetSizer(mSizer);
}

void Info::refresh()
{
}
