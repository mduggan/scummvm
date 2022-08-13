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

#ifndef SYBERIA_TE_TE_FONT3_H
#define SYBERIA_TE_TE_FONT3_H

#include "common/str.h"
#include "common/types.h"

#include "syberia/te/te_resource.h"
#include "syberia/te/te_vector3f32.h"

struct FT_FaceRec_;

namespace Syberia {

class TeFont3 : public TeResource {
public:
	TeFont3();

	class FontSizeData {};
	class GlyphData {};

	bool load(const Common::String &path);
	void unload();
	void init();
	
	GlyphData glyph(uint size, uint charcode);

	float ascender(uint pxSize);
	float descender(uint pxSize);
	float height(uint pxSize);
	TeVector3f32 kerning(uint pxSize, uint isocode1, uint isocode2);

private:
	FT_FaceRec_ *_ftFace;

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_FONT3_H
