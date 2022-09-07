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

#include "common/file.h"
#include "common/path.h"
#include "graphics/surface.h"
#include "image/jpeg.h"

namespace Syberia {

TeJpeg::TeJpeg() : _loadedSurface(nullptr) {
}

TeJpeg::~TeJpeg() {
	if (_loadedSurface)
		delete _loadedSurface;
}

/*static*/
bool TeJpeg::matchExtension(const Common::String &extn) {
	return extn == "jpg" || extn == "jpeg";
}

bool TeJpeg::load(const Common::Path &path) {
	Common::File file;
	return file.open(path) && load(file);
}

bool TeJpeg::load(Common::SeekableReadStream &stream) {
	Image::JPEGDecoder jpg;

	if (_loadedSurface)
		delete _loadedSurface;
	_loadedSurface = nullptr;

	jpg.setOutputPixelFormat(Graphics::createPixelFormat<888>());
	if (!jpg.loadStream(stream))
		return false;
	
	_loadedSurface = jpg.getSurface()->convertTo(Graphics::createPixelFormat<888>());
	return true;
}

uint TeJpeg::width() {
	if (_loadedSurface)
		return _loadedSurface->w;
	return 0;
}

uint TeJpeg::height() {
	if (_loadedSurface)
		return _loadedSurface->h;
	return 0;
}

TeImage::Format TeJpeg::imageFormat() {
	return TeImage::RGB8;
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
	if (!_loadedSurface)
		return false;
	if (imgout.w == _loadedSurface->w && imgout.h == _loadedSurface->h && imgout.format == _loadedSurface->format) {
		imgout.copyFrom(*_loadedSurface);
		return true;
	}
	
	error("TODO: Implement TeJpeg::update for different sizes");
}

} // end namespace Syberia
