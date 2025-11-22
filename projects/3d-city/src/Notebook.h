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
#include "Explorer.h"
#include "Info.h"
#include "Library.h"
#include "Properties.h"
#include "Tools.h"

enum PageType
{
	PAGE_NONE,
	PAGE_INFO,
	PAGE_EXPLORER,
	PAGE_LIBRARY,
	PAGE_PROPERTIES,
	PAGE_TOOLS
};

class Notebook : public wxAuiNotebook, public Ogre::Singleton<Notebook>
{
public:
	Notebook(wxWindow* parent);

	PageType getActivePage() const;

	void setActivePage(PageType page);
protected:
	Info*		mInfo;
	Explorer*	mExplorer;
	Library*	mLibrary;
	Properties*	mProperties;
	Tools*		mTools;
};
