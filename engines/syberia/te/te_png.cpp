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

#include "common/file.h"
#include "image/png.h"
#include "graphics/surface.h"

#include "syberia/te/te_png.h"

namespace Syberia {

TePng::TePng() : _loadedSurface(nullptr) {
}

TePng::~TePng() {
	if (_loadedSurface)
		delete _loadedSurface;
}

/*static*/
bool TePng::matchExtension(const Common::String &extn) {
	return extn == "png";
}

bool TePng::load(const Common::Path &path) {
	Common::File file;
	return file.open(path) && load(file);
}

bool TePng::load(Common::SeekableReadStream &stream) {
	Image::PNGDecoder png;
	if (!png.loadStream(stream))
		return false;
	
	if (png.getTransparentColor() == -1) {
		_loadedSurface = png.getSurface()->convertTo(Graphics::createPixelFormat<888>());
	} else {
		_loadedSurface = png.getSurface()->convertTo(Graphics::createPixelFormat<8888>());
	}
}

uint TePng::width() {
	if (_loadedSurface)
		return _loadedSurface->w;
	return 0;
}

uint TePng::height() {
	if (_loadedSurface)
		return _loadedSurface->h;
	return 0;
}

TeImage::Format TePng::imageFormat() {
	if (_loadedSurface) {
		if (_loadedSurface->format == Graphics::createPixelFormat<8888>())
			return TeImage::RGBA8;
		else if (_loadedSurface->format == Graphics::createPixelFormat<888>())
			return TeImage::RGB8;
	}
	return TeImage::INVALID;
}

bool TePng::update(unsigned long i, TeImage &imgout) {
	error("TODO: Implement TePng::update");
}

} // end namespace Syberia
