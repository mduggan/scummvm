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

#include "syberia/te/te_color.h"

namespace Syberia {

TeColor::TeColor() : _c{0, 0, 0, 0xff} {
}

TeColor::TeColor(uint32 rgba) {
	_c[0] = (byte)(rgba >> 24);
	_c[1] = (byte)(rgba >> 16);
	_c[2] = (byte)(rgba >> 8);
	_c[3] = (byte)rgba;
}

TeColor::TeColor(uint16 shortcol) {
	_c[0] = (byte)(shortcol >> 7) & 0xf8;
	_c[1] = (byte)(shortcol >> 2) & 0xf8;
	_c[2] = (byte)shortcol << 3;
	_c[3] = 0;
}

TeColor::TeColor(byte r, byte g, byte b, byte a) : _c{r, g, b, a} {
}

uint32 TeColor::getPacked() {
	return (g() & 0xf8) << 2 | (r() & 0xf8) << 7 | (b() >> 3);
}

bool TeColor::serialize(Common::WriteStream &stream) const {
	uint32 result = stream.write(_c, 4);
	return result == 4;
}
bool TeColor::deserialize(Common::ReadStream &stream) {
	uint32 result = stream.read(_c, 4);
	return result == 4;
}

} // end namespace Syberia
