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


void Te3DObject2::setParent(TeI3DObject2 *newparent) {
	error("TODO: Implement me");
}

void Te3DObject2::setPosition(const TeVector3f32 &pos) {
	if (_position == pos)
		return;

	error("TODO: Implement me");
}

void Te3DObject2::setRotation(const TeQuaternion &rot) {
	if (_rotation == rot)
		return;

	error("TODO: Implement me");
}

void Te3DObject2::setScale(const TeVector3f32 &scale) {
	if (_scale == scale)
		return;

	error("TODO: Implement me");
}

void Te3DObject2::setSize(const TeVector3f32 &size) {
	if (_size == size)
		return;

	error("TODO: Implement me");
}

void Te3DObject2::setVisible(bool visible) {
	if (_visible == visible)
		return;

	error("TODO: Implement me");
}

void Te3DObject2::setZPosition(float zpos) {
	error("TODO: Implement me");
}

// TODO: Add more functions here.

} // end namespace Syberia
