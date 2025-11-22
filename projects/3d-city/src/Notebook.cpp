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
#include "Notebook.h"

IMPLEMENT_SINGLETON(Notebook)

Notebook::Notebook(wxWindow* parent) : wxAuiNotebook(parent)
{
	SetWindowStyle(
		wxAUI_NB_SCROLL_BUTTONS |
		wxAUI_NB_TAB_MOVE |
		wxAUI_NB_TAB_SPLIT |
		wxAUI_NB_TOP |
		wxAUI_NB_WINDOWLIST_BUTTON);

	mInfo = new Info(this);
	mExplorer = new Explorer(this);
//	mLibrary = new Library(this);
	mProperties = new Properties(this);
	mTools = new Tools(this);

	AddPage(mInfo, "Info");
	AddPage(mExplorer, "Explorer");
//	AddPage(mLibrary, "Library");
	AddPage(mProperties, "Properties");
	AddPage(mTools, "Tools");
}

PageType Notebook::getActivePage() const
{
	if (GetPage(GetSelection()) == mExplorer)
	{
		return PAGE_EXPLORER;
	}
	else if (GetPage(GetSelection()) == mLibrary)
	{
		return PAGE_LIBRARY;
	}
	else if (GetPage(GetSelection()) == mProperties)
	{
		return PAGE_PROPERTIES;
	}
	else if (GetPage(GetSelection()) == mTools)
	{
		return PAGE_TOOLS;
	}

	return PAGE_NONE;
}

void Notebook::setActivePage(PageType page)
{
	switch (page)
	{
		case PAGE_INFO: SetSelection(GetPageIndex(mInfo)); break;
		case PAGE_EXPLORER: SetSelection(GetPageIndex(mExplorer)); break;
		case PAGE_LIBRARY: SetSelection(GetPageIndex(mLibrary)); break;
		case PAGE_PROPERTIES: SetSelection(GetPageIndex(mProperties)); break;
		case PAGE_TOOLS: SetSelection(GetPageIndex(mTools)); break;
	}
}
