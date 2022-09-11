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

#ifndef SYBERIA_TE_TE_VECTOR2F32_H
#define SYBERIA_TE_TE_VECTOR2F32_H

#include "math/vector2d.h"

namespace Syberia {

class TeVector2s32;
class TeVector3f32;

class TeVector2f32 : public Math::Vector2d {
public:
	TeVector2f32() : Math::Vector2d() {};

	TeVector2f32(const TeVector2s32 &other);

	TeVector2f32(float x_, float y_) : Math::Vector2d(x_, y_) {};

	/*
	TODO: do we need anything that isn't already in Vector2d here?
	TeVector2f32(const TeVector2f32 &other);
	TeVector2f32(const TeVector3f32 &other);
	TeVector2f32(float *vals);

	float crossProduct(const TeVector2f32 &other) const;
	float dotProduct(const TeVector2f32 &other) const;
	float length() const;
	float squaredLength() const;

	void normalize();
	
	TeVector2f32 operator+(const TeVector2f32 &other) const;
	TeVector2f32 operator-(const TeVector2f32 &other) const;
	bool operator==(const TeVector2f32 &other) const;

	bool deserialize(Common::ReadStream &stream);
	bool serialize(Common::WriteStream &stream);

public:
	float x;
	float y;
	*/
};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_VECTOR2F32_H
