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

#ifndef SYBERIA_GAME_BILLBOARD_H
#define SYBERIA_GAME_BILLBOARD_H

#include "common/str.h"

#include "syberia/te/te_vector3f32.h"
#include "syberia/te/te_vector2f32.h"

namespace Syberia {

class Billboard {
public:
	Billboard();

	bool load(const Common::String &path);

	void calcVertex();
	void position(const TeVector3f32 &pos);
	void position2(const TeVector3f32 &pos);
	void size(const TeVector2f32 &size);

private:
	TeVector3f32 _pos;
	TeVector3f32 _pos2;
	TeVector2f32 _size;
	bool _hasPos2;
};

} // end namespace Syberia

#endif // SYBERIA_GAME_BILLBOARD_H
