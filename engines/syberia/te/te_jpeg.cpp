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

#include "syberia/te/te_jpeg.h"

namespace Syberia {

TeJpeg::TeJpeg() {
}

/*static*/
bool TeJpeg::matchExtension(const Common::String &extn) {
	return extn == "jpg" || extn == "jpeg";
}

bool TeJpeg::load(const Common::Path &path) {
	error("TODO: Implement load");
}

bool TeJpeg::load(Common::SeekableReadStream &stream) {
	error("TODO: Implement load");
}

uint TeJpeg::width() {
	error("TODO: Implement width");
}

uint TeJpeg::height() {
	error("TODO: Implement height");
}

TeImage::Format TeJpeg::imageFormat() {
	error("TODO: Implement setLeftBorderSize");
}

void TeJpeg::setLeftBorderSize(uint val) {
	error("TODO: Implement setLeftBorderSize");
}

uint TeJpeg::leftBorderSize() {
	error("TODO: Implement leftBorderSize");
}

void TeJpeg::setRightBorderSize(uint val) {
	error("TODO: Implement setRightBorderSize");
}

uint TeJpeg::rightBorderSize() {
	error("TODO: Implement rightBorderSize");
}

void TeJpeg::setBottomBorderSize(uint val) {
	error("TODO: Implement setBottomBorderSize");
}

uint TeJpeg::bottomBorderSize() {
	error("TODO: Implement bottomBorderSize");
}

void TeJpeg::setTopBorderSize(uint val) {
	error("TODO: Implement setTopBorderSize");
}

uint TeJpeg::topBorderSize() {
	error("TODO: Implement topBorderSize");
}

bool TeJpeg::update(unsigned long i, TeImage &imgout) {
	error("TODO: Implement update");
}

} // end namespace Syberia
