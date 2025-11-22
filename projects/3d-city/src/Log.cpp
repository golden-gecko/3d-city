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
#include "Log.h"

IMPLEMENT_SINGLETON(Log)

Log::Log(wxWindow* parent) : wxRichTextCtrl(parent, wxID_ANY)
{
	BeginFont(wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
		wxFONTWEIGHT_NORMAL));
	SetEditable(false);
}

void Log::error(const std::string& text)
{
	BeginTextColour(wxColour(255, 0, 0));
	AppendText(wxDateTime::Now().FormatTime() + " " + text + "\n");
	ShowPosition(GetLastPosition());
}

void Log::info(const std::string& text)
{
	BeginTextColour(wxColour(0, 128, 0));
	AppendText(wxDateTime::Now().FormatTime() + " " + text + "\n");
	ShowPosition(GetLastPosition());
}
