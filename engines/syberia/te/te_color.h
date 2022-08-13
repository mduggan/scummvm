/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SYBERIA_TE_TE_COLOR_H
#define SYBERIA_TE_TE_COLOR_H

#include "common/types.h"
#include "common/stream.h"

namespace Syberia {

class TeColor {
public:
	TeColor();
	TeColor(uint32 rgba);
	TeColor(uint16 shortcol);
	TeColor(byte r, byte g, byte b, byte a);

	byte r() const { return _c[0]; };
	byte g() const { return _c[1]; };
	byte b() const { return _c[2]; };
	byte a() const { return _c[3]; };
	
	uint32 getPacked();
	
	bool serialize(Common::WriteStream &stream) const;
	bool deserialize(Common::ReadStream &stream);

private:
	byte _c[4];

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_COLOR_H
