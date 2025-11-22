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

class Explorer : public wxPanel, public Ogre::Singleton<Explorer>
{
public:
	Explorer(wxWindow* parent);

	void disconnectEvents()
	{
		mLocal->Disconnect(wxEVT_COMMAND_TREE_SEL_CHANGED,
			wxTreeEventHandler(Explorer::onLocalItemSelected), NULL, this);
		mLocal->Disconnect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,
			wxTreeEventHandler(Explorer::onItemRightClick), NULL, this);
		mView->Disconnect(wxEVT_COMMAND_TREE_SEL_CHANGED,
			wxTreeEventHandler(Explorer::onViewItemSelected), NULL, this);
		mView->Disconnect(wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,
			wxTreeEventHandler(Explorer::onItemRightClick), NULL, this);
	}
	void deleteItems()
	{
		mLocal->DeleteAllItems();
		mView->DeleteAllItems();
	}

	void disconnect();

	void openMaps();
	void closeMaps();
	void deleteMaps();
	void deleteObjects();

	void refreshLocal();
	void refreshView();

	void refresh();
protected:
	wxTreeCtrl*	mLocal;
	wxTreeCtrl*	mView;

	wxButton*	mRefreshLocal;
	wxButton*	mRefreshView;

	wxBoxSizer*	mSizer;
	wxMenu*		mMenu;

	void onLocalItemSelected(wxTreeEvent& event);
	void onViewItemSelected(wxTreeEvent& event);

	void onRefreshLocalClick(wxTreeEvent& event);
	void onRefreshViewClick(wxTreeEvent& event);

	void onItemRightClick(wxTreeEvent& event);
};
