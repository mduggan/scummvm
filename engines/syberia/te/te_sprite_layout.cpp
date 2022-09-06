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

TeSpriteLayout::TeSpriteLayout() {
}

void TeSpriteLayout::cont() {
	error("TODO: Implement me.");
}

void TeSpriteLayout::draw() {
	error("TODO: Implement me.");
}

bool TeSpriteLayout::onParentWorldColorChanged() {
	error("TODO: Implement me.");
}

bool TeSpriteLayout::load(const Common::String &path) {
	if (path.empty()) {
		_tiledTexturePtr.reset();
		return true;
	}
	
	
	error("TODO: Implement me.");
}

void TeSpriteLayout::play() {
	error("TODO: Implement me.");
}

void TeSpriteLayout::unload() {
	error("TODO: Implement me.");
}

void TeSpriteLayout::pause() {
	error("TODO: Implement me.");
}

void TeSpriteLayout::setBufferSize(long bufsize) {
	error("TODO: Implement me.");
}

void TeSpriteLayout::setColor(TeColor col) {
	error("TODO: Implement me.");
}

void TeSpriteLayout::setColorKey(const TeColor &col) {
	error("TODO: Implement me.");
}

void TeSpriteLayout::setColorKeyActivated(bool activated) {
	error("TODO: Implement me.");
}

void TeSpriteLayout::setColorKeyTolerance(float val) {
	error("TODO: Implement me.");
}

bool TeSpriteLayout::setName(const Common::String &newName) {
	TeLayout::setName(newName);
	warning("TODO: TeSpriteLayout::setName setName of tiled surface.");
	//_tiledTexturePtr->setName(newName);
	return true;
}

void TeSpriteLayout::setSize(const TeVector3f32 &newSize) {
	TeLayout::setSize(newSize);
	_sizeSet = true;
}

void TeSpriteLayout::stop() {
	error("TODO: Implement me.");
}

void TeSpriteLayout::updateMesh() {
	TeLayout::updateMesh();
	error("TODO: Implement me.");
}

void TeSpriteLayout::updateSize() {
	error("TODO: Implement me.");
}

} // end namespace Syberia
