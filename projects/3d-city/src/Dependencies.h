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

// Ogre3D
#include <Ogre.h>

// wxWidgets
#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibook.h>
#include <wx/notebook.h>
#include <wx/process.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/splash.h>
#include <wx/statline.h>
#include <wx/statusbr.h>
#include <wx/tglbtn.h>
#include <wx/treectrl.h>

// wxPropertyGrid
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

// Ogre::Singleton
#define IMPLEMENT_SINGLETON(class_name) class_name* Ogre::Singleton<class_name>::ms_Singleton = NULL;
