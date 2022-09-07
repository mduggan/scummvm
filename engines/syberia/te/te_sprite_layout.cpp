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

#include "syberia/te/te_sprite_layout.h"

namespace Syberia {

TeSpriteLayout::TeSpriteLayout() : _tiledSurfacePtr(new TeTiledSurface()), _sizeSet(false) {
}


int TeSpriteLayout::bufferSize() {
	return _tiledSurfacePtr->bufferSize();
}

void TeSpriteLayout::cont() {
	_tiledSurfacePtr->cont();
}

void TeSpriteLayout::draw() {
	error("TODO: Implement me.");
}

bool TeSpriteLayout::onParentWorldColorChanged() {
	Te3DObject2::onParentWorldColorChanged();
	setColor(color());
}

bool TeSpriteLayout::load(const Common::String &path) {
	if (path.empty()) {
		_tiledSurfacePtr.reset();
		return true;
	}
	
	stop();
	unload();
	
	if (_tiledSurfacePtr->load(path)) {
		const TeVector2s32 texSize = _tiledSurfacePtr->_tiledTexture->_totalSize;
		if (texSize._y <= 0) {
			setRatio(1.0);
		} else {
			setRatio(texSize._x / texSize._y);
		}
		if (sizeType() == CoordinatesType::ABSOLUTE && !_sizeSet) {
			setSize(TeVector3f32(texSize._x, texSize._y, 1.0));
		}
		updateMesh();
	}
	return true;
}

void TeSpriteLayout::play() {
	_tiledSurfacePtr->play();
}

void TeSpriteLayout::unload() {
	_tiledSurfacePtr->unload();
}

void TeSpriteLayout::pause() {
	_tiledSurfacePtr->pause();
}

void TeSpriteLayout::setBufferSize(long bufsize) {
	_tiledSurfacePtr->setBufferSize(bufsize);
}

void TeSpriteLayout::setColor(TeColor col) {
	Te3DObject2::setColor(col);
	col = color();
	_tiledSurfacePtr->setColor(col);
}

void TeSpriteLayout::setColorKey(const TeColor &col) {
	_tiledSurfacePtr->setColorKey(col);
}

void TeSpriteLayout::setColorKeyActivated(bool activated) {
	_tiledSurfacePtr->setColorKeyActivated(activated);
}

void TeSpriteLayout::setColorKeyTolerence(float val) {
	_tiledSurfacePtr->setColorKeyTolerence(val);
}

bool TeSpriteLayout::setName(const Common::String &newName) {
	TeLayout::setName(newName);
	_tiledSurfacePtr->setName(newName);
	return true;
}

void TeSpriteLayout::setSize(const TeVector3f32 &newSize) {
	TeLayout::setSize(newSize);
	_sizeSet = true;
}

void TeSpriteLayout::stop() {
	_tiledSurfacePtr->stop();
}

void TeSpriteLayout::updateMesh() {
	TeLayout::updateMesh();
	error("TODO: Implement me.");
}

void TeSpriteLayout::updateSize() {
	error("TODO: Implement me.");
}

} // end namespace Syberia
