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

#include "syberia/te/te_png.h"

namespace Syberia {

TePng::TePng() {
}

/*static*/
bool TePng::matchExtension(const Common::String &extn) {
	return extn == "png";
}

bool TePng::load(const Common::Path &path) {
	error("TODO: Implement load");
}

bool TePng::load(Common::ReadStream &stream) {
	error("TODO: Implement load");
}

uint TePng::width() {
	error("TODO: Implement width");
}

uint TePng::height() {
	error("TODO: Implement height");
}

int TePng::nbFrames() {
	error("TODO: Implement nbFrames");
}

TeImage::Format TePng::imageFormat() {
	error("TODO: Implement setLeftBorderSize");
}

void TePng::setLeftBorderSize(uint val) {
	error("TODO: Implement setLeftBorderSize");
}

uint TePng::leftBorderSize() {
	error("TODO: Implement leftBorderSize");
}

void TePng::setRightBorderSize(uint val) {
	error("TODO: Implement setRightBorderSize");
}

uint TePng::rightBorderSize() {
	error("TODO: Implement rightBorderSize");
}

void TePng::setBottomBorderSize(uint val) {
	error("TODO: Implement setBottomBorderSize");
}

uint TePng::bottomBorderSize() {
	error("TODO: Implement bottomBorderSize");
}

void TePng::setTopBorderSize(uint val) {
	error("TODO: Implement setTopBorderSize");
}

uint TePng::topBorderSize() {
	error("TODO: Implement topBorderSize");
}

float TePng::frameRate() {
	error("TODO: Implement frameRate");
}

bool TePng::update(unsigned long i, TeImage &imgout) {
	error("TODO: Implement update");
}

bool TePng::isAtEnd() {
	error("TODO: Implement isAtEnd");
}

TeSignal0Param &TePng::onVideoFinished() {
	error("TODO: Implement onVideoFinished");
}

void TePng::setColorKeyActivated(bool val) {
	error("TODO: Implement setColorKeyActivated");
}

void TePng::setColorKey(const TeColor &col) {
	error("TODO: Implement setColorKey");
}

void TePng::setColorKeyTolerence(float val) {
	error("TODO: Implement setColorKeyTolerence");
}

} // end namespace Syberia
