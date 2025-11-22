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

#include <sstream>
#include <string>

class Convert
{
public:
	template <typename T> static T Parse(const std::string& value)
	{
		std::stringstream stream(value);

		T out; stream >> out;

		return out;
	}

	template <typename T> static std::string ToString(T value,
		std::streamsize width = 0, char fill = ' ', std::streamsize precision = 0,
		std::ios_base::fmtflags flags = std::ios::fmtflags(0))
	{
		std::stringstream stream;
		
		stream.width(width);
		stream.fill(fill);
		stream.precision(precision);
		stream.setf(flags);

		stream << value;

		return stream.str();
	}
};
