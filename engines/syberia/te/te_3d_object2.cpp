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

#include "syberia/te/te_3d_object2.h"

namespace Syberia {

Te3DObject2::Te3DObject2() : _childListChanged(false), _parent(nullptr), _colorInheritance(true) {
}

void Te3DObject2::addChild(Te3DObject2 *child) {
	_children.push_back(child);
	error("TODO: call some virtual function then childListChangedSignal here");
}

void Te3DObject2::addChildBefore(Te3DObject2 *newChild, Te3DObject2 *ref) {
	error("TODO: Implement me");
}

Te3DObject2 *Te3DObject2::child(long offset) {
	return _children[offset];
}

long Te3DObject2::childIndex(Te3DObject2 *child) {
	for (uint i = 0; i < _children.size(); i++) {
		if (_children[i] == child)
			return i;
	}
	return -1;
}

/*static*/ void Te3DObject2::deserialize(Common::ReadStream *stream, Te3DObject2 *dest) {
	error("TODO: Implement me");
}

/*static*/ void Te3DObject2::serialize(Common::ReadStream *stream, Te3DObject2 *src) {
	error("TODO: Implement me");
}

bool Te3DObject2::onParentWorldColorChanged() {
	error("TODO: Call onParentWorldColorChangedSignal");
	return false;
}

bool Te3DObject2::onParentWorldTransformationMatrixChanged() {
	error("TODO: onParentWorldTransformationMatrixChangedSignal");
	return false;
}

long Te3DObject2::onPositionChanged() {
	error("TODO: Implement me.");
}

long Te3DObject2::onSizeChanged() {
	error("TODO: Implement me.");
}

long Te3DObject2::onWorldColorChanged() {
	error("TODO: Implement me.");
}

long Te3DObject2::onWorldTransformationMatrixChanged() {
	error("TODO: Implement me.");
}

long Te3DObject2::onWorldVisibleChanged() {
	error("TODO: Implement me.");
}

void Te3DObject2::removeChild(Te3DObject2 *child) {
	uint i;
	for (i = 0; i < _children.size(); i++) {
		if (_children[i] == child) {
			break;
		}
	}
	if (i < _children.size()) {
		error("TODO: Call onChildListChangedSignal");
	}
}

bool Te3DObject2::onWorldVisibleChangedSlot() {
	error("TODO: Call onWorldVisibleChangedSignal");
	return false;
}

void Te3DObject2::removeChildren() {
	for (uint i = 0; i < _children.size(); i++) {
		_children[i]->setParent(nullptr);
	}
	_children.clear();
	error("TODO: Call childListChangedSignal");
}

void Te3DObject2::rotate(const TeQuaternion &rot) {
	TeQuaternion newRot = rotation();
	newRot *= rot;
	setRotation(newRot);
}

void Te3DObject2::setColor(const TeColor &col) {
	_color = col;
	error("TODO: Call onParentWorldColorChangedSignal");
}

void Te3DObject2::setParent(TeI3DObject2 *newparent) {
	error("TODO: Implement me");
}

void Te3DObject2::setPosition(const TeVector3f32 &pos) {
	if (_position == pos)
		return;
	
	_position = pos;
	error("TODO: Call onPositionChangedSignal and onParentWorldTransformationMatrixChangedSignal");
}

void Te3DObject2::setRotation(const TeQuaternion &rot) {
	if (_rotation == rot)
		return;

	_rotation = rot;
	error("TODO: Call onParentWorldTransformationMatrixChangedSignal");
}

void Te3DObject2::setScale(const TeVector3f32 &scale) {
	if (_scale == scale)
		return;

	_scale = scale;
	error("TODO: Call onParentWorldTransformationMatrixChangedSignal");
}

void Te3DObject2::setSize(const TeVector3f32 &size) {
	if (_size == size)
		return;

	_size = size;
	error("TODO: Call onSizeChangedSignal");
}

void Te3DObject2::setVisible(bool visible) {
	if (_visible == visible)
		return;

	_visible = visible;
	onWorldVisibleChangedSlot();
}

void Te3DObject2::setZPosition(float zpos) {
	TeVector3f32 pos = position();
	pos.z() = zpos;
	setPosition(pos);
}

TeMatrix4x4 Te3DObject2::transformationMatrix() const {
	TeMatrix4x4 retval;
	retval.translate(position());
	retval = retval * rotation().toMatrix();
	retval.scale(scale());
	return retval;
}

void Te3DObject2::translate(TeVector3f32 *vec) {
	
}

} // end namespace Syberia
