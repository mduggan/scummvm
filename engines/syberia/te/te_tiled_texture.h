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

#ifndef SYBERIA_TE_TE_TILED_TEXTURE_H
#define SYBERIA_TE_TE_TILED_TEXTURE_H

#include "common/path.h"
#include "common/ptr.h"

#include "syberia/te/te_3d_texture.h"
#include "syberia/te/te_image.h"
#include "syberia/te/te_palette.h"
#include "syberia/te/te_resource.h"
#include "syberia/te/te_vector2s32.h"

namespace Syberia {

class TeTiledTexture : public TeResource {
public:
	TeTiledTexture();
	
	typedef struct {
		float x1;
		float x2;
		float y1;
		float y2;
	} Tile;
	
	uint imageFormat();
	bool isLoaded();
	bool load(const Common::Path &path);
	bool load(const TeImage &image);
	bool load(const Common::SharedPtr<Te3DTexture> &texture);
	long numberOfColumns();
	long numberOfRow();
	
	/*static*/ TeImage *optimizedTileImage(const Common::Array<TeImage> &images, const TeVector2s32 &size,
								  const Common::SharedPtr<TePalette> &pal, enum TeImage::Format format);

	void release();
	void save() {};
	const Tile *tile(TeVector2s32 &loc);
	void update(const TeImage &image);

	TeVector2s32 _totalSize;

private:
	Common::Array<Tile> _tileArray;
	TeVector2s32 _tileSize;
	TeVector2s32 _somethingSize;
	byte _somethingByte;

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_TILED_TEXTURE_H
