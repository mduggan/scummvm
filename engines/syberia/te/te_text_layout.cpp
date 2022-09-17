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

#include "syberia/te/te_text_layout.h"

namespace Syberia {

TeTextLayout::TeTextLayout(): _textSizeProportionalToWidth(0), _textSizeType(0) {
}

void TeTextLayout::setInterLine(float val) {
	error("TODO: call TeTextBase2::setInterLine(val)");
}

void TeTextLayout::setText(const Common::String &val) {
	error("TODO: Implement TeTextLayout::setText");
}

void TeTextLayout::setTextSizeType(int type) {
	if (_textSizeType != type) {
		_textSizeType = type;
		error("TODO: setTextSizeType in TeTextBase2");
	}
}

void TeTextLayout::setTextSizeProportionalToWidth(int val) {
	if (_textSizeProportionalToWidth != val) {
		_textSizeProportionalToWidth = val;
		error("TODO: setTextSizeProportionalToWidth in TeTextBase2");
	}
	
}

void TeTextLayout::setWrapMode(int mode) {
	error("TODO: Implement TeTextLayout::setWrapMode");
}


// TODO: Add more functions here.

} // end namespace Syberia
