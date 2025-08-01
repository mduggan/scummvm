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

#ifndef AGS_SHARED_GFX_IMAGE_H
#define AGS_SHARED_GFX_IMAGE_H

#include "ags/lib/allegro.h"

namespace AGS3 {

struct PACKFILE;

extern BITMAP *load_bitmap(PACKFILE *pf, color *pal);
extern BITMAP *load_bitmap(const char *filename, color *pal);
extern BITMAP *load_bmp(const char *filename, color *pal);
extern BITMAP *load_lbm(const char *filename, color *pal);
extern BITMAP *load_pcx(const char *filename, color *pal);
extern BITMAP *load_tga(const char *filename, color *pal);

extern bool save_bitmap(Common::WriteStream &out, BITMAP *bmp, const RGB *pal);

} // namespace AGS3

#endif
