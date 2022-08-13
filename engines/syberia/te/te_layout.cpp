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

#include "common/textconsole.h"
#include "syberia/te/te_layout.h"
#include "syberia/te/te_i_3d_object2.h"

namespace Syberia {

TeLayout::TeLayout() {
}

void TeLayout::addChild(TeI3DObject2 *child) {
	error("TODO: Implement me");
}

void TeLayout::addChildBefore(TeI3DObject2 *newchild, const TeI3DObject2 *current) {
	error("TODO: Implement me");
}

void TeLayout::removeChild(TeI3DObject2 *child) {
	error("TODO: Implement me");
}

const TeVector3f32 &TeLayout::anchor() {
	return _anchor;
}
void TeLayout::disableAutoZ() {
	_autoz = false;
}
void TeLayout::enableAutoZ() {
	_autoz = true;
}

bool TeLayout::isAutoZEnabled() {
	return _autoz;
}

void TeLayout::draw() {
	error("TODO: Implement me");
}

bool TeLayout::isMouseIn(const TeVector2s32 &mouseloc) {
	error("TODO: Implement me");
}

TeILayout::DrawMode TeLayout::mode() {
	return _drawMode;
}

bool TeLayout::onChildSizeChanged() {
	_childChanged = true;
	_childOrParentChanged = true;
	error("TODO: call some virtual function here");
	return false;
}

bool TeLayout::onParentSizeChanged() {
	_sizeChanged = true;
	_positionChanged = true;
	_somethingChanged = true;
	return false;
}

bool TeLayout::onParentWorldTransformationMatrixChanged() {
	_somethingChanged = true;
	return false;
}

TeVector3f32 TeLayout::position(){
	error("TODO: call some virtual functions here and calc position");
}

TeLayout::CoordinatesType TeLayout::positionType() const {
	return _positionType;
}

float TeLayout::ratio() const {
	return _ratio;
}

TeLayout::RatioMode TeLayout::ratioMode() const {
	return _ratioMode;
}

float TeLayout::safeAreaRatio() const {
	return _safeAreaRatio;
}

void TeLayout::setAnchor(const TeVector3f32 &anchor) {
	if (_anchor != anchor) {
		_anchor = anchor;
		_positionChanged = true;
		_somethingChanged = true;
		error("TODO: call a virtual function here");
	}
}

void TeLayout::setMode(DrawMode mode) {
	_drawMode = mode;
}

void TeLayout::setParent(TeI3DObject2 *parent) {
	error("TODO: implement me");
}
	
void TeLayout::setPosition(const TeVector3f32 &pos) {
	TeVector3f32 pos3d(pos.x(), pos.y(), _zPos);
	if (_userPosition != pos3d) {
		_userPosition.x() = pos.x();
		_userPosition.y() = pos.y();
		_positionChanged = true;
		_somethingChanged = true;
	}
}

void TeLayout::setPositionType(CoordinatesType newtype) {
	if (_positionType != newtype) {
		_positionType = newtype;
		_positionChanged = true;
		_somethingChanged = true;
	}
}

void TeLayout::setRatio(float val) {
	if (_ratio != val) {
	  _ratio = val;
	  _sizeChanged = true;
	  _somethingChanged = true;
	}
}

void TeLayout::setRatioMode(RatioMode mode) {
	if (_ratioMode != mode) {
		_ratioMode = mode;
		_sizeChanged = true;
		_somethingChanged = true;
   }
}

void TeLayout::setRotation(TeQuaternion *rot) {
	error("TODO: implement me");
}

void TeLayout::setSafeAreaRatio(float ratio) {
	if (_safeAreaRatio != ratio) {
		_safeAreaRatio = ratio;
		_sizeChanged = true;
		_somethingChanged = true;
	}
}

void TeLayout::setScale(const TeVector3f32 &scale) {
	error("TODO: call virtual setScale here");
	//_somethingChanged = true;
}

void TeLayout::setSize(const TeVector3f32 &size) {
	TeVector3f32 size3d(size.x(), size.y(), _zSize);
	if (_userSize != size3d) {
		_userSize.x() = size.x();
		_userSize.y() = size.y();
		_sizeChanged = true;
		_positionChanged = true;
		_somethingChanged = true;
	}

}

void TeLayout::setSizeType(CoordinatesType coordtype) {
	if (_sizeType != coordtype) {
		_sizeType = coordtype;
		_sizeChanged = true;
		_somethingChanged = true;
	}
}

void TeLayout::setZPosition(float zpos) {
	if (_zPos != zpos) {
	  _zPos = zpos;
	  _positionChanged = true;
	  _somethingChanged = true;
	}
}

TeVector3f32 TeLayout::size() {
	error("TODO: call some virtual functions here and calc size");
}

TeLayout::CoordinatesType TeLayout::sizeType() const {
	return _sizeType;
}

TeVector3f32 TeLayout::transformMousePosition(const TeVector2s32 &mousepos) {
	error("TODO: calc mouse pos");
	return TeVector3f32();
}

//void updateMesh(); // does nothing?
void TeLayout::updatePosition() {
	if (!_positionChanged)
		return;
	error("TODO: implement me");
}

void TeLayout::updateSize() {
	if (!_sizeChanged)
		return;
	error("TODO: implement me");
}

void TeLayout::updateWorldMatrix() {
	if (!_somethingChanged)
		return;
	error("TODO: implement me");
}

void TeLayout::updateZ() {
	if (!_childOrParentChanged || !_autoz)
		return;
	error("TODO: implement me");
}

void TeLayout::updateZSize() {
	if (!_childChanged)
		return;
	error("TODO: implement me");
}

TeVector3f32 TeLayout::userPosition() const {
	return _userPosition;
}

TeVector3f32 TeLayout::userSize() const {
	error("TODO: call virtual function here");
	return _userSize;
}

TeVector3f32 TeLayout::worldPosition() {
	error("TODO: Implement me");
}

TeMatrix4x4 TeLayout::worldTransformationMatrix() {
	error("TODO: Implement me");
}

bool TeLayout::worldVisible() {
	error("TODO: Implement me");
}

float TeLayout::xSize() {
	error("TODO: Implement me");
}

float TeLayout::ySize() {
	error("TODO: Implement me");
}

float TeLayout::zSize() {
	error("TODO: Implement me");
}

} // end namespace Syberia
