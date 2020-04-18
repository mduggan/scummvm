/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "ultima/ultima8/misc/pent_include.h"

#include "ultima/ultima8/graphics/shape_frame.h"
#include "ultima/ultima8/graphics/raw_shape_frame.h"
#include "ultima/ultima8/graphics/palette.h"
#include "ultima/ultima8/graphics/palette_manager.h"

#include "graphics/managed_surface.h"
#include "image/png.h"

namespace Ultima {
namespace Ultima8 {

ShapeFrame::ShapeFrame(const RawShapeFrame *rawframe) :
		_xoff(rawframe->_xoff), _yoff(rawframe->_yoff),
		_width(rawframe->_width), _height(rawframe->_height) {

	_pixels = new uint8[_width * _height];
	_mask = new uint8[_width * _height];

	memset(_mask, 0, _width * _height);

	for (int y = 0; y < _height; y++) {
		int32 xpos = 0;
		const uint8 *linedata = rawframe->_rle_data + rawframe->_line_offsets[y];

		do {
			xpos += *linedata++;

			if (xpos >= _width)
				break;

			int32 dlen = *linedata++;
			int type = 0;

			if (rawframe->_compressed) {
				type = dlen & 1;
				dlen >>= 1;
			}

			for (int doff = 0; doff < dlen; doff++) {
				_pixels[y * _width + xpos + doff] = *linedata;
				_mask[y * _width + xpos + doff] = 1;
				if (!type) {
					linedata++;
				}
			}

			xpos += dlen;
			if (type) {
				linedata++;
			}

		} while (xpos < _width);

	}

}

ShapeFrame::~ShapeFrame() {
	delete [] _pixels;
	delete [] _mask;
}

// Checks to see if the frame has a pixel at the point
bool ShapeFrame::hasPoint(int32 x, int32 y) const {
	// Add the offset
	x += _xoff;
	y += _yoff;

	// First gross culling based on dims
	if (x < 0 || y < 0 || x >= _width || y >= _height)
		return false;

	return _mask[y * _width + x] != 0;
}

// Get the pixel at the point
uint8 ShapeFrame::getPixelAtPoint(int32 x, int32 y) const {
	// Add the offset
	x += _xoff;
	y += _yoff;

	// First gross culling based on dims
	if (x < 0 || y < 0 || x >= _width || y >= _height)
		return 0xFF;

	return _pixels[y * _width + x];
}

bool ShapeFrame::dumpFramePNG(Common::WriteStream &ws) const {
	if (_width == 0 || _height == 0)
		return false;

	Graphics::ManagedSurface surf(_width, _height, Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0));

	surf.setTransparentColor(0xff);

	uint32 *pix = static_cast<uint32 *>(surf.getPixels());
	uint8 *gamepal = PaletteManager::get_instance()->getPalette(PaletteManager::Pal_Game)->_palette;
	for (int y = 0; y < surf.h; y++) {
		for (int x = 0; x < surf.w; x++) {
			uint32 col = 0x00ff0000; /// Pure green.
			if (_mask[y * _width + x]) {
				uint8 rawcol = _pixels[y * _width + x];
				uint8 r = gamepal[rawcol * 3];
				uint8 g = gamepal[rawcol * 3 + 1];
				uint8 b = gamepal[rawcol * 3 + 2];
				col = r << 24 | g << 16 | b << 8 | 0xff;
			}
			pix[(surf.pitch / surf.format.bytesPerPixel) * y + x] = col;
		}
	}

	return Image::writePNG(ws, surf);
}


} // End of namespace Ultima8
} // End of namespace Ultima
