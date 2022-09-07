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

#include "syberia/syberia.h"

#include "syberia/te/te_resource_manager.h"
#include "syberia/te/te_tiled_texture.h"
#include "syberia/te/te_tiled_surface.h"

namespace Syberia {

TeTiledTexture::TeTiledTexture() : _tileSize(0x800, 0x800) {
}

uint TeTiledTexture::imageFormat() {
	error("TODO: Implement TeTiledTexture::imageFormat");
	//if (!_tileArray.empty())
	//	return _tileArray[0].getFormat();
	return TeImage::INVALID;
}

bool TeTiledTexture::isLoaded() {
	error("TODO: Implement TeTiledTexture::isLoaded");
}


bool TeTiledTexture::load(const Common::Path &path) {
	error("TODO: Implement TeTiledTexture::load(path)");
}

bool TeTiledTexture::load(const TeImage &img) {
	error("TODO: Implement TeTiledTexture::load(img)");
}

bool TeTiledTexture::load(const Common::SharedPtr<Te3DTexture> &texture) {
	error("TODO: Implement TeTiledTexture::load(texture)");
}

long TeTiledTexture::numberOfColumns() {
	return ceilf((float)_totalSize._x / (float)_tileSize._x);
}

long TeTiledTexture::numberOfRow() {
	return ceilf((float)_totalSize._y / (float)_tileSize._y);
}

/*static*/ TeImage *optimizedTileImage(const Common::Array<TeImage> &images, const TeVector2s32 &size,
							  const Common::SharedPtr<TePalette> &pal, enum TeImage::Format format);

void TeTiledTexture::release() {
	_tileArray.clear();
	_totalSize = TeVector2s32();
	_somethingSize = TeVector2s32();
}

const TeTiledTexture::Tile *TeTiledTexture::tile(TeVector2s32 &loc) {
	error("TODO: Implement TeTiledTexture::tile");
}

void TeTiledTexture::update(const TeImage &image) {
	error("TODO: Implement TeTiledTexture::update");
}


// TODO: Add more functions here.

} // end namespace Syberia
