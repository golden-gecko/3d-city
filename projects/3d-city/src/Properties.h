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

class MapView;
class ObjectView;
class Database;
class Map;
class Object;
class BuildingView;
class JunctionView;
class PlantView;
class RouteView;
class WaterObjectView;

class Properties : public wxPanel, public Ogre::Singleton<Properties>
{
public:
	Properties(wxWindow* parent);

	void refresh();
protected:
	wxPropertyGrid*	mGrid;
	wxBoxSizer*		mSizer;

	void show(Database* database);
	void show(Map* map);
	void show(MapView* mapView);

	void show(Object* object);
	void show(Building* object);
	void show(Junction* object);
	void show(Plant* object);
	void show(Route* object);
	void show(WaterObject* object);

	void show(ObjectView* objectView);
	void show(BuildingView* view);
	void show(JunctionView* view);
	void show(PlantView* view);
	void show(RouteView* view);
	void show(WaterObjectView* view);

	void onPropertyChanging(wxPropertyGridEvent& event);
	void onPropertyChanging(wxPropertyGridEvent& event, Database* database);
	void onPropertyChanging(wxPropertyGridEvent& event, Map* map);
	void onPropertyChanging(wxPropertyGridEvent& event, MapView* mapView);

	void onPropertyChanging(wxPropertyGridEvent& event, Object* object);
	void onPropertyChanging(wxPropertyGridEvent& event, Building* object);
	void onPropertyChanging(wxPropertyGridEvent& event, Junction* object);
	void onPropertyChanging(wxPropertyGridEvent& event, Plant* object);
	void onPropertyChanging(wxPropertyGridEvent& event, Route* object);
	void onPropertyChanging(wxPropertyGridEvent& event, WaterObject* object);

	void onPropertyChanging(wxPropertyGridEvent& event, ObjectView* objectView);
	void onPropertyChanging(wxPropertyGridEvent& event, BuildingView* view);
	void onPropertyChanging(wxPropertyGridEvent& event, JunctionView* view);
	void onPropertyChanging(wxPropertyGridEvent& event, PlantView* view);
	void onPropertyChanging(wxPropertyGridEvent& event, RouteView* view);
	void onPropertyChanging(wxPropertyGridEvent& event, WaterObjectView* view);
};
