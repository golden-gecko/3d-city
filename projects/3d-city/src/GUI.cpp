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
#include "ConnectDialog.h"
#include "Convert.h"
#include "GUI.h"
#include "OpenMapDialog.h"
#include "SearchDialog.h"

IMPLEMENT_SINGLETON(GUI)
IMPLEMENT_APP(GUI)

bool GUI::OnInit()
{
	wxImage::AddHandler(new wxPNGHandler());
	wxImage::AddHandler(new wxJPEGHandler());
	wxImage::AddHandler(new wxBMPHandler());

	// Load logo.
	wxBitmap bitmap(
		"..\\materials\\textures\\3d-city.bmp",
		wxBITMAP_TYPE_BMP);

	// Display splash screen.
	wxSplashScreen splash(
		bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_NO_TIMEOUT,
		0, NULL, -1, wxDefaultPosition, wxSize(400, 300),
		wxSIMPLE_BORDER);

	// Create global objects.
	mCity = new City();
	mGraphics = new Graphics();

	// Create main window.
	mMain = new Main();
	mMain->Center();
	mMain->Show();
	mMain->Maximize();

	// Init resources (must be called after render window creation).
	Graphics::getSingleton().initResources();

	return wxApp::OnInit();
}

int GUI::OnExit()
{
	delete mCity;
	delete mGraphics;

	return wxApp::OnExit();
}

int GUI::FilterEvent(wxEvent& event)
{
	if (event.GetEventType() == wxEVT_KEY_DOWN)
	{
		wxKeyEvent& keyEvent = static_cast<wxKeyEvent&>(event);

		switch (keyEvent.GetKeyCode())
		{
			case WXK_F3:
			{
				SearchDialog::getSingleton().Show(
					!(SearchDialog::getSingleton().IsShown()));

				return 0;
			}

			case 'F':
			{
				if (keyEvent.ControlDown())
				{
					SearchDialog::getSingleton().Show(
						!(SearchDialog::getSingleton().IsShown()));

					return 0;
				}
				return wxApp::FilterEvent(event);
			}
		}
	}
	return wxApp::FilterEvent(event);
}

void GUI::refresh()
{
	Explorer::getSingleton().refresh();
	Info::getSingleton().refresh();
	Properties::getSingleton().refresh();
	Tools::getSingleton().refresh();
}

/*
int main(int argc, char** argv)
{
	try
	{
		return wxEntry(argc, argv);
	}
	catch (Ogre::Exception& e)
	{
		MessageBox(0, e.getDescription().c_str(), "Ogre3D error", MB_ICONERROR | MB_OK);

		return 0;
	}
	catch (...)
	{
		MessageBox(0, "Unknown error.", "Unknown error", MB_ICONERROR | MB_OK);

		return 0;
	}
}
*/
