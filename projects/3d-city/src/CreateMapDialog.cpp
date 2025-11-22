//------------------------------------------------------------------------------
//
//  3d-city v0.1 alpha
//  Copyright (C) 2008/2009 Wojciech Holisz (wholisz@wp.pl)
//
//  Niniejszy program jest wolnym oprogramowaniem; mo¿esz go 
//  rozprowadzaæ dalej i/lub modyfikowaæ na warunkach Powszechnej
//  Licencji Publicznej GNU, wydanej przez Fundacjê Wolnego
//  Oprogramowania - wed³ug wersji 2-giej tej Licencji lub którejœ
//  z póŸniejszych wersji. 
//
//  Niniejszy program rozpowszechniany jest z nadziej¹, i¿ bêdzie on 
//  u¿yteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyœlnej 
//  gwarancji PRZYDATNOŒCI HANDLOWEJ albo PRZYDATNOŒCI DO OKREŒLONYCH 
//  ZASTOSOWAÑ. W celu uzyskania bli¿szych informacji - Powszechna 
//  Licencja Publiczna GNU. 
//
//  Z pewnoœci¹ wraz z niniejszym programem otrzyma³eœ te¿ egzemplarz 
//  Powszechnej Licencji Publicznej GNU (GNU General Public License);
//  jeœli nie - napisz do Free Software Foundation, Inc., 675 Mass Ave,
//  Cambridge, MA 02139, USA.
//
//------------------------------------------------------------------------------

#include "Dependencies.h"
#include "City.h"
#include "CreateMapDialog.h"
#include "Convert.h"
#include "Explorer.h"
#include "Log.h"
#include "Render.h"

IMPLEMENT_SINGLETON(CreateMapDialog)

CreateMapDialog::CreateMapDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, "Create new map",
	wxDefaultPosition, wxSize(400, 400), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	mDatabases = new wxChoice(this, wxID_ANY);
	mName = new wxTextCtrl(this, wxID_ANY, "New Map");
	mDescription = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

	mMapSize = new wxTextCtrl(this, wxID_ANY, "1000");

	mLatitude = new wxTextCtrl(this, wxID_ANY, "90.0000");
	mLongitude = new wxTextCtrl(this, wxID_ANY, "180.0000");
	mZoom = new wxChoice(this, wxID_ANY);

	for (int i = 1; i <= 20; ++i)
	{
		mZoom->Append(Convert::ToString(i));
	}

	mZoom->SetSelection(15);

	mFileName = new wxTextCtrl(this, wxID_ANY);
	mBrowse = new wxButton(this, wxID_ANY, "Browse");
	mFileDialog = new wxFileDialog(this);
	mBrowse->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(CreateMapDialog::onBrowse), NULL, this);

	//mImageOrRaw = new wxCheckBox(this, wxID_ANY, "Image");
	//mImageOrRaw->SetValue(true);
	//mIntOrFloat = new wxCheckBox(this, wxID_ANY, "Float");
	//mIntOrFloat->SetValue(false);
	//mTerrainSize = new wxTextCtrl(this, wxID_ANY);
	//mBytesPerValue = new wxTextCtrl(this, wxID_ANY);

	//mSizer[0] = new wxBoxSizer(wxHORIZONTAL);
	//mSizer[0]->Add(mTerrainSize, 2, wxEXPAND | wxRIGHT, 5);
	//mSizer[0]->Add(mImageOrRaw, 1, wxEXPAND);

	//mSizer[1] = new wxBoxSizer(wxHORIZONTAL);
	//mSizer[1]->Add(mBytesPerValue, 2, wxEXPAND | wxRIGHT, 5);
	//mSizer[1]->Add(mIntOrFloat, 1, wxEXPAND);

	mTerrainData = new wxBoxSizer(wxHORIZONTAL);
	mTerrainData->Add(mFileName, 1, wxEXPAND | wxRIGHT, 5);
	mTerrainData->Add(mBrowse, 0, wxEXPAND);

	mOk = new wxButton(this, wxID_ANY, "OK");
	mCancel = new wxButton(this, wxID_ANY, "Cancel");

	mOk->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(CreateMapDialog::onOk), NULL, this);
	mCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(CreateMapDialog::onCancel), NULL, this);

	mCenter = new wxBoxSizer(wxHORIZONTAL);
	mCenter->Add(mLatitude, 1, wxEXPAND | wxRIGHT, 5);
	mCenter->Add(mLongitude, 1, wxEXPAND);

	mTextBoxes = new wxFlexGridSizer(7, 2, 5, 10);
	mTextBoxes->Add(new wxStaticText(this, wxID_ANY, "Database"), 0, wxEXPAND);
	mTextBoxes->Add(mDatabases, 1, wxEXPAND);
	mTextBoxes->Add(new wxStaticText(this, wxID_ANY, "Map name"), 0, wxEXPAND);
	mTextBoxes->Add(mName, 1, wxEXPAND);
	mTextBoxes->Add(new wxStaticText(this, wxID_ANY, "Map description"), 0, wxEXPAND | wxBOTTOM, 5);
	mTextBoxes->Add(mDescription, 1, wxEXPAND | wxBOTTOM, 5);

	mTextBoxes->Add(new wxStaticText(this, wxID_ANY, "Map size"), 0, wxEXPAND | wxBOTTOM, 5);
	mTextBoxes->Add(mMapSize, 1, wxEXPAND | wxBOTTOM, 5);

	mTextBoxes->Add(new wxStaticText(this, wxID_ANY, "Center point"), 0, wxEXPAND);
	mTextBoxes->Add(mCenter, 1, wxEXPAND);
	mTextBoxes->Add(new wxStaticText(this, wxID_ANY, "Zoom"), 0, wxEXPAND | wxBOTTOM, 5);
	mTextBoxes->Add(mZoom, 1, wxEXPAND | wxBOTTOM, 5);

	//mTextBoxes->Add(new wxStaticText(this, wxID_ANY, "Data size"), 0, wxEXPAND);
	//mTextBoxes->Add(mSizer[0], 1, wxEXPAND);

	//mTextBoxes->Add(new wxStaticText(this, wxID_ANY, "Bytes Per Value"), 0, wxEXPAND);
	//mTextBoxes->Add(mSizer[1], 1, wxEXPAND);

	mTextBoxes->Add(new wxStaticText(this, wxID_ANY, "Terrain data"), 0, wxEXPAND);
	mTextBoxes->Add(mTerrainData, 1, wxEXPAND);

	mTextBoxes->AddGrowableCol(1, 1);
	mTextBoxes->AddGrowableRow(2, 1);

	mButtons = new wxBoxSizer(wxHORIZONTAL);
	mButtons->Add(mOk, 0, wxRIGHT, 5);
	mButtons->Add(mCancel);

	mMain = new wxBoxSizer(wxVERTICAL);
	mMain->Add(mTextBoxes, 1, wxEXPAND | wxALL, 5);
	mMain->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT, 5);
	mMain->Add(mButtons, 0, wxALIGN_RIGHT | wxALL, 5);

	SetSizer(mMain);
}

int CreateMapDialog::ShowModal()
{
	mDatabases->Clear();

	const DatabaseList& databases = City::getSingleton().getDatabases();

	for (DatabaseList::const_iterator i = databases.begin(); i != databases.end(); ++i)
	{
		mDatabases->Append(i->second->getName(), i->second);
	}

	mDatabases->SetSelection(0);

	return wxDialog::ShowModal();
}

void CreateMapDialog::onBrowse(wxCommandEvent& event)
{
	if (mFileDialog->ShowModal() == wxID_OK)
	{
		mFileName->SetValue(mFileDialog->GetPath());
	}
}

void CreateMapDialog::onOk(wxCommandEvent& event)
{
	try
	{
		if (mDatabases->GetSelection() == wxNOT_FOUND)
		{
			Log::getSingleton().error("No database. Select existing or connect to new dabase.");
			
			return;
		}

		Database* database = static_cast<Database*>
			(mDatabases->GetClientData(mDatabases->GetSelection()));

		std::string name = mName->GetValue();
		std::string description = mDescription->GetValue();
		long size = Convert::Parse<long>(mMapSize->GetValue().c_str());
		double latitude = Convert::Parse<double>(mLatitude->GetValue().c_str());
		double longitude = Convert::Parse<double>(mLongitude->GetValue().c_str());
		double zoom = Convert::Parse<double>(mZoom->GetStringSelection().c_str());
		std::string fileName = mFileName->GetValue();

		if (name.size() == 0)
		{
			Log::getSingleton().error("Enter map name.");
		}
		else if (size < 10 && size > 20000)
		{
			Log::getSingleton().error("Enter correct size (10 < size < 20000).");
		}
		else if (latitude < 0.0 || latitude > 180.0 || longitude < 0.0 || longitude > 360.0)
		{
			Log::getSingleton().error("Enter correct latitude (0.0 < latitude < 180.0) and longitude (0.0 < longitude 360.0).");
		}
		else
		{
			// Create map in selected database.
			if (Map* map = database->createMap())
			{
				// Set map attributes.
				map->setName(name);
				map->setDescription(description);
				map->setSize(Size(size, size));
				map->setCenter(Point(latitude, longitude));
				map->setZoom(zoom);

				// Read terrain from image.
				//if (mImageOrRaw->GetValue())
				{
					// Load image.
					wxImage image;

					if (image.LoadFile(fileName))
					{
						// Copy image to structur.
						TerrainData data(image.GetWidth() * image.GetWidth());

						for (int i = 0; i < image.GetWidth() * image.GetWidth(); ++i)
						{
							data[i] = image.GetData()[i * 3];
						}

						// Attach terrain to map.
						map->setTerrainSize(TerrainSize(
							(unsigned int)image.GetWidth(),
							(unsigned int)image.GetWidth()));
						map->setTerrainData(data);
					}
					else
					{
						map->setTerrainSize(TerrainSize(65, 65));
						map->setTerrainData(TerrainData(65 * 65));
					}
				}
				// Read terrain data from a file.
				//else
				//{
				//	std::ifstream file(
				//		fileName.c_str(),
				//		std::ios_base::binary | std::ios_base::in);

				//	if (file.is_open())
				//	{
				//		file.close();
				//	}
				//	else
				//	{
				//		Log::getSingleton().error("Cannot open file. Craeting default flat terrain.");

				//		map->setTerrainSize(TerrainSize(65, 65));
				//		map->setTerrainData(TerrainData(65 * 65));
				//	}
				//}

				// Create and select map view.
				if (MapView* mapView = City::getSingleton().createView(map))
				{
					Explorer::getSingleton().refresh();
					Render::getSingleton().select(mapView);

					Close();
				}
				else
				{
					database->destroyMap(map->getID());

					Log::getSingleton().error("Error opening map.");
				}
			}
			else
			{
				Log::getSingleton().error("Error creating map.");
			}
		}
	}
	catch (Ogre::Exception& e)
	{
		Log::getSingleton().error("Ogre3D error: " + e.getDescription());
	}
	catch (...)
	{
		Log::getSingleton().error("Unknown error");
	}
}

void CreateMapDialog::onCancel(wxCommandEvent& event)
{
	Close();
}
