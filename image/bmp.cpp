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

#include "image/bmp.h"

#include "common/stream.h"
#include "common/substream.h"
#include "common/textconsole.h"
#include "graphics/pixelformat.h"
#include "graphics/surface.h"
#include "image/codecs/codec.h"

// NOTE: This decoder understands only so called BMP Win3.x old format
// In order to produce files suitable for it, use ImageMagick:
//
// convert input.file BMP3:output.bmp
//

namespace Image {

BitmapDecoder::BitmapDecoder(): _codec(nullptr), _surface(nullptr), _palette(0) {
}

BitmapDecoder::~BitmapDecoder() {
	destroy();
}

void BitmapDecoder::destroy() {
	delete _codec;
	_codec = nullptr;

	_surface = nullptr;
	_palette.clear();
}

bool BitmapDecoder::loadStream(Common::SeekableReadStream &stream) {
	destroy();

	uint16 fileType = stream.readUint16BE();
	uint32 imageOffset = 0;

	if (fileType == MKTAG16('B', 'M')) {
		// The bitmap file header is present
		/* uint32 fileSize = */ stream.readUint32LE();
		/* uint16 res1 = */ stream.readUint16LE();
		/* uint16 res2 = */ stream.readUint16LE();
		imageOffset = stream.readUint32LE();
	} else {
		// Not present, let's try to parse as a headerless one
		stream.seek(-2, SEEK_CUR);
	}

	uint32 infoSize = stream.readUint32LE();
	if (infoSize != 40 && infoSize != 52 && infoSize != 56 && infoSize != 108 && infoSize != 124) {
		warning("Only Windows v1-v5 bitmaps are supported, unknown header: %d", infoSize);
		return false;
	}

	uint32 width = stream.readUint32LE();
	int32 height = stream.readSint32LE();

	if (width == 0 || height == 0)
		return false;

	if (height < 0) {
		warning("Right-side up bitmaps not supported");
		return false;
	}

	/* uint16 planes = */ stream.readUint16LE();
	uint16 bitsPerPixel = stream.readUint16LE();

	if (bitsPerPixel != 4 && bitsPerPixel != 8 && bitsPerPixel != 16 && bitsPerPixel != 24 && bitsPerPixel != 32) {
		warning("%dbpp bitmaps not supported", bitsPerPixel);
		return false;
	}

	uint32 compression = stream.readUint32BE();

	if (bitsPerPixel == 16 && compression != SWAP_CONSTANT_32(0)) {
		warning("only RGB555 raw mode supported for %dbpp bitmaps", bitsPerPixel);
		return false;
	}

	uint32 imageSize = stream.readUint32LE();
	/* uint32 pixelsPerMeterX = */ stream.readUint32LE();
	/* uint32 pixelsPerMeterY = */ stream.readUint32LE();
	uint32 paletteColorCount = stream.readUint32LE();
	/* uint32 colorsImportant = */ stream.readUint32LE();

	stream.seek(infoSize - 40, SEEK_CUR);

	if (bitsPerPixel == 4 || bitsPerPixel == 8) {
		if (paletteColorCount == 0)
			paletteColorCount = bitsPerPixel == 8 ? 256 : 16;

		// Read the palette
		_palette.resize(paletteColorCount, false);
		for (uint16 i = 0; i < paletteColorCount; i++) {
			byte b = stream.readByte();
			byte g = stream.readByte();
			byte r = stream.readByte();
			stream.readByte();

			_palette.set(i, r, g, b);
		}
	}

	// Create the codec (it will warn about unhandled compression)
	_codec = createBitmapCodec(compression, 0, width, height, bitsPerPixel);
	if (!_codec)
		return false;

	// If the image offset is zero (like in headerless ones), set it to the current
	// position.
	if (imageOffset == 0)
		imageOffset = stream.pos();

	// If the image size is zero, set it to the rest of the stream.
	if (imageSize == 0)
		imageSize = stream.size() - imageOffset;

	// Grab the frame data
	Common::SeekableSubReadStream subStream(&stream, imageOffset, imageOffset + imageSize);

	// We only support raw bitmaps for now
	_surface = _codec->decodeFrame(subStream);

	return true;
}

bool writeBMP(Common::WriteStream &out, const Graphics::Surface &input, const byte *palette) {
	const Graphics::PixelFormat requiredFormat_3byte = Graphics::PixelFormat::createFormatBGR24();

	Graphics::Surface *tmp = NULL;
	const Graphics::Surface *surface;

	if (input.format == requiredFormat_3byte) {
		surface = &input;
	} else {
		surface = tmp = input.convertTo(requiredFormat_3byte, palette);
	}

	int dstPitch = surface->w * 3;
	int extraDataLength = (dstPitch % 4) ? 4 - (dstPitch % 4) : 0;
	int padding = 0;

	out.writeByte('B');
	out.writeByte('M');
	out.writeUint32LE(surface->h * dstPitch + 54);
	out.writeUint32LE(0);
	out.writeUint32LE(54);
	out.writeUint32LE(40);
	out.writeUint32LE(surface->w);
	out.writeUint32LE(surface->h);
	out.writeUint16LE(1);
	out.writeUint16LE(24);
	out.writeUint32LE(0);
	out.writeUint32LE(0);
	out.writeUint32LE(0);
	out.writeUint32LE(0);
	out.writeUint32LE(0);
	out.writeUint32LE(0);


	for (uint y = surface->h; y-- > 0;) {
		out.write((const void *)surface->getBasePtr(0, y), dstPitch);
		out.write(&padding, extraDataLength);
	}

	// free tmp surface
	if (tmp) {
		tmp->free();
		delete tmp;
	}

	return true;
}

} // End of namespace Image
