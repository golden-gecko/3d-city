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
#include "ConnectDialog.h"
#include "Explorer.h"
#include "Log.h"

IMPLEMENT_SINGLETON(ConnectDialog)

ConnectDialog::ConnectDialog(wxWindow* parent) : wxDialog(parent, wxID_ANY, "Connect to database",
	wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	mName = new wxTextCtrl(this, wxID_ANY, "Demo Database");
	mType = new wxChoice(this, wxID_ANY);

	mParameters = new wxTextCtrl(this, wxID_ANY,
		"..\\data\\demo.bin",
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	mBrowse = new wxButton(this, wxID_ANY, "Browse");
	mFileDialog = new wxFileDialog(this);

	mBrowse->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(ConnectDialog::onBrowse), NULL, this);

	mOk = new wxButton(this, wxID_ANY, "OK");
	mCancel = new wxButton(this, wxID_ANY, "Cancel");

	mOk->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(ConnectDialog::onOk), NULL, this);
	mCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(ConnectDialog::onCancel), NULL, this);

	mParams = new wxBoxSizer(wxHORIZONTAL);
	mParams->Add(mParameters, 1, wxEXPAND | wxRIGHT, 5);
	mParams->Add(mBrowse);

	mTextBoxes = new wxFlexGridSizer(3, 2, 5, 10);
	mTextBoxes->Add(new wxStaticText(this, wxID_ANY, "Connection name"), 0, wxEXPAND | wxLEFT | wxTOP, 5);
	mTextBoxes->Add(mName, 1, wxEXPAND | wxRIGHT | wxTOP, 5);
	mTextBoxes->Add(new wxStaticText(this, wxID_ANY, "Connection type"), 0, wxEXPAND | wxLEFT, 5);
	mTextBoxes->Add(mType, 1, wxEXPAND | wxRIGHT, 5);
	mTextBoxes->Add(new wxStaticText(this, wxID_ANY, "Parameters"), 0, wxEXPAND | wxLEFT, 5);
	mTextBoxes->Add(mParams, 1, wxEXPAND | wxRIGHT, 5);
	mTextBoxes->AddGrowableCol(1, 1);
	mTextBoxes->AddGrowableRow(2, 1);

	mButtons = new wxBoxSizer(wxHORIZONTAL);
	mButtons ->Add(mOk, 0, wxRIGHT, 5);
	mButtons ->Add(mCancel);

	mMain = new wxBoxSizer(wxVERTICAL);
	mMain->Add(mTextBoxes, 1, wxEXPAND);
	mMain->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 5);
	mMain->Add(mButtons, 0, wxALIGN_RIGHT | wxALL, 5);

	SetSizer(mMain);
	SetMinSize(wxSize(400, 200));

	// Create plugin list.
	const PluginList& plugins = City::getSingleton().getPlugins();

	for (PluginList::const_iterator i = plugins.begin(); i != plugins.end(); ++i)
	{
		mType->Append(i->first, i->second);
	}

	mType->SetSelection(0);
}

void ConnectDialog::onBrowse(wxCommandEvent& event)
{
	if (mFileDialog->ShowModal() == wxID_OK)
	{
		mParameters->SetValue(mFileDialog->GetPath());
	}
}

void ConnectDialog::onOk(wxCommandEvent& event)
{
	try
	{
		std::string name = mName->GetValue();
		std::string type = mType->GetStringSelection();
		std::string parameters = mParameters->GetValue();

		if (name.size() == 0)
		{
			Log::getSingleton().error("Enter connection name.");
		}
		else if (Database* database = City::getSingleton().openDatabase(name, type, parameters))
		{
			if(database->getConnection()->isConnected() == false)
			{
				City::getSingleton().closeDatabase(database->getID());

				Log::getSingleton().error("Connection failed.");
			}
			else if (database->select() == false)
			{
				City::getSingleton().closeDatabase(database->getID());

				Log::getSingleton().info("Connection established.");
				Log::getSingleton().error("Error reading data.");
			}
			else
			{
				Log::getSingleton().info("Connection established.");

				Explorer::getSingleton().refresh();

				Close();
			}
		}
		else
		{
			Log::getSingleton().error("Error creating database.");
		}
	}
	catch (...)
	{
		Log::getSingleton().error("Unknown error.");
	}
}

void ConnectDialog::onCancel(wxCommandEvent& event)
{
	Close();
}
