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

// Since FreeType2 includes files, which contain forbidden symbols, we need to
// allow all symbols here.
#define FORBIDDEN_SYMBOL_ALLOW_ALL

#include "syberia/syberia.h"
#include "syberia/te/te_font3.h"
#include "syberia/te/te_core.h"
#include "graphics/fonts/freetype.h"

namespace Syberia {


static uint getUnicodeFromISO(uint chr)
{
	uint retval;
	
	TeCore *core = g_engine->getCore();
	bool isrus = (core->fileFlagSystemFlag("language") == "ru");
	
	if (!isrus) {
		uint uVar1 = 0x152;
		if (chr != 0xbc) {
			uVar1 = chr;
		}
		retval = 0x153;
		if (uVar1 != 0xbd) {
			retval = uVar1;
		}
	} else {
		retval = 0xad;
		if (chr != 0xad) {
			if (chr == 0xf0) {
				retval = 0x2116;
			}
			else if (chr == 0xfd) {
				retval = 0xa7;
			} else {
				retval = chr + 0x360;
				if (0x5e < chr - 0xa1) {
					retval = chr;
				}
			}
		}
	}
	return retval;
}

TeFont3::TeFont3() {
}


bool TeFont3::load(const Common::String &path) {
	// TODO: implement me.
	return false;
}

void TeFont3::unload() {
	if (_ftFace) {
		Graphics::FreeType::Done_Face(_ftFace);
	}
	// TODO: implement me.
}

void TeFont3::init() {
	// TODO: implement me.
}

float TeFont3::ascender(uint pxSize) {
	float size = 0.0;
	if (_ftFace) {
		FT_Error result = Graphics::FreeType::Set_Pixel_Sizes(_ftFace, pxSize, pxSize);
		if (result < 1) {
			size = _ftFace->ascender;
		} else {
			warning("TeFont3::glyph: FT_Set_Pixel_Sizes error %x\n", result);
		}
	}
	return size;
}

float TeFont3::descender(uint pxSize) {
	float size = 0.0;
	if (_ftFace) {
		FT_Error result = Graphics::FreeType::Set_Pixel_Sizes(_ftFace, pxSize, pxSize);
		if (result < 1) {
			size = _ftFace->descender;
		} else {
			warning("TeFont3::glyph: FT_Set_Pixel_Sizes error %x\n", result);
		}
	}
	return size;
}

float TeFont3::height(uint pxSize) {
	float size = 0.0;
	if (_ftFace) {
		FT_Error result = Graphics::FreeType::Set_Pixel_Sizes(_ftFace, pxSize, pxSize);
		if (result < 1) {
			size = _ftFace->height;
		} else {
			warning("TeFont3::glyph: FT_Set_Pixel_Sizes error %x\n", result);
		}
	}
	return size;
}

TeVector3f32 TeFont3::kerning(uint pxSize, uint isocode1, uint isocode2) {
	uint uni1 = getUnicodeFromISO(isocode1);
	uint uni2 = getUnicodeFromISO(isocode2);
	if (_ftFace) {
		FT_Error result = Graphics::FreeType::Set_Pixel_Sizes(_ftFace, pxSize, pxSize);
	  if ((int)result < 1) {
		  FT_Vector kern;
		result = Graphics::FreeType::Get_Kerning(_ftFace, uni1, uni2, 0, &kern);
		if ((int)result < 1) {
		  float xf = (float)(kern.x >> 6);
		  float yf = (float)(kern.y >> 6);
			return TeVector3f32(xf, yf, 0.0f);
		}
		warning("TeFont3::glyph: FT_Get_Kerning error %x\n", result);
	  } else {
		  warning("TeFont3::glyph: FT_Set_Pixel_Sizes error %x\n", result);
	  }
	}

	return TeVector3f32(0.0f, 0.0f, 0.0f);
}


} // end namespace Syberia
