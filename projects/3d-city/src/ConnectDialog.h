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

#pragma once

#include "Dependencies.h"

class ConnectDialog : public wxDialog, public Ogre::Singleton<ConnectDialog>
{
public:
	ConnectDialog(wxWindow* parent);

	virtual ~ConnectDialog()
	{
		// Is this correct?
		delete mFileDialog;
	}
protected:
	wxTextCtrl*			mName;
	wxChoice*			mType;
	wxTextCtrl*			mParameters;
	wxButton*			mBrowse;
	wxFileDialog*		mFileDialog;

	wxButton*			mOk;
	wxButton*			mCancel;

	wxFlexGridSizer*	mTextBoxes;
	wxSizer*			mParams;
	wxSizer*			mButtons;
	wxSizer*			mMain;

	void onBrowse(wxCommandEvent& event);
	void onOk(wxCommandEvent& event);
	void onCancel(wxCommandEvent& event);
};
