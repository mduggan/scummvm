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

#include "syberia/te/te_tga.h"

namespace Syberia {

TeTga::TeTga() {
}

/*static*/
bool TeTga::matchExtension(const Common::String &extn) {
	return extn == "png";
}

bool TeTga::load(const Common::Path &path) {
	error("TODO: Implement load");
}

bool TeTga::load(Common::SeekableReadStream &stream) {
	error("TODO: Implement load");
}

uint TeTga::width() {
	error("TODO: Implement width");
}

uint TeTga::height() {
	error("TODO: Implement height");
}

TeImage::Format TeTga::imageFormat() {
	error("TODO: Implement setLeftBorderSize");
}

void TeTga::setLeftBorderSize(uint val) {
	error("TODO: Implement setLeftBorderSize");
}

uint TeTga::leftBorderSize() {
	error("TODO: Implement leftBorderSize");
}

void TeTga::setRightBorderSize(uint val) {
	error("TODO: Implement setRightBorderSize");
}

uint TeTga::rightBorderSize() {
	error("TODO: Implement rightBorderSize");
}

void TeTga::setBottomBorderSize(uint val) {
	error("TODO: Implement setBottomBorderSize");
}

uint TeTga::bottomBorderSize() {
	error("TODO: Implement bottomBorderSize");
}

void TeTga::setTopBorderSize(uint val) {
	error("TODO: Implement setTopBorderSize");
}

uint TeTga::topBorderSize() {
	error("TODO: Implement topBorderSize");
}

bool TeTga::update(unsigned long i, TeImage &imgout) {
	error("TODO: Implement update");
}

} // end namespace Syberia
