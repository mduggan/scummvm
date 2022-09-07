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

#include "syberia/te/te_theora.h"

namespace Syberia {

TeTheora::TeTheora() {
}

/*static*/
bool TeTheora::matchExtension(const Common::String &extn) {
	return extn == "ogv";
}

bool TeTheora::load(const Common::Path &path) {
	error("TODO: Implement load");
}

bool TeTheora::load(Common::SeekableReadStream &stream) {
	error("TODO: Implement load");
}

uint TeTheora::width() {
	error("TODO: Implement width");
}

uint TeTheora::height() {
	error("TODO: Implement height");
}

int TeTheora::nbFrames() {
	error("TODO: Implement nbFrames");
}

TeImage::Format TeTheora::imageFormat() {
	error("TODO: Implement setLeftBorderSize");
}

void TeTheora::setLeftBorderSize(uint val) {
	error("TODO: Implement setLeftBorderSize");
}

uint TeTheora::leftBorderSize() {
	error("TODO: Implement leftBorderSize");
}

void TeTheora::setRightBorderSize(uint val) {
	error("TODO: Implement setRightBorderSize");
}

uint TeTheora::rightBorderSize() {
	error("TODO: Implement rightBorderSize");
}

void TeTheora::setBottomBorderSize(uint val) {
	error("TODO: Implement setBottomBorderSize");
}

uint TeTheora::bottomBorderSize() {
	error("TODO: Implement bottomBorderSize");
}

void TeTheora::setTopBorderSize(uint val) {
	error("TODO: Implement setTopBorderSize");
}

uint TeTheora::topBorderSize() {
	error("TODO: Implement topBorderSize");
}

float TeTheora::frameRate() {
	error("TODO: Implement frameRate");
}

bool TeTheora::update(unsigned long i, TeImage &imgout) {
	error("TODO: Implement update");
}

bool TeTheora::isAtEnd() {
	error("TODO: Implement isAtEnd");
}

void TeTheora::setColorKeyActivated(bool val) {
	error("TODO: Implement setColorKeyActivated");
}

void TeTheora::setColorKey(const TeColor &col) {
	error("TODO: Implement setColorKey");
}

void TeTheora::setColorKeyTolerence(float val) {
	error("TODO: Implement setColorKeyTolerence");
}


// TODO: Add more functions here.

} // end namespace Syberia
