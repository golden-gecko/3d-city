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

#include <iostream>
#include <sstream>
#include <string>

#define DEBUG			1
#define DEBUG_ASSERT	1
#define DEBUG_COUT		1
#define DEBUG_FILE		1
#define DEBUG_FUNCTION	1

class Debug
{
public:
	static void Assert(bool expression, const std::string& message, const std::string& file, int line)
	{
		if (expression == false)
		{
			std::stringstream stream; stream
				<< "Assertion\n\n"
				<< message
				<< "\n\nfailed in file "
				<< file.substr(file.find_last_of("\\") + 1)
				<< " at "
				<< line
				<< " line.\n\nClose application?";

			std::cout << stream.str() << std::endl;

			int choice = MessageBox(NULL,
				stream.str().c_str(),
				"Assertion failed",
				MB_ICONERROR | MB_YESNO);

			if (choice == IDYES)
			{
				exit(1);
			}
		}
	}
};

template <typename T1>
std::string arguments(T1 t1)
{
	std::stringstream stream;
	stream << t1;

	return stream.str();
}

template <typename T1, typename T2>
std::string arguments(T1 t1, T2 t2)
{
	std::stringstream stream;
	stream << t1 << ", " << t2;

	return stream.str();
}

template <typename T1, typename T2, typename T3>
std::string arguments(T1 t1, T2 t2, T3 t3)
{
	std::stringstream stream;
	stream << t1 << ", " << t2 << ", " << t3;

	return stream.str();
}

#if DEBUG == 1 && DEBUG_ASSERT == 1
#	define ASSERT(e) Debug::Assert(e ? true : false, #e, __FILE__, __LINE__);
#else
#	define ASSERT(e)
#endif

#if DEBUG == 1 && DEBUG_FUNCTION == 1
#	define LOG(...) cout << __FUNCTION__ << "(" << arguments(__VA_ARGS__) << ")" << endl;
#else
#	define LOG
#endif
