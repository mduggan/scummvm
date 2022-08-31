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

#ifndef SYBERIA_TE_TE_VECTOR3F32_H
#define SYBERIA_TE_TE_VECTOR3F32_H

#include "math/vector2d.h"

namespace Syberia {

class TeVector3f32 : public Math::Vector3d {

public:
	TeVector3f32() { };
	TeVector3f32(float x_, float y_, float z_) {
		set(x_, y_, z_);
	}
	TeVector3f32(const Math::Vector3d &v) : Math::Vector3d(v) {}

	static void deserialize(Common::ReadStream *stream, TeVector3f32 *dest) {
		dest->x() = stream->readFloatLE();
		dest->y() = stream->readFloatLE();
		dest->z() = stream->readFloatLE();
	}

	static void serialize(Common::WriteStream *stream, const TeVector3f32 *src) {
		stream->writeFloatLE(src->x());
		stream->writeFloatLE(src->y());
		stream->writeFloatLE(src->z());
	}

	/*
	 TODO: do we need anything not already provided by Vector3d?
public:
	TeVector3f32();

public:
	float x;
	float y;
	float z;
	 */
};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_VECTOR3F32_H
