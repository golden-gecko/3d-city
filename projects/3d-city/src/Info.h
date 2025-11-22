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
#include "City.h"
#include "Identity.h"

class Info : public wxPanel, public Ogre::Singleton<Info>
{
public:
	Info(wxWindow* parent);

	void refresh();

	void setPoint(const Point& point)
	{
		Point p = point;

		if (abs(p.latitude) < 0.00000001) p.latitude = 0.0;
		if (abs(p.longitude) < 0.00000001) p.longitude = 0.0;
		if (abs(p.height) < 0.01) p.height = 0.0;

		mLatitude->SetLabel(Convert::ToString(p.latitude,
			0, ' ', 8, std::ios_base::fixed));
		mLongitude->SetLabel(Convert::ToString(p.longitude,
			0, ' ', 8, std::ios_base::fixed));
		mHeight->SetLabel(Convert::ToString(p.height,
			0, ' ', 2, std::ios_base::fixed));
	}
	void setPoint3D(const Point3D& point)
	{
		Point3D p = point;

		if (abs(p.x) < 0.00000001) p.x = 0.0;
		if (abs(p.y) < 0.01) p.y = 0.0;
		if (abs(p.z) < 0.00000001) p.z = 0.0;

		mX->SetLabel(Convert::ToString(
			p.x, 0, ' ', 8, std::ios_base::fixed));
		mY->SetLabel(Convert::ToString(
			p.y, 0, ' ', 2, std::ios_base::fixed));
		mZ->SetLabel(Convert::ToString(
			p.z, 0, ' ', 8, std::ios_base::fixed));
	}

	const Identity& getSelected() const
	{
		return mSelected;
	}
	void setSelected(const Identity& identity)
	{
		mSelected = identity;
	}
	void setFPS(float fps)
	{
		mFPS->SetLabel(Convert::ToString(fps, 0, 32, 2, std::ios::fixed));
	}
protected:
	wxStaticText*		mLatitude;
	wxStaticText*		mLongitude;
	wxStaticText*		mHeight;

	wxStaticText*		mY;
	wxStaticText*		mX;
	wxStaticText*		mZ;

	wxStaticText*		mFPS;

	wxFlexGridSizer*	mSizer;

	Identity			mSelected;
};
