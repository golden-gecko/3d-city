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
#include "Graphics.h"

IMPLEMENT_SINGLETON(Graphics)

Graphics::Graphics()
{
	// Create root object.
	mRoot = new Ogre::Root("Plugins.cfg", "Ogre.cfg", "Ogre.log");

	if (mRoot->restoreConfig() == false)
	{
		mRoot->showConfigDialog();
	}

	mRoot->initialise(false);

	// Load resource configuration file.
	Ogre::ConfigFile cf;
	cf.load("Resources.cfg");

	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName;
	Ogre::ConfigFile::SettingsMultiMap* settings;
	Ogre::ConfigFile::SettingsMultiMap::iterator i;

	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		settings = seci.getNext();

		for (i = settings->begin(); i != settings->end(); i++)
		{
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				i->second, i->first, secName);
		}
	}

	// Load ZIP files with meshes.
	WIN32_FIND_DATA wfd;

	HANDLE hFind = FindFirstFile("..\\meshes\\*.zip", &wfd);

	if (hFind)
	{
		do
		{
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				std::string("..\\meshes\\") + wfd.cFileName, "Zip");
		}
		while (FindNextFile(hFind, &wfd));
	}

	Ogre::ResourceGroupManager::getSingleton().
		createResourceGroup("Cache");
}

void Graphics::createIntro()
{
	mManager = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, "___Intro");
	mCamera = mManager->createCamera("___Intro");
}
