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
#include "Debug.h"
#include "Info.h"
#include "Log.h"
#include "Properties.h"
#include "BuildingView.h"
#include "JunctionView.h"
#include "PlantView.h"
#include "RouteView.h"
#include "WaterObjectView.h"

IMPLEMENT_SINGLETON(Properties)

Properties::Properties(wxWindow* parent) : wxPanel(parent)
{
	mGrid = new wxPropertyGrid(this, wxID_ANY, wxPoint(), wxSize(),
		wxPG_AUTO_SORT | wxPG_SPLITTER_AUTO_CENTER | wxPG_DEFAULT_STYLE);
	mGrid->Connect(wxEVT_PG_CHANGING,
		wxPropertyGridEventHandler(Properties::onPropertyChanging), NULL, this);

	mSizer = new wxBoxSizer(wxHORIZONTAL);
	mSizer->Add(mGrid, 1, wxEXPAND);

	SetSizer(mSizer);
}

void Properties::refresh()
{
	// Clear properties.
	mGrid->Clear();

	// Get selected item.
	const Identity& identity = Info::getSingleton().getSelected();

	// Show item properties.
	switch (identity.type)
	{
		case IT_DATABASE:
		{
			show(identity.getDatabase());

			break;
		}

		case IT_MAP:
		{
			show(identity.getMap());

			break;
		}

		case IT_OBJECT:
		{
			show(identity.getObject());

			break;
		}

		case IT_MAP_VIEW:
		{
			show(identity.getMapView());

			break;
		}

		case IT_OBJECT_VIEW:
		{
			show(identity.getObjectView());

			break;
		}
	}

	// Refresh property list.
	mGrid->Refresh();
}

void Properties::show(Database* database)
{
	if (database)
	{
		mGrid->Append(new wxUIntProperty(
			"ID", wxPG_LABEL, database->getID()));
	}
}

void Properties::show(Map* map)
{
	if (map)
	{
		mGrid->Append(new wxUIntProperty(
			"ID", wxPG_LABEL, map->getID()));
		mGrid->Append(new wxStringProperty(
			"Name", wxPG_LABEL, map->getName()));
		mGrid->Append(new wxLongStringProperty(
			"Description", wxPG_LABEL, map->getDescription()));

		mGrid->Append(new wxImageFileProperty(
			"Texture", wxPG_LABEL, map->getTexture()));
		mGrid->SetPropertyAttribute(
			"Texture", wxPG_FILE_WILDCARD, "All files (*.*)|*.*");
		mGrid->SetPropertyAttribute(
			"Texture", wxPG_FILE_SHOW_FULL_PATH, 0);
	}
}

void Properties::show(Object* object)
{
	if (object)
	{
		mGrid->Append(new wxUIntProperty(
			"ID", wxPG_LABEL, object->getID()));
		mGrid->Append(new wxUIntProperty(
			"Map ID", wxPG_LABEL, object->getMap()->getID()));
		mGrid->Append(new wxStringProperty(
			"Name", wxPG_LABEL, object->getName()));
		mGrid->Append(new wxLongStringProperty(
			"Description", wxPG_LABEL, object->getDescription()));

		switch (object->getType())
		{
			case OT_BUILDING: show(static_cast<Building*>(object)); break;
			case OT_JUNCTION: show(static_cast<Junction*>(object)); break;
			case OT_PLANT: show(static_cast<Plant*>(object)); break;
			case OT_STREET: show(static_cast<Street*>(object)); break;
			case OT_ROUTE: show(static_cast<Route*>(object)); break;
			case OT_WATER_OBJECT: show(static_cast<WaterObject*>(object)); break;
		}
	}
}

void Properties::show(Building* object)
{
	if (object)
	{
		mGrid->Append(new wxUIntProperty(
			"Number", wxPG_LABEL, object->getNumber()));
		mGrid->Append(new wxFloatProperty(
			"Height", wxPG_LABEL, object->getHeight()));
		mGrid->Append(new wxImageFileProperty(
			"Texture", wxPG_LABEL, object->getTexture()));
		mGrid->SetPropertyAttribute(
			"Texture", wxPG_FILE_WILDCARD, "All files (*.*)|*.*");
		mGrid->SetPropertyAttribute(
			"Texture", wxPG_FILE_SHOW_FULL_PATH, 0);
	}
}

void Properties::show(Junction* object)
{
	wxPGId parent = mGrid->Append(new wxParentProperty(
		"Routes", wxPG_LABEL, "<composed>"));

	RouteList::const_iterator i = object->getRoutes().begin();
	for (; i != object->getRoutes().end(); ++i)
	{
		mGrid->AppendIn(parent, new wxUIntProperty(
			"Route ID", wxPG_LABEL, i->first));
	}

	parent = mGrid->Append(new wxParentProperty(
		"Center", wxPG_LABEL, "<composed>"));

	mGrid->AppendIn(parent, new wxFloatProperty(
		"x", wxPG_LABEL, object->getCenter().x));
	mGrid->AppendIn(parent, new wxFloatProperty(
		"y", wxPG_LABEL, object->getCenter().y));
	mGrid->AppendIn(parent, new wxFloatProperty(
		"z", wxPG_LABEL, object->getCenter().z));
}

void Properties::show(Plant* object)
{
}

void Properties::show(Route* object)
{
	wxPGId parent = mGrid->Append(new wxParentProperty(
		"Junctions", wxPG_LABEL, "<composed>"));

	JunctionList::const_iterator i = object->getJunctions().begin();
	for (; i != object->getJunctions().end(); ++i)
	{
		mGrid->AppendIn(parent, new wxUIntProperty(
			"Junction ID", wxPG_LABEL, (*i)->getID()));
	}
}

void Properties::show(WaterObject* object)
{
}

void Properties::show(MapView* mapView)
{
	if (mapView)
	{
		mGrid->Append(new wxUIntProperty(
			"ID", wxPG_LABEL, mapView->getID()));
		mGrid->Append(new wxUIntProperty(
			"Map ID", wxPG_LABEL, mapView->getMap()->getID()));
		mGrid->Append(new wxFloatProperty(
			"Scale", wxPG_LABEL, mapView->getScale()));
	// TODO: Implement orthographic view.
	//	mGrid->Append(new wxBoolProperty(
	//		"Orthographic", wxPG_LABEL, mapView->getIsOrthographic()));
	}
}

void Properties::show(ObjectView* objectView)
{
	if (objectView)
	{
		mGrid->Append(new wxUIntProperty(
			"ID", wxPG_LABEL, objectView->getID()));
		mGrid->Append(new wxUIntProperty(
			"Object ID", wxPG_LABEL, objectView->getObject()->getID()));

		switch (objectView->getObject()->getType())
		{
			case OT_BUILDING: show(static_cast<RouteView*>(objectView)); break;
			case OT_JUNCTION: show(static_cast<JunctionView*>(objectView)); break;
			case OT_PLANT: show(static_cast<PlantView*>(objectView)); break;
			case OT_ROUTE: show(static_cast<RouteView*>(objectView)); break;
			case OT_WATER_OBJECT: show(static_cast<WaterObjectView*>(objectView)); break;
		}
	}
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event)
{
	if (Database* database = Info::getSingleton().getSelected().getDatabase())
	{
		onPropertyChanging(event, database);
	}
	else if (Map* map= Info::getSingleton().getSelected().getMap())
	{
		onPropertyChanging(event, map);
	}
	else if (Object* object = Info::getSingleton().getSelected().getObject())
	{
		onPropertyChanging(event, object);
	}
	else if (MapView* view = Info::getSingleton().getSelected().getMapView())
	{
		onPropertyChanging(event, view);
	}
	else if (ObjectView* view = Info::getSingleton().getSelected().getObjectView())
	{
		onPropertyChanging(event, view);
	}
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, Database* database)
{
	if (event.GetPropertyName() == "Name")
	{
		database->setName(event.GetValue().GetString().c_str());
	}
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, Map* map)
{
	if (event.GetPropertyName() == "Name")
	{
		map->setName(event.GetValue().GetString().c_str());
	}
	else if (event.GetPropertyName() == "Texture")
	{
		map->setTexture(wxFileNameFromPath(event.GetValue().GetString()).c_str());
	}

	City::getSingleton().updateViews(map);
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, MapView* mapView)
{
	if (event.GetPropertyName() == "Orthographic")
	{
		mapView->setIsOrthographic(event.GetValue().GetBool());
	}
	else if (event.GetPropertyName() == "Scale")
	{
		double scale = event.GetValue().GetDouble();

		if (scale < 0.1)
		{
			scale = 0.1;
		}
		else if (scale > 10.0)
		{
			scale = 10.0;
		}

		mapView->setScale(scale);
		mapView->update();
	}
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, Object* object)
{
	if (object)
	{
		if (event.GetPropertyName() == "Name")
		{
			object->setName(event.GetValue().GetString().c_str());
		}
		else if (event.GetPropertyName() == "Description")
		{
			object->setDescription(event.GetValue().GetString().c_str());
		}

		switch (object->getType())
		{
			case OT_BUILDING: onPropertyChanging(event, static_cast<Building*>(object)); break;
			case OT_JUNCTION: onPropertyChanging(event, static_cast<Junction*>(object)); break;
			case OT_PLANT: onPropertyChanging(event, static_cast<Plant*>(object)); break;
			case OT_ROUTE: onPropertyChanging(event, static_cast<Route*>(object)); break;
			case OT_WATER_OBJECT: onPropertyChanging(event, static_cast<WaterObject*>(object)); break;
		}

		City::getSingleton().updateViews(object);
	}
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, Building* object)
{
	if (event.GetPropertyName() == "Height")
	{
		object->setHeight(event.GetValue().GetDouble());
	}
	else if (event.GetPropertyName() == "Number")
	{
		object->setNumber(event.GetValue().GetLong());
	}
	else if (event.GetPropertyName() == "Texture")
	{
		object->setTexture(wxFileNameFromPath(event.GetValue().GetString()).c_str());
	}
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, Junction* object)
{
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, Plant* object)
{
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, Route* object)
{
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, WaterObject* object)
{
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, ObjectView* objectView)
{
	if (objectView)
	{
		switch (objectView->getObject()->getType())
		{
			case OT_BUILDING: onPropertyChanging(event, static_cast<BuildingView*>(objectView)); break;
			case OT_JUNCTION: onPropertyChanging(event, static_cast<JunctionView*>(objectView)); break;
			case OT_PLANT: onPropertyChanging(event, static_cast<PlantView*>(objectView)); break;
			case OT_ROUTE: onPropertyChanging(event, static_cast<RouteView*>(objectView)); break;
			case OT_WATER_OBJECT: onPropertyChanging(event, static_cast<WaterObjectView*>(objectView)); break;
		}

		objectView->update();
	}
}

void Properties::show(BuildingView* view)
{
}

void Properties::show(JunctionView* view)
{
}

void Properties::show(PlantView* view)
{
}

void Properties::show(RouteView* view)
{
	if (view)
	{
		mGrid->Append(new wxUIntProperty(
			"Segments", wxPG_LABEL, view->getSegments()));
	}
}

void Properties::show(WaterObjectView* view)
{
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, BuildingView* view)
{
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, JunctionView* view)
{
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, PlantView* view)
{
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, RouteView* view)
{
	if (event.GetPropertyName() == "Segments")
	{
		unsigned short value = (unsigned short)event.GetValue().GetLong();

		if (value < view->getRoute()->getPointCount())
		{
			value = view->getRoute()->getPointCount();
		}

		view->setSegments(value);
	}
}

void Properties::onPropertyChanging(wxPropertyGridEvent& event, WaterObjectView* view)
{
}
