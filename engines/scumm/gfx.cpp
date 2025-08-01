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

#include "common/system.h"
#include "scumm/actor.h"
#include "scumm/charset.h"
#ifdef ENABLE_HE
#include "scumm/he/intern_he.h"
#endif
#include "scumm/resource.h"
#include "scumm/scumm_v0.h"
#include "scumm/scumm_v5.h"
#include "scumm/scumm_v6.h"
#include "scumm/usage_bits.h"
#include "scumm/he/wiz_he.h"
#include "scumm/util.h"

#ifdef USE_ARM_GFX_ASM

#ifndef IPHONE
#define asmDrawStripToScreen _asmDrawStripToScreen
#define asmCopy8Col _asmCopy8Col
#endif

extern "C" void asmDrawStripToScreen(int height, int width, void const* text, void const* src, byte* dst,
	int vsPitch, int vmScreenWidth, int textSurfacePitch);
extern "C" void asmCopy8Col(byte* dst, int dstPitch, const byte* src, int height, uint8 bitDepth);
#endif /* USE_ARM_GFX_ASM */

namespace Scumm {

static void blit(byte *dst, int dstPitch, const byte *src, int srcPitch, int w, int h, uint8 bitDepth);
static void fill(byte *dst, int dstPitch, uint16 color, int w, int h, uint8 bitDepth);
#ifndef USE_ARM_GFX_ASM
static void copy8Col(byte *dst, int dstPitch, const byte *src, int height, uint8 bitDepth);
#endif
static void clear8Col(byte *dst, int dstPitch, int height, uint8 bitDepth);

struct StripTable {
	int offsets[160];
	int run[160];
	int color[160];
	int zoffsets[120];	// FIXME: Why only 120 here?
	int zrun[120];		// FIXME: Why only 120 here?
};

enum {
	kNoDelay = 0,
	// This should actually be 3 in all games using it;
	// every one of those games seems to accumulate some
	// kind of internal random delay while performing
	// the screen effect (like sound interrupts running,
	// forcing the SCUMM timer to a lower frequency).
	// I have added an extra quarter frame to emulate that.
	kPictureDelay = 4,
	kC64Delay = 6
};

#define NUM_SHAKE_POSITIONS 8
static const int8 shake_positions[NUM_SHAKE_POSITIONS] = {
	0, 1, 2, 1, 0, 2, 3, 1
};

/**
 * The following structs define four basic fades/transitions used by
 * transitionEffect(), each looking differently to the user.
 * Note that the stripTables contain strip numbers, and they assume
 * that the screen has 40 vertical strips (i.e. 320 pixel), and 25 horizontal
 * strips (i.e. 200 pixel). There is a hack in transitionEffect that
 * makes it work correctly in games which have a different screen height
 * (for example, 240 pixel), but nothing is done regarding the width.
 * The number of iterations is hardcoded from version 3 up to 7.
 * Version 8 doesn't have any of these effects at all.
 */
struct TransitionEffect {
	byte numOfIterations;
	int8 deltaTable[16];	// four times l / t / r / b
	byte stripTable[16];	// ditto
};

static const TransitionEffect transitionEffects[6] = {
	// Iris effect (looks like an opening/closing camera iris)
	{
		13,		// Number of iterations
		{
			1,  1, -1,  1,
		   -1,  1, -1, -1,
			1, -1, -1, -1,
			1,  1,  1, -1
		},
		{
			0,  0, 39,  0,
		   39,  0, 39, 24,
			0, 24, 39, 24,
			0,  0,  0, 24
		}
	},

	// Box wipe (a box expands from the upper-left corner to the lower-right corner)
	{
		25,		// Number of iterations
		{
			0,  1,  2,  1,
			2,  0,  2,  1,
			2,  0,  2,  1,
			0,  0,  0,  0
		},
		{
			0,  0,  0,  0,
			0,  0,  0,  0,
			1,  0,  1,  0,
		  255,  0,  0,  0
		}
	},

	// Box wipe (a box expands from the lower-right corner to the upper-left corner)
	{
		25,		// Number of iterations
		{
		   -2, -1,  0, -1,
		   -2, -1, -2,  0,
		   -2, -1, -2,  0,
			0,  0,  0,  0
		},
		{
		   39, 24, 39, 24,
		   39, 24, 39, 24,
		   38, 24, 38, 24,
		  255,  0,  0,  0
		}
	},

	// Inverse box wipe
	{
		25,		// Number of iterations
		{
			0, -1, -2, -1,
		   -2,  0, -2, -1,
		   -2,  0, -2, -1,
		    0,  0,  0,  0
		},
		{
			0, 24, 39, 24,
		   39,  0, 39, 24,
		   38,  0, 38, 24,
		  255,  0,  0,  0
		}
	},

	// Inverse iris effect, specially tailored for V1/V2 games
	{
		9,		// Number of iterations
		{
			-1, -1,  1, -1,
			-1,  1,  1,  1,
			-1, -1, -1,  1,
			 1, -1,  1,  1
		},
		{
			 7, 7, 32, 7,
			 7, 8, 32, 8,
			 7, 8,  7, 8,
			32, 7, 32, 8
		}
	},

	// Horizontal wipe (a box expands from left to right side). For MM NES
	{
		16,		// Number of iterations
		{
			  2,  0,  2,  0,
			  2,  0,  2,  0,
			  0,  0,  0,  0,
			  0,  0,  0,  0
		},
		{
			  0, 0,  0,  15,
			  1, 0,  1,  15,
			255, 0,  0,  0,
			255, 0,  0,  0
		}
	}

};


Gdi::Gdi(ScummEngine *vm) : _vm(vm) {
	_numZBuffer = 0;
	memset(_imgBufOffs, 0, sizeof(_imgBufOffs));
	_numStrips = 0;

	_paletteMod = 0;
	_roomPalette = vm->_roomPalette;
	_transparentColor = 255;
	_decomp_shr = 0;
	_decomp_mask = 0;
	_vertStripNextInc = 0;
	_zbufferDisabled = false;
	_objectMode = false;
	_distaff = false;
}

Gdi::~Gdi() {
}

GdiHE::GdiHE(ScummEngine *vm) : Gdi(vm), _tmskPtr(nullptr) {
}


GdiNES::GdiNES(ScummEngine *vm) : Gdi(vm) {
	memset(&_NES, 0, sizeof(_NES));
}

#ifdef USE_RGB_COLOR
GdiPCEngine::GdiPCEngine(ScummEngine *vm) : Gdi(vm) {
	memset(&_PCE, 0, sizeof(_PCE));
}

GdiPCEngine::~GdiPCEngine() {
	free(_PCE.roomTiles);
	free(_PCE.staffTiles);
	free(_PCE.masks);
}
#endif

GdiV1::GdiV1(ScummEngine *vm) : Gdi(vm) {
	memset(&_V1, 0, sizeof(_V1));
}

void GdiV1::setRenderModeColorMap(const byte *map) {
	_colorMap = map;
}

byte GdiV1::remapColorToRenderMode(byte col) const {
	assert(_colorMap);
	return _colorMap[col];
}

GdiV2::GdiV2(ScummEngine *vm) : Gdi(vm) {
	_roomStrips = nullptr;
}

GdiV2::~GdiV2() {
	free(_roomStrips);
}

#ifdef USE_RGB_COLOR
GdiHE16bit::GdiHE16bit(ScummEngine *vm) : GdiHE(vm) {
}
#endif

void Gdi::init() {
	_numStrips = _vm->_screenWidth / 8;

	// Increase the number of screen strips by one; needed for smooth scrolling
	if (_vm->_game.version >= 7) {
		// We now have mostly working smooth scrolling code in place for V7+ games
		// (i.e. The Dig, Full Throttle and COMI). It seems to work very well so far.
		//
		// To understand how we achieve smooth scrolling, first note that with it, the
		// virtual screen strips don't match the display screen strips anymore. To
		// overcome that problem, we simply use a screen pitch that is 8 pixel wider
		// than the actual screen width, and always draw one strip more than needed to
		// the backbuf (thus we have to treat the right border separately).
		_numStrips += 1;
	}
}

void Gdi::roomChanged(byte *roomptr) {
}

void GdiNES::roomChanged(byte *roomptr) {
	decodeNESGfx(roomptr);
}

#ifdef USE_RGB_COLOR
void GdiPCEngine::roomChanged(byte *roomptr) {
	decodePCEngineGfx(roomptr);
}
#endif

void Gdi::loadTiles(byte *roomptr) {
}

#ifdef USE_RGB_COLOR
void GdiPCEngine::loadTiles(byte *roomptr) {
	decodePCEngineTileData(_vm->findResourceData(MKTAG('T','I','L','E'), roomptr));
}
#endif

void GdiV1::roomChanged(byte *roomptr) {
	for (int i = 0; i < 4; i++){
		_V1.colors[i] = roomptr[6 + i];
	}
	decodeV1Gfx(roomptr + READ_LE_UINT16(roomptr + 10), _V1.charMap, 2048);
	decodeV1Gfx(roomptr + READ_LE_UINT16(roomptr + 12), _V1.picMap, roomptr[4] * roomptr[5]);
	decodeV1Gfx(roomptr + READ_LE_UINT16(roomptr + 14), _V1.colorMap, roomptr[4] * roomptr[5]);
	decodeV1Gfx(roomptr + READ_LE_UINT16(roomptr + 16), _V1.maskMap, roomptr[4] * roomptr[5]);

	// Read the mask data. The 16bit length value seems to always be 8 too big.
	// See bug #3458 for details on this.
	const byte *maskPtr = roomptr + READ_LE_UINT16(roomptr + 18);
	decodeV1Gfx(maskPtr + 2, _V1.maskChar, READ_LE_UINT16(maskPtr) - 8);
	_objectMode = true;
}

void GdiV2::roomChanged(byte *roomptr) {
	_roomStrips = generateStripTable(roomptr + READ_LE_UINT16(roomptr + 0x0A),
			_vm->_roomWidth, _vm->_roomHeight, _roomStrips);
}

#pragma mark -
#pragma mark --- Virtual Screens ---
#pragma mark -


void ScummEngine::initScreens(int b, int h) {
	int i;
	int adj = 0;

	for (i = 0; i < 4; i++) {
		_res->nukeResource(rtBuffer, i + 1);
		_res->nukeResource(rtBuffer, i + 5);
	}

#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
	if (_townsScreen) {
		if (!_townsClearLayerFlag && (h - b != _virtscr[kMainVirtScreen].h))
			_townsScreen->clearLayer(0);

		if (_game.id != GID_MONKEY) {
			_textSurface.fillRect(Common::Rect(0, 0, _textSurface.w * _textSurfaceMultiplier, _textSurface.h * _textSurfaceMultiplier), 0);
			_townsScreen->clearLayer(1);
		}
	}
#endif

	if (_macScreen) {
		_macScreen->fillRect(Common::Rect(_macScreen->w, _macScreen->h), 0);
		clearTextSurface();
	}

	if (_game.version >= 7) {
		initVirtScreen(kBannerVirtScreen, (_screenHeight / 2) - 10, _screenWidth, 13, false, false);
	} else if (_game.platform == Common::kPlatformFMTowns) {
		// HACK: The original only ever renders in 640x480 mode. The banners' top and bottom borders are exactly one unscaled pixel high. This will
		// still allow the text to fit in nicely. It does not work with scaled (2 pixel height) borders, though. So we add two extra pixels...
		int bannerHeight = (_textSurfaceMultiplier == 1) ? 12 : 20;
		initVirtScreen(kBannerVirtScreen, (b + adj + h) / 2 - bannerHeight / _textSurfaceMultiplier, _screenWidth * _textSurfaceMultiplier, bannerHeight, false, false);
	} else {
		initVirtScreen(kBannerVirtScreen, 80, _screenWidth, 12, false, false);
	}

	if ((_game.platform == Common::kPlatformNES) && (h != _screenHeight)) {
		// This is a hack to shift the whole screen downwards to match the original.
		// Otherwise we would have to do lots of coordinate adjustments all over
		// the code.
		adj = 16;
		initVirtScreen(kBannerVirtScreen, 0, _screenWidth, adj, false, false);
	}

	initVirtScreen(kMainVirtScreen, b + adj, _screenWidth, h - b, true, true);
	initVirtScreen(kTextVirtScreen, adj, _screenWidth, b, false, false);
	initVirtScreen(kVerbVirtScreen, h + adj, _screenWidth, _screenHeight - h - adj, false, false);
	_screenB = b;
	_screenH = h;

	_gdi->init();

#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
	// The right most visible strip for FM-Towns smooth scrolling
	_scrollFeedStrips[0] = _gdi->_numStrips - 1;
#endif
}

void ScummEngine::initVirtScreen(VirtScreenNumber slot, int top, int width, int height, bool twobufs,
													 bool scrollable) {
	VirtScreen *vs = &_virtscr[slot];
	int size;

	assert(height >= 0);
	assert((int)slot >= 0 && (int)slot < 4);

	if (_game.version >= 7) {
		if (slot == kMainVirtScreen && (_roomHeight != 0))
			height = _roomHeight;
	}

	vs->number = slot;
	vs->w = width;
	vs->topline = top;
	vs->h = height;
	vs->hasTwoBuffers = twobufs;
	vs->xstart = 0;
	vs->backBuf = nullptr;
	if (_game.features & GF_16BIT_COLOR)
		vs->format = Graphics::PixelFormat(2, 5, 5, 5, 0, 10, 5, 0, 0);
	else
		vs->format = Graphics::PixelFormat::createFormatCLUT8();
	vs->pitch = width * vs->format.bytesPerPixel;

	if (_game.version >= 7) {
		// Increase the pitch by one; needed to accommodate the extra screen
		// strip which we use to implement smooth scrolling. See Gdi::init().
		vs->pitch += 8;
	}

	size = vs->pitch * vs->h;
	if (scrollable) {
		// Allow enough spaces so that rooms can be up to 4 resp. 8 screens
		// wide. To achieve horizontal scrolling, SCUMM uses a neat trick:
		// only the offset into the screen buffer (xstart) is changed. That way
		// very little of the screen has to be redrawn, and we have a very low
		// memory overhead (namely for every pixel we want to scroll, we need
		// one additional byte in the buffer).
		if (_game.version >= 7) {
			size += vs->pitch * 8;
		} else {
			size += vs->pitch * 4;
		}
	}

	_res->createResource(rtBuffer, slot + 1, size);
	vs->setPixels(getResourceAddress(rtBuffer, slot + 1));
	if (_game.platform == Common::kPlatformNES)
		memset(vs->getBasePtr(0, 0), 0x1d, size);	// reset background (MM NES)
	else
		memset(vs->getBasePtr(0, 0), 0, size);		// reset background

	if (twobufs) {
		vs->backBuf = _res->createResource(rtBuffer, slot + 5, size);
	}

	if (slot != 3) {
		vs->setDirtyRange(0, height);
	}
}

VirtScreen *ScummEngine::findVirtScreen(int y) {
	VirtScreen *vs = _virtscr;
	int i;

	if (_forceBannerVirtScreen)
		return &vs[3];

	for (i = 0; i < 3; i++, vs++) {
		if (y >= vs->topline && y < vs->topline + vs->h) {
			return vs;
		}
	}
	return nullptr;
}

void ScummEngine::markRectAsDirty(VirtScreenNumber virt, int left, int right, int top, int bottom, int dirtybit) {
	VirtScreen *vs = &_virtscr[virt];
	int lp, rp;

	if (left > right || top > bottom)
		return;
	if (top > vs->h || bottom < 0)
		return;

	if (top < 0)
		top = 0;
	if (bottom > vs->h)
		bottom = vs->h;

	if (virt == kMainVirtScreen && dirtybit) {

		lp = left / 8 + _screenStartStrip;
		if (lp < 0)
			lp = 0;

		rp = (right + vs->xstart) / 8;
		if (_game.version >= 7) {
			if (rp > 409)
				rp = 409;
		} else {
			if (rp >= 200)
				rp = 200;
		}
		for (; lp <= rp; lp++)
			setGfxUsageBit(lp, dirtybit);
	}

	// The following code used to be in the separate method setVirtscreenDirty
	lp = left / 8;
	rp = right / 8;

	if ((lp >= _gdi->_numStrips) || (rp < 0))
		return;
	if (lp < 0)
		lp = 0;
	if (rp >= _gdi->_numStrips)
		rp = _gdi->_numStrips - 1;

	while (lp <= rp) {
		if (top < vs->tdirty[lp])
			vs->tdirty[lp] = top;
		if (bottom > vs->bdirty[lp])
			vs->bdirty[lp] = bottom;
		lp++;
	}
}

/**
 * Update all dirty screen areas. This method blits all of the internal engine
 * graphics to the actual display, as needed. In addition, the 'shaking'
 * code in the backend is controlled from here.
 */
void ScummEngine::drawDirtyScreenParts() {
	// Update verbs
	updateDirtyScreen(kVerbVirtScreen);

	// Update the conversation area (at the top of the screen)
	updateDirtyScreen(kTextVirtScreen);

	// Update game area ("stage")
	if (camera._last.x != camera._cur.x || (_game.version >= 7 && (camera._cur.y != camera._last.y))) {
		// Camera moved: redraw everything
		VirtScreen *vs = &_virtscr[kMainVirtScreen];
		drawStripToScreen(vs, 0, vs->w, 0, vs->h);
		vs->setDirtyRange(vs->h, 0);
	} else {
		updateDirtyScreen(kMainVirtScreen);
	}
}

void ScummEngine_v6::drawDirtyScreenParts() {
	// For the Full Throttle credits to work properly, the blast
	// texts have to be drawn before the blast objects. Unless
	// someone can think of a better way to achieve this effect.

	if (_game.version >= 7 && VAR(VAR_BLAST_ABOVE_TEXT) == 1) {
		drawBlastTexts();
		drawBlastObjects();
		if (_game.version == 8) {
			// Does this case ever happen? We need to draw the
			// actor over the blast object, so we're forced to
			// also draw it over the subtitles.
			processUpperActors();
		}
	} else {
		drawBlastObjects();
		if (_game.version == 8) {
			// Do this before drawing blast texts. Subtitles go on
			// top of the CoMI verb coin, e.g. when Murray is
			// talking to himself early in the game.
			processUpperActors();
		}
		drawBlastTexts();
	}

	// Call the original method.
	ScummEngine::drawDirtyScreenParts();

	// Remove all blasted objects/text again, except
	// for v7-8 which do that at a later time.
	if (_game.version < 7) {
		removeBlastTexts();
		removeBlastObjects();
	}
}

/**
 * Blit the dirty data from the given VirtScreen to the display. If the camera moved,
 * a full blit is done, otherwise only the visible dirty areas are updated.
 */
void ScummEngine::updateDirtyScreen(VirtScreenNumber slot) {
	VirtScreen *vs = &_virtscr[slot];

	// Do nothing for unused virtual screens
	if (vs->h == 0)
		return;

	int i;
	int w = 8;
	int start = 0;

	for (i = 0; i < _gdi->_numStrips; i++) {
		if (vs->bdirty[i]) {
			const int top = vs->tdirty[i];
			const int bottom = vs->bdirty[i];
			vs->tdirty[i] = vs->h;
			vs->bdirty[i] = 0;
			if (i != (_gdi->_numStrips - 1) && vs->bdirty[i + 1] == bottom && vs->tdirty[i + 1] == top) {
				// Simple optimizations: if two or more neighboring strips
				// form one bigger rectangle, coalesce them.
				w += 8;
				continue;
			}
#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
			if (_game.platform == Common::kPlatformFMTowns && vs->number == kBannerVirtScreen) {
				int scl = _textSurfaceMultiplier;
				towns_drawStripToScreen(vs, start * 8 * scl, (vs->topline + top) * scl, start * 8 * scl, top * scl, w * scl, bottom - top);
			} else
#endif
				drawStripToScreen(vs, start * 8, w, top, bottom);
			w = 8;
		}
		start = i + 1;
	}
}

/**
 * Blit the specified rectangle from the given virtual screen to the display.
 * Note: t and b are in *virtual screen* coordinates, while x is relative to
 * the *real screen*. This is due to the way tdirty/vdirty work: they are
 * arrays which map 'strips' (sections of the real screen) to dirty areas as
 * specified by top/bottom coordinate in the virtual screen.
 */
void ScummEngine::drawStripToScreen(VirtScreen *vs, int x, int width, int top, int bottom) {
	// Short-circuit if nothing has to be drawn
	if (bottom <= top || top >= vs->h)
		return;

	// Some paranoia checks
	assert(top >= 0 && bottom <= vs->h);
	assert(x >= 0 && width <= vs->pitch);
	assert(_textSurface.getPixels());

	// Some extra vertical alignment for certain render modes. It matters for MI1EGA. The dithering patterns require the alignment,
	// otherwise there will be visible glitches. It can be found in the original interpreters.
	int align = (_game.version > 2 && (_renderMode == Common::kRenderCGA || _renderMode == Common::kRenderHercG || _renderMode == Common::kRenderHercA)) ? 4 : (_enableEGADithering ? 2 : 1);
	top &= ~(align - 1);
	if (bottom & (align - 1))
		bottom = (bottom + align) & ~(align - 1);

	// Perform some clipping
	if (width > vs->w - x)
		width = vs->w - x;
	if (top < _screenTop)
		top = _screenTop;
	if (bottom > _screenTop + _screenHeight)
		bottom = _screenTop + _screenHeight;

	// Convert the vertical coordinates to real screen coords
	int y = vs->topline + top - _screenTop;
	int height = bottom - top;

	if (width <= 0 || height <= 0)
		return;

	if (_macScreen && _game.version <= 3) {
		mac_drawStripToScreen(vs, top, x, y, width, height);
		return;
	}

	const void *src = vs->getPixels(x, top);
	int m = _textSurfaceMultiplier;
	int vsPitch;
	int pitch = vs->pitch;
	vsPitch = vs->pitch - width * vs->format.bytesPerPixel;

	// In MM NES If we're repainting the entire screen, just make everything black
	if ((_game.platform == Common::kPlatformNES) && width == 256 && height == 240) {
		_system->fillScreen(0x1d);
		return;
	}

	if (_game.version < 7) {
		// For The Dig, FT and COMI, we just blit everything to the screen at once.
		// For older games, things are more complicated. First off, we need to
		// deal with the _textSurface, which needs to be composited over the
		// screen contents. Secondly, a rendering mode might be active, which
		// means a filter has to be applied.

		// Compute pointer to the text surface
		assert(_compositeBuf);
		const void *text = _textSurface.getBasePtr(x * m, y * m);

		// The values x, width, etc. are all multiples of 8 at this point,
		// so loop unrolloing might be a good idea...
		assert(IS_ALIGNED(text, 4));
		assert(0 == (width & 3));

#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
		if (_game.platform == Common::kPlatformFMTowns) {
			towns_drawStripToScreen(vs, x, y, x, top, width, height);
			return;
		} else
#endif
		// Compose the text over the game graphics
		if (_outputPixelFormat.bytesPerPixel == 2) {
			const byte *srcPtr = (const byte *)src;
			const byte *textPtr = (byte *)_textSurface.getBasePtr(x * m, y * m);
			byte *dstPtr = _compositeBuf;

			for (int h = 0; h < height * m; ++h) {
				for (int w = 0; w < width * m; ++w) {
					uint16 tmp = *textPtr++;
					if (tmp == CHARSET_MASK_TRANSPARENCY) {
						tmp = READ_UINT16(srcPtr);
						WRITE_UINT16(dstPtr, tmp); dstPtr += 2;
					} else if (_game.heversion != 0) {
						error ("16Bit Color HE Game using old charset");
					} else {
						WRITE_UINT16(dstPtr, _16BitPalette[tmp]); dstPtr += 2;
					}
					srcPtr += vs->format.bytesPerPixel;
				}
				srcPtr += vsPitch;
				textPtr += _textSurface.pitch - width * m;
			}
		} else {
#ifdef USE_ARM_GFX_ASM
			asmDrawStripToScreen(height, width, text, src, _compositeBuf, vs->pitch, width, _textSurface.pitch);
#else
			// We blit four pixels at a time, for improved performance.
			const uint32 *src32 = (const uint32 *)src;
			uint32 *dst32 = (uint32 *)_compositeBuf;

			vsPitch >>= 2;

			const uint32 *text32 = (const uint32 *)text;
			const int textPitch = (_textSurface.pitch - width * m) >> 2;
			for (int h = height * m; h > 0; --h) {
				for (int w = width * m; w > 0; w -= 4) {
					uint32 temp = *text32++;

					// Generate a byte mask for those text pixels (bytes) with
					// value CHARSET_MASK_TRANSPARENCY. In the end, each byte
					// in mask will be either equal to 0x00 or 0xFF.
					// Doing it this way avoids branches and bytewise operations,
					// at the cost of readability ;).
					uint32 mask = temp ^ CHARSET_MASK_TRANSPARENCY_32;
					mask = (((mask & 0x7f7f7f7f) + 0x7f7f7f7f) | mask) & 0x80808080;
					mask = ((mask >> 7) + 0x7f7f7f7f) ^ 0x80808080;

					// The following line is equivalent to this code:
					//   *dst32++ = (*src32++ & mask) | (temp & ~mask);
					// However, some compilers can generate somewhat better
					// machine code for this equivalent statement:
					*dst32++ = ((temp ^ *src32++) & mask) ^ temp;
				}
				src32 += vsPitch;
				text32 += textPitch;
			}
#endif
		}
		src = _compositeBuf;
		pitch = width * vs->format.bytesPerPixel;

		if (_game.platform == Common::kPlatformNES) {
			// HACK: This is dirty hack which renders narrow NES rooms centered
			// NES can address negative number strips and that poses problem for
			// our code. So instead of adding zillions of fixes and potentially
			// breaking other games, we shift it right at the rendering stage.
			//
			// This hack originally checked for (vs->number == kTextVirtScreen) as well.
			// This causes bug #3594/#13107 to happen though...
			if (((_NESStartStrip > 0) && (vs->number == kMainVirtScreen))) {
				x += 16;
				while (x + width >= _screenWidth)
					width -= 16;
				if (width <= 0)
					return;

				// HACK: In this way we won't get a screen with dirty side strips when
				// loading a narrow room in a full screen room.
				if (width == 224 && height == 240 && x == 16) {
					char blackbuf[16 * 240];
					memset(blackbuf, 0, 16 * 240); // Prepare a buffer 16px wide and 240px high, to fit on a lateral strip

					width = 240; // Fix right strip
					_system->copyRectToScreen(blackbuf, 16, 0, 0, 16, 240); // Fix left strip
				}
			}
		} else if (_useCJKMode && m == 2) {
			pitch *= m;
			x *= m;
			y *= m;
			width *= m;
			height *= m;
		} else if (_enableEGADithering) {
			// EGA mode for certain VGA versions (MI2, LOOM Talkie)
			src = ditherVGAtoEGA(pitch, x, y, width, height);
		} else if (_game.platform == Common::kPlatformDOS && _game.version < 5) {
			// CGA and Hercules modes for MM/ZAK v1/v2, INDY3, LOOM, MI1EGA
			src = postProcessDOSGraphics(vs, pitch, x, y, width, height);
		}
	}

	if (_macScreen && _game.platform == Common::kPlatformMacintosh && _game.version > 3) {
		// Instead of using mac_drawStripToScreen(), we draw the entire already processed
		// video buffer to screen with the following routine, because for newer games we
		// need the text overlay to be already stamped on top of the buffer.
		mac_drawBufferToScreen((const byte *)src, pitch, x, y, width, height);
	} else {
		// Finally blit the whole thing to the screen
		_system->copyRectToScreen(src, pitch, x, y, width, height);
	}
}

const byte *ScummEngine::postProcessDOSGraphics(VirtScreen *vs, int &pitch, int &x, int &y, int &width, int &height) const {
	static const byte v2VrbColMap[] =	{ 0x0, 0x5, 0x5, 0x5, 0xA, 0xA, 0xA, 0xF, 0xF, 0x5, 0x5, 0x5, 0xA, 0xA, 0xF, 0xF };
	static const byte v2TxtColMap[] =	{ 0x0, 0xF, 0xA, 0x5, 0xA, 0x5, 0x5, 0xF, 0xA, 0xA, 0xA, 0xA, 0xA, 0x5, 0x5, 0xF };
	static const byte mmv1VrbColMap[] =	{ 0x0, 0x5, 0x5, 0x5, 0xA, 0xA, 0xA, 0xF, 0xA, 0x5, 0x5, 0x5, 0xA, 0xA, 0xA, 0xF };
	static const byte v2MainColMap[] =	{ 0x0, 0x4, 0x1, 0x5, 0x8, 0xA, 0x2, 0xF, 0xC, 0x7, 0xD, 0x5, 0xE, 0xB, 0xD, 0xF };

	static const byte v3MainColMap[] =	{
		0x0, 0x4, 0x1, 0x5, 0x8, 0xA, 0x2, 0x3, 0xC, 0x7, 0xD, 0x5, 0xF, 0xB, 0x5, 0xF, 0x0, 0x1, 0x4, 0x5, 0x2, 0xA, 0x8, 0xC, 0x3, 0xD, 0x5, 0x5, 0xF, 0xE, 0x5, 0xF
	};

	static const byte v4MainColMap[] =	{
		0x0, 0x4, 0x1, 0x5, 0x2, 0xA, 0x2, 0x3, 0x0, 0x5, 0x5, 0x7, 0xF, 0xE, 0x5, 0xF, 0x0, 0x1, 0x4, 0x5, 0x8, 0xA, 0x8, 0xC, 0x0, 0x7, 0x5, 0xD, 0xF, 0xB, 0x5, 0xF,
		0x0, 0x4, 0x1, 0x5, 0x2, 0xA, 0x2, 0x3, 0x0, 0x5, 0x5, 0x7, 0xF, 0xE, 0x5, 0xF, 0x0, 0x1, 0x4, 0x5, 0x8, 0xA, 0x8, 0xC, 0x0, 0xD, 0x5, 0xD, 0xF, 0xB, 0x5, 0xF
	};

	static const byte hrcTableV4[32] = {
		0x00, 0x08, 0xAA, 0xBB, 0x55, 0x66, 0x99, 0x7F, 0x11, 0x55, 0x77, 0xEE, 0xAA, 0xEE, 0xFF, 0xFF,
		0x00, 0x80, 0xAA, 0xDD, 0x00, 0x99, 0x66, 0xF7, 0x44, 0xAA, 0xDD, 0x77, 0xFF, 0xBB, 0xBB, 0xFF
	};

	static const byte *mainColMap[] = { nullptr, nullptr, v2MainColMap, v3MainColMap, v4MainColMap };

	byte tmpTxtColMap[16];
	for (uint8 i = 0; i < ARRAYSIZE(tmpTxtColMap); ++i)
		tmpTxtColMap[i] = mmv1VrbColMap[_gdi->remapColorToRenderMode(i)];

	byte *res = _compositeBuf;
	byte *dst = _compositeBuf;
	const byte *src = res;
	bool renderHerc = (_renderMode == Common::kRenderHercA || _renderMode == Common::kRenderHercG);
	bool renderV1 = (_game.version == 1);
	bool renderV3 = _game.version > 2;

	if (!renderV1 && !renderHerc && _renderMode != Common::kRenderCGA)
		return res;

	const byte *colMap = (_game.id == GID_ZAK || _game.version == 2) ? ((vs->number == kVerbVirtScreen || renderHerc) ? v2VrbColMap : v2TxtColMap) : (vs->number == kVerbVirtScreen ? mmv1VrbColMap : tmpTxtColMap);
	const byte *colMap2 = mainColMap[_game.version];

	// For LOOM and INDY3, CGA gets dithered as 2x2 squares, for MI1EGA as 2x4 squares. Odd lines have the colors swapped, so there will be checkered patterns.
	uint8 lnMod = (_game.version > 3 && !renderHerc) ? 0x40 : 0x20;
	uint8 lnIdx = renderV3 ? ((y & ((lnMod >> 4) - 1)) << 4) : 0;

	if (_renderMode == Common::kRenderCGA || _renderMode == Common::kRenderCGAComp) {
		if (renderV3 || vs->number == kMainVirtScreen) {
			for (int h = height; h; --h) {
				for (int w = width >> 1; w; --w) {
					byte c = renderV1 ? *src : (colMap2[src[0] + lnIdx] & 0x0C) | (colMap2[src[1] + lnIdx] & 0x03);
					*dst++ = (c >> 2) & 3;
					*dst++ = c & 3;
					src += 2;
				}
				if (renderV3)
					lnIdx = (lnIdx + 0x10) % lnMod;
			}
		} else {
			for (int h = height; h; --h) {
				for (int w = width >> 1; w; --w) {
					*dst++ = (colMap[*src++] >> 2) & 3;
					*dst++ = colMap[*src++] & 3;
				}
			}
		}

	} else if (renderHerc || _renderMode == Common::kRenderCGA_BW) {
		// The monochrome rendering is very similar for Hercules and CGA b/w. For Hercules we have to do some corrections to fit into the 350 pixels height.
		// For Hercules V1/2, the text and verb vs are rendered in normal height, only the main vs gets scaled by leaving out every other line. Hercules V4
		// instead scales everything in a 4-to-7 lines ratio. And for all versions, we center the image horizontally within the 720 pixels width.
		// For CGA b/w the origial resolution is 640x200, so we just scale that to our 640x400 by repeating each line.
		pitch = renderHerc ? kHercWidth : (_screenWidth << 1);
		dst = res = _hercCGAScaleBuf;
		int pitch1 = (pitch - width) << 1;

		if (renderV3) {
			// This is for MI1EGA Hercules only
			pitch1 = pitch - (width << 1);
			int height2 = height >> 2;
			height = height2 * 7;
			y = (y << 1) - (y >> 2);

			for (int h1 = height2; h1; --h1) {
				lnIdx = 0; // The 7-lines pattern always starts from the beginning. Which works fine, since the strips get vertically aligned for Hercules and CGA.
				for (int h2 = 7; h2; --h2) {
					for (int w = width >> 2; w; --w) {
						byte c = (hrcTableV4[src[0] + lnIdx] & 0xC0) | (hrcTableV4[src[1] + lnIdx] & 0x30) | (hrcTableV4[src[2] + lnIdx] & 0x0C) | (hrcTableV4[src[3] + lnIdx] & 0x03);
						for (int i = 7; i >= 0; --i)
							*dst++ = (c >> i) & 1;
						src += 4;
					}
					dst += pitch1;
					if (lnIdx ^= 0x10)
						src -= width;
				}
				src += width;
			}

		} else if (vs->number == kMainVirtScreen) {
			// V1/2 Hercules and CGA b/w mode
			uint32 *dst2 = (uint32*)(dst + pitch);
			int pitch2 = pitch1 >> 2;
			int height2 = height;

			if (renderHerc) {
				y = (y - vs->topline) * 2 + vs->topline;
				height = MIN<int>(height << 1, kHercHeight - y);
				height2 = height >> 1;
			}

			for (int h = height2; h; --h) {
				for (int w = width >> 1; w; --w) {
					const uint32 *s = (const uint32*)dst;
					byte c = renderV1 ? *src : (colMap2[src[0]] & 0x0C) | (colMap2[src[1]] & 0x03);
					*dst++ = (c >> 3) & 1;
					*dst++ = (c >> 2) & 1;
					*dst++ = (c >> 1) & 1;
					*dst++ = c & 1;
					*dst2++ = renderHerc ? 0 : *s;
					src += 2;
				}
				dst += pitch1;
				dst2 += pitch2;
			}

		} else {
			// V1/2 Hercules and CGA b/w mode
			if (renderHerc) {
				pitch1 = kHercWidth - (width << 1);
				y -= vs->topline;
				if (vs->number == kVerbVirtScreen) {
					y += vs->topline * 2 - 16;
					height = MIN<int>(height, kHercHeight - y);
				}
			}

			uint16 *dst2 = (uint16*)(dst + pitch);
			int pitch2 = pitch1 >> 1;

			for (int h = height; h; --h) {
				for (int w = width; w; --w) {
					const uint16 *s = (const uint16*)dst;
					uint8 col = colMap[*src++];
					*dst++ = (col >> 1) & 1;
					*dst++ = col & 1;
					if (!renderHerc)
						*dst2++ = *s;
				}
				dst += pitch1;
				dst2 += pitch2;
			}
		}

		x <<= 1;
		width <<= 1;

		if (renderHerc) {
			x += 40;
		} else {
			y <<= 1;
			height <<= 1;
		}

	} else if (renderV1 && vs->number == kTextVirtScreen) {
		// For EGA, the only colors that need remapping are for the kTextVirtScreen.
		// ZAKv1 is the only game that is affected by this. The original interpreter
		// will also apply this mapping in VGA mode (as we do) but not in MCGA mode.
		// So, should we ever decide to offer a separate MCGA render mode, then we
		// should skip this for that mode...
		for (uint8 i = 0; i < ARRAYSIZE(tmpTxtColMap); ++i)
			tmpTxtColMap[i] = _gdi->remapColorToRenderMode(i);
		for (int h = height; h; --h)  {
			for (int w = width; w; --w)
				*dst++ = tmpTxtColMap[*src++];
		}
	}

	return res;
}

const byte *ScummEngine::ditherVGAtoEGA(int &pitch, int &x, int &y, int &width, int &height) const {
	pitch <<= 1;
	int pitch2 = (pitch - width) << 1;

	uint8 *dst0 = _hercCGAScaleBuf;
	uint8 *dst1 = _hercCGAScaleBuf + pitch;
	uint8 *src = _compositeBuf;

	for (int i = height, st = 1 ^ (y & 1); i; --i, st ^= 1) {
		for (int ii = width; ii; --ii) {
			byte in = *src++;
			*dst0++ = *dst1++ = _egaColorMap[st][in];
			*dst0++ = *dst1++ = _egaColorMap[st ^ 1][in];
		}
		dst0 += pitch2;
		dst1 += pitch2;
	}

	x <<= 1;
	y <<= 1;
	width <<= 1;
	height <<= 1;

	return _hercCGAScaleBuf;
}

#pragma mark -
#pragma mark --- Background buffers & charset mask ---
#pragma mark -


void ScummEngine::initBGBuffers(int height) {
	const byte *ptr;
	int size, itemsize, i;
	byte *room;

	if (_game.version >= 7) {
		// Resize main virtual screen in V7 games. This is necessary
		// because in V7, rooms may be higher than one screen, so we have
		// to accommodate for that.
		initVirtScreen(kMainVirtScreen, _virtscr[kMainVirtScreen].topline, _screenWidth, height, true, true);
	}

	if (_game.heversion >= 70)
		room = getResourceAddress(rtRoomImage, _roomResource);
	else
		room = getResourceAddress(rtRoom, _roomResource);

	if (_game.version <= 3) {
		_gdi->_numZBuffer = 2;
	} else if (_game.features & GF_SMALL_HEADER) {
		int off;
		ptr = findResourceData(MKTAG('S','M','A','P'), room);
		_gdi->_numZBuffer = 0;

		if (_game.features & GF_16COLOR)
			off = READ_LE_UINT16(ptr);
		else
			off = READ_LE_UINT32(ptr);

		while (off && _gdi->_numZBuffer < 4) {
			_gdi->_numZBuffer++;
			ptr += off;
			off = READ_LE_UINT16(ptr);
		}
	} else if (_game.version == 8) {
		// in V8 there is no RMIH and num z buffers is in RMHD
		ptr = findResource(MKTAG('R','M','H','D'), room);
		_gdi->_numZBuffer = READ_LE_UINT32(ptr + 24) + 1;
	} else if (_game.heversion >= 70) {
		ptr = findResource(MKTAG('R','M','I','H'), room);
		_gdi->_numZBuffer = READ_LE_UINT16(ptr + 8) + 1;
	} else {
		ptr = findResource(MKTAG('R','M','I','H'), findResource(MKTAG('R','M','I','M'), room));
		_gdi->_numZBuffer = READ_LE_UINT16(ptr + 8) + 1;
	}
	assert(_gdi->_numZBuffer >= 1 && _gdi->_numZBuffer <= 8);

	if (_game.version >= 7)
		itemsize = (_roomHeight + 10) * _gdi->_numStrips;
	else
		itemsize = (_roomHeight + 4) * _gdi->_numStrips;


	size = itemsize * _gdi->_numZBuffer;
	memset(_res->createResource(rtBuffer, 9, size), 0, size);

	for (i = 0; i < (int)ARRAYSIZE(_gdi->_imgBufOffs); i++) {
		if (i < _gdi->_numZBuffer)
			_gdi->_imgBufOffs[i] = i * itemsize;
		else
			_gdi->_imgBufOffs[i] = (_gdi->_numZBuffer - 1) * itemsize;
	}
}

/**
 * Redraw background as needed, i.e. the left/right sides if scrolling took place etc.
 * Note that this only updated the virtual screen, not the actual display.
 */
void ScummEngine::redrawBGAreas() {
	int i;
	int diff;
	int val = 0;

	if (_game.id != GID_PASS && _game.version >= 4 && _game.version <= 6) {
		// Starting with V4 games (with the exception of the PASS demo), text
		// is drawn over the game graphics (as  opposed to be drawn in a
		// separate region of the screen). So, when scrolling in one of these
		// games (pre-new camera system), if actor text is visible (as indicated
		// by the _hasMask flag), we first remove it before proceeding.
		if (camera._cur.x != camera._last.x && _charset->_hasMask)
			stopTalk();
	}

	// Redraw parts of the background which are marked as dirty.
	if (!_fullRedraw && _bgNeedsRedraw) {
		for (i = 0; i != _gdi->_numStrips; i++) {
			if (testGfxUsageBit(_screenStartStrip + i, USAGE_BIT_DIRTY)) {
				redrawBGStrip(i, 1);
			}
		}
	}

	if (_game.version >= 7) {
		diff = camera._cur.x / 8 - camera._last.x / 8;
		if (_fullRedraw || ABS(diff) >= _gdi->_numStrips) {
			_bgNeedsRedraw = false;
			redrawBGStrip(0, _gdi->_numStrips);
		} else if (diff > 0) {
			val = -diff;
			redrawBGStrip(_gdi->_numStrips - diff, diff);
		} else if (diff < 0) {
			val = -diff;
			redrawBGStrip(0, -diff);
		}
	} else {
		diff = camera._cur.x - camera._last.x;
		if (!_fullRedraw && diff == 8) {
			val = -1;
			scrollLeft();
		} else if (!_fullRedraw && diff == -8) {
			val = +1;
			scrollRight();
		} else if (_fullRedraw || diff != 0) {
			if (_game.version <= 5) {
				((ScummEngine_v5 *)this)->clearFlashlight();
			}
			_bgNeedsRedraw = false;
			redrawBGStrip(0, _gdi->_numStrips);
		}
	}

	drawRoomObjects(val);
	_bgNeedsRedraw = false;
}

#ifdef ENABLE_HE
void ScummEngine_v71he::redrawBGAreas() {
	if (camera._cur.x != camera._last.x && _charset->_hasMask)
		stopTalk();

	byte *room = getResourceAddress(rtRoomImage, _roomResource) + _IM00_offs;
	if (_fullRedraw) {
		_bgNeedsRedraw = false;
		_gdi->drawBMAPBg(room, &_virtscr[kMainVirtScreen]);
	}

	drawRoomObjects(0);
	_bgNeedsRedraw = false;
}

void ScummEngine_v72he::redrawBGAreas() {
	ScummEngine_v71he::redrawBGAreas();
	_wiz->flushAWizBuffer();
}
#endif

void ScummEngine::redrawBGStrip(int start, int num) {
	byte *room;

	int s = _screenStartStrip + start;

	for (int i = 0; i < num; i++)
		setGfxUsageBit(s + i, USAGE_BIT_DIRTY);

	if (_game.heversion >= 70)
		room = getResourceAddress(rtRoomImage, _roomResource);
	else
		room = getResourceAddress(rtRoom, _roomResource);

	_gdi->drawBitmap(room + _IM00_offs, &_virtscr[kMainVirtScreen], s, 0, _roomWidth, _virtscr[kMainVirtScreen].h, s, num, 0);
}

void ScummEngine::restoreBackground(Common::Rect rect, byte backColor) {
	VirtScreen *vs;
	byte *screenBuf;

	if (rect.top < 0)
		rect.top = 0;
	if (rect.left >= rect.right || rect.top >= rect.bottom)
		return;

	if ((vs = findVirtScreen(rect.top)) == nullptr)
		return;

	if (rect.left > vs->w)
		return;

	// Indy4 Amiga always uses the room or verb palette map to match colors to
	// the currently setup palette, thus we need to select it over here too.
	// Done like the original interpreter.
	if (_game.platform == Common::kPlatformAmiga && _game.id == GID_INDY4) {
		if (vs->number == kVerbVirtScreen)
			backColor = _verbPalette[backColor];
		else
			backColor = _roomPalette[backColor];
	}

	// MM NES background color is 0x1d
	if (_game.platform == Common::kPlatformNES) {
		backColor = 0x1d;
	}

	// Convert 'rect' to local (virtual screen) coordinates
	rect.top -= vs->topline;
	rect.bottom -= vs->topline;

	rect.clip(vs->w, vs->h);

	const int height = rect.height();
	const int width = rect.width();

#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
	if (_game.platform == Common::kPlatformFMTowns && _game.id == GID_MONKEY && vs->number == kVerbVirtScreen && rect.bottom <= 154)
		rect.right = 319;
#endif

	markRectAsDirty(vs->number, rect, USAGE_BIT_RESTORED);

	screenBuf = vs->getPixels(rect.left, rect.top);

	if (!height)
		return;

	if (vs->hasTwoBuffers && _currentRoom != 0 && isLightOn()) {
		blit(screenBuf, vs->pitch, vs->getBackPixels(rect.left, rect.top), vs->pitch, width, height, vs->format.bytesPerPixel);
		if (vs->number == kMainVirtScreen && _charset->_hasMask) {
#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
			if (_game.platform == Common::kPlatformFMTowns) {
				byte *mask = (byte *)_textSurface.getBasePtr(rect.left * _textSurfaceMultiplier, (rect.top + vs->topline) * _textSurfaceMultiplier);
				fill(mask, _textSurface.pitch, 0, width * _textSurfaceMultiplier, height * _textSurfaceMultiplier, _textSurface.format.bytesPerPixel);
			} else
#endif
			{
				byte *mask = (byte *)_textSurface.getBasePtr(rect.left, rect.top - _screenTop);
				fill(mask, _textSurface.pitch, CHARSET_MASK_TRANSPARENCY, width * _textSurfaceMultiplier, height * _textSurfaceMultiplier, _textSurface.format.bytesPerPixel);
			}
		}
	} else {
#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
		if (_game.platform == Common::kPlatformFMTowns) {
			backColor |= (backColor << 4);
			byte *mask = (byte *)_textSurface.getBasePtr(rect.left * _textSurfaceMultiplier, (rect.top + vs->topline) * _textSurfaceMultiplier);
			fill(mask, _textSurface.pitch, backColor, width * _textSurfaceMultiplier, height * _textSurfaceMultiplier, _textSurface.format.bytesPerPixel);
		}
#endif

		if (_macScreen) {
			byte *mask = (byte *)_textSurface.getBasePtr(rect.left * _textSurfaceMultiplier, (rect.top + vs->topline) * _textSurfaceMultiplier);
			fill(mask, _textSurface.pitch, CHARSET_MASK_TRANSPARENCY, width * _textSurfaceMultiplier, height * _textSurfaceMultiplier, _textSurface.format.bytesPerPixel);
		}

		if (_game.features & GF_16BIT_COLOR)
			fill(screenBuf, vs->pitch, _16BitPalette[backColor], width, height, vs->format.bytesPerPixel);
		else
			fill(screenBuf, vs->pitch, backColor, width, height, vs->format.bytesPerPixel);
	}
}

void ScummEngine::restoreCharsetBg() {
	_nextLeft = _string[0].xpos;
	_nextTop = _string[0].ypos + _screenTop;

	if (_charset->_hasMask || _postGUICharMask) {
		_postGUICharMask = false;
		_charset->_hasMask = false;
		_charset->_str.left = -1;
		_charset->_left = -1;

		if (_macGui && _game.id == GID_INDY3 && _charset->_textScreenID == kTextVirtScreen) {
			mac_undrawIndy3TextBox();
			return;
		}

		// Restore background on the whole text area. This code is based on
		// restoreBackground(), but was changed to only restore those parts which are
		// currently covered by the charset mask.

		VirtScreen *vs = &_virtscr[_charset->_textScreenID];
		if (!vs->h)
			return;

		markRectAsDirty(vs->number, Common::Rect(vs->w, vs->h), USAGE_BIT_RESTORED);

		byte *screenBuf = vs->getPixels(0, 0);

		if (vs->hasTwoBuffers && _currentRoom != 0 && isLightOn()) {
			if (vs->number != kMainVirtScreen) {
				// Restore from back buffer
				const byte *backBuf = vs->getBackPixels(0, 0);
				blit(screenBuf, vs->pitch, backBuf, vs->pitch, vs->w, vs->h, vs->format.bytesPerPixel);
			}
		} else {
			if (!(_game.version < 4 && _messageBannerActive && (getCurrentLights() & LIGHTMODE_flashlight_on))) {
				// Clear area
				if (_game.platform == Common::kPlatformNES)
					memset(screenBuf, 0x1d, vs->h * vs->pitch);
				else
					memset(screenBuf, 0, vs->h * vs->pitch);
			}
		}

		if (vs->hasTwoBuffers || _macScreen) {
			// Clean out the charset mask
			clearTextSurface();
		}
	}
}

void ScummEngine::clearCharsetMask() {
	memset(getResourceAddress(rtBuffer, 9), 0, _gdi->_imgBufOffs[1]);
}

void ScummEngine::clearTextSurface() {
	towns_fillTopLayerRect(0, 0, _textSurface.w, _textSurface.h, 0);
	fill((byte *)_textSurface.getPixels(), _textSurface.pitch,
#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
		_game.platform == Common::kPlatformFMTowns ? 0 :
#endif
		CHARSET_MASK_TRANSPARENCY,  _textSurface.w, _textSurface.h, _textSurface.format.bytesPerPixel);
}

byte *ScummEngine::getMaskBuffer(int x, int y, int z) {
	return _gdi->getMaskBuffer((x + _virtscr[kMainVirtScreen].xstart) / 8, y, z);
}

byte *Gdi::getMaskBuffer(int x, int y, int z) {
	return _vm->getResourceAddress(rtBuffer, 9)
			+ x + y * _numStrips + _imgBufOffs[z];
}


#pragma mark -
#pragma mark --- Misc ---
#pragma mark -

static void blit(byte *dst, int dstPitch, const byte *src, int srcPitch, int w, int h, uint8 bitDepth) {
	assert(w > 0);
	assert(h > 0);
	assert(src != nullptr);
	assert(dst != nullptr);

	if ((w * bitDepth == srcPitch) && (w * bitDepth == dstPitch)) {
		memcpy(dst, src, w * h * bitDepth);
	} else {
		do {
			memcpy(dst, src, w * bitDepth);
			dst += dstPitch;
			src += srcPitch;
		} while (--h);
	}
}

static void fill(byte *dst, int dstPitch, uint16 color, int w, int h, uint8 bitDepth) {
	assert(h > 0);
	assert(dst != nullptr);

	if (bitDepth == 2) {
		do {
			for (int i = 0; i < w; i++)
				WRITE_UINT16(dst + i * 2, color);
			dst += dstPitch;
		} while (--h);
	} else {
		if (w == dstPitch) {
			memset(dst, color, w * h);
		} else {
			do {
				memset(dst, color, w);
				dst += dstPitch;
			} while (--h);
		}
	}
}

#ifdef USE_ARM_GFX_ASM

#define copy8Col(A,B,C,D,E) asmCopy8Col(A,B,C,D,E)

#else

static void copy8Col(byte *dst, int dstPitch, const byte *src, int height, uint8 bitDepth) {

	do {
#if defined(SCUMM_NEED_ALIGNMENT)
		memcpy(dst, src, 8 * bitDepth);
#else
		((uint32 *)dst)[0] = ((const uint32 *)src)[0];
		((uint32 *)dst)[1] = ((const uint32 *)src)[1];
		if (bitDepth == 2) {
			((uint32 *)dst)[2] = ((const uint32 *)src)[2];
			((uint32 *)dst)[3] = ((const uint32 *)src)[3];
		}
#endif
		dst += dstPitch;
		src += dstPitch;
	} while (--height);
}

#endif /* USE_ARM_GFX_ASM */

static void clear8Col(byte *dst, int dstPitch, int height, uint8 bitDepth) {
	do {
#if defined(SCUMM_NEED_ALIGNMENT)
		if (g_scumm->_game.platform == Common::kPlatformNES)
			memset(dst, 0x1d, 8 * bitDepth);
		else
			memset(dst, 0, 8 * bitDepth);
#else
		if (g_scumm->_game.platform == Common::kPlatformNES) {
			memset(dst, 0x1d, 8 * bitDepth);
		} else {
			((uint32*)dst)[0] = 0;
			((uint32*)dst)[1] = 0;
			if (bitDepth == 2) {
				((uint32*)dst)[2] = 0;
				((uint32*)dst)[3] = 0;
			}
		}
#endif
		dst += dstPitch;
	} while (--height);
}

void ScummEngine::drawBox(int x, int y, int x2, int y2, int color) {
	int width, height;
	VirtScreen *vs;
	byte *backbuff, *bgbuff;

	if ((vs = findVirtScreen(y)) == nullptr)
		return;

	if (_game.version == 8) {
		width = _screenWidth + 8;
		height = _screenHeight;
		int effX2 = x2;
		int effX;
		if (width >= x2) {
			effX = x;
		} else {
			effX2 = width;
			effX = x;
			if (x < 0)
				effX = 0;
		}
		backbuff = vs->getPixels(effX, y + _screenTop);
		fill(backbuff, vs->pitch, color, effX2, y2, vs->format.bytesPerPixel);
		markRectAsDirty(vs->number, effX, effX + effX2, y + _screenTop, y + y2 + _screenTop);
		return;
	}

	// Indy4 Amiga always uses the room or verb palette map to match colors to
	// the currently setup palette, thus we need to select it over here too.
	// Done like the original interpreter.
	if (_game.platform == Common::kPlatformAmiga && _game.id == GID_INDY4) {
		if (vs->number == kVerbVirtScreen)
			color = _verbPalette[color];
		else
			color = _roomPalette[color];
	}

	if (x > x2)
		SWAP(x, x2);

	if (y > y2)
		SWAP(y, y2);

	x2++;
	y2++;

	// Adjust for the topline of the VirtScreen
	y -= vs->topline;
	y2 -= vs->topline;

	// Clip the coordinates
	if (x < 0)
		x = 0;
	else if (x >= vs->w)
		return;

	if (x2 < 0)
		return;
	else if (x2 > vs->w)
		x2 = vs->w;

	if (y < 0)
		y = 0;
	else if (y > vs->h)
		return;

	if (y2 < 0)
		return;
	else if (y2 > vs->h)
		y2 = vs->h;

	width = x2 - x;
	height = y2 - y;

	// This will happen in the Sam & Max intro - see bug #1797 - where
	// it would trigger an assertion in blit().

	if (width <= 0 || height <= 0)
		return;

#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
	// Some FM-Towns games draw directly on layer 2, without setting the virtscreen dirty, bypassing the normal drawing
	// routines. It can make a difference, e. g. bug no. 15027 ("INDY3 (FMTowns): Map lines are drawn incorrectly, plus
	// more issues when leaving Germany"). Making the virtscreen dirty, would cause some wrong colors, due to the way the
	// scripts handle the shadow palette there.
	if (!_townsScreen || _game.platform != Common::kPlatformFMTowns || _game.version != 3 || vs->number == kTextVirtScreen)
#endif
		markRectAsDirty(vs->number, x, x2, y, y2);

	backbuff = vs->getPixels(x, y);
	bgbuff = vs->getBackPixels(x, y);

	// A check for -1 might be wrong in all cases since o5_drawBox() in its current form
	// is definitely not capable of passing a parameter of -1 (color range is 0 - 255).
	// Just to make sure I don't break anything I restrict the code change to FM-Towns
	// version 5 games where this change is necessary to fix certain long standing bugs.
	if (color == -1
#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
		|| (color >= 254 && _game.platform == Common::kPlatformFMTowns && (_game.id == GID_MONKEY2 || _game.id == GID_INDY4))
#endif
		) {
#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
		if (_game.platform == Common::kPlatformFMTowns) {
			if (color == 254)
				towns_setupPalCycleField(x, y, x2, y2);
		} else
#endif
		{
			if (vs->number != kMainVirtScreen)
				error("can only copy bg to main window");

			blit(backbuff, vs->pitch, bgbuff, vs->pitch, width, height, vs->format.bytesPerPixel);
			if (_charset->_hasMask) {
				byte *mask = (byte *)_textSurface.getBasePtr(x * _textSurfaceMultiplier, (y - _screenTop) * _textSurfaceMultiplier);
				fill(mask, _textSurface.pitch, CHARSET_MASK_TRANSPARENCY, width * _textSurfaceMultiplier, height * _textSurfaceMultiplier, _textSurface.format.bytesPerPixel);
			}
		}
	} else if (_game.heversion >= 72) {
		// Flags are used for different methods in HE games
		uint32 colorToBack         = (!(_game.heversion > 99 || _isHE995)) ? 0x8000 : 0x1000000;
		uint32 colorCopyForeToBack = (!(_game.heversion > 99 || _isHE995)) ? 0x4000 : 0x2000000;
		uint32 colorCopyBackToFore = (!(_game.heversion > 99 || _isHE995)) ? 0x2000 : 0x4000000;

		uint32 flags = color;
		if (flags & colorCopyBackToFore) {
			blit(backbuff, vs->pitch, bgbuff, vs->pitch, width, height, vs->format.bytesPerPixel);
		} else if (flags & colorCopyForeToBack) {
			blit(bgbuff, vs->pitch, backbuff, vs->pitch, width, height, vs->format.bytesPerPixel);
		} else if (flags & colorToBack) {
			flags &= (flags & 0x1000000) ? 0xFFFFFF : 0x7FFF;
			fill(backbuff, vs->pitch, flags, width, height, vs->format.bytesPerPixel);
			fill(bgbuff, vs->pitch, flags, width, height, vs->format.bytesPerPixel);
		} else {
			fill(backbuff, vs->pitch, flags, width, height, vs->format.bytesPerPixel);
		}
	} else if (_game.heversion >= 60) {
		// Flags are used for different methods in HE games
		uint16 flags = color;
		if (flags & 0x2000) {
			blit(backbuff, vs->pitch, bgbuff, vs->pitch, width, height, vs->format.bytesPerPixel);
		} else if (flags & 0x4000) {
			blit(bgbuff, vs->pitch, backbuff, vs->pitch, width, height, vs->format.bytesPerPixel);
		} else if (flags & 0x8000) {
			flags &= 0x7FFF;
			fill(backbuff, vs->pitch, flags, width, height, vs->format.bytesPerPixel);
			fill(bgbuff, vs->pitch, flags, width, height, vs->format.bytesPerPixel);
		} else {
			fill(backbuff, vs->pitch, flags, width, height, vs->format.bytesPerPixel);
		}
	} else {
		if (_game.features & GF_16BIT_COLOR) {
			fill(backbuff, vs->pitch, _16BitPalette[color], width, height, vs->format.bytesPerPixel);
		} else {
#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
			if (_game.platform == Common::kPlatformFMTowns) {
				if (_game.version == 3 && vs->number != kTextVirtScreen) {
					// The original FM-Towns v3 interpreter overdraws both width
					// and height by 1 pixel. I don't know if it is voluntary or
					// not, so I have added safety checks.
					if (x + width < vs->w)
						width++;
					if (y + height < vs->h)
						height++;
				}

				color = ((color & 0x0f) << 4) | (color & 0x0f);
				// Some FM-Towns games draw directly on layer 2, without setting the virtscreeb dirty, bypassing the
				// normal drawing routines. It can make a difference, e. g. bug no. 15027 ("INDY3 (FMTowns): Map lines
				// are drawn incorrectly, plus more issues when leaving Germany"). Making the virtscreen dirty, would
				// cause some wrong colors, due to the way the scripts handle the shadow palette there.
				if (_game.version == 3 && vs->number != kTextVirtScreen) {
					towns_fillTopLayerRect(x * _textSurfaceMultiplier,
						(y - _screenTop + vs->topline) * _textSurfaceMultiplier, width * _textSurfaceMultiplier, height * _textSurfaceMultiplier, color);
					// We still need to continue and make the same changes to the textSurface (the original FM-Towns
					// SCUMM3 interpreters don't have that, everything goes directly onto screen layer 2 there).
				}

				if (vs->number == kBannerVirtScreen) {
					byte *mask = _virtscr[kBannerVirtScreen].getPixels(x, y);
					fill(mask, vs->pitch, color, width * _textSurfaceMultiplier, height * _textSurfaceMultiplier, vs->format.bytesPerPixel);
				} else {
					byte *mask = (byte *)_textSurface.getBasePtr(x * _textSurfaceMultiplier, (y - _screenTop + vs->topline) * _textSurfaceMultiplier);
					fill(mask, _textSurface.pitch, color, width * _textSurfaceMultiplier, height * _textSurfaceMultiplier, _textSurface.format.bytesPerPixel);
				}

				if (_game.id != GID_MONKEY && !(_game.version == 3 && vs->number == kTextVirtScreen))
					return;
			}
#endif

			if (_macScreen) {
				byte *mask = (byte *)_textSurface.getBasePtr(x * _textSurfaceMultiplier, (y - _screenTop + vs->topline) * _textSurfaceMultiplier);
				fill(mask, _textSurface.pitch, CHARSET_MASK_TRANSPARENCY, width * _textSurfaceMultiplier, height * _textSurfaceMultiplier, _textSurface.format.bytesPerPixel);
			}

			fill(backbuff, vs->pitch, color, width, height, vs->format.bytesPerPixel);
		}
	}
}

void ScummEngine::drawLine(int x1, int y1, int x2, int y2, int color) {
	if ((_game.platform == Common::kPlatformFMTowns && _game.version == 5) ||
		(_game.platform == Common::kPlatformMacintosh && _game.version > 3)) {
		drawBox(x1, _screenTop + y1, x2, _screenTop + y2, color);
		return;
	}

	int effColor, black, white;
	int effX1, effY1;
	int width, height, widthAccumulator, heightAccumulator, horizontalStrips, originalHeight;
	int nudgeX, nudgeY;

	bool canDrawPixel, noColorSpecified;

	VirtScreen *vs;

	if ((vs = findVirtScreen(y1)) == nullptr)
		return;

	black = getPaletteColorFromRGB(_currentPalette, 0x00, 0x00, 0x00);
	white = getPaletteColorFromRGB(_currentPalette, 0xFC, 0xFC, 0xFC);

	noColorSpecified = false;
	effColor = color;
	if (color == -1) {
		noColorSpecified = true;
		effColor = white;
	}

	effX1 = x1;
	effY1 = y1;
	width = abs(x2 - x1);
	height = abs(y2 - y1);
	originalHeight = height;

	if (height <= width)
		height = width;

	widthAccumulator = 0;
	heightAccumulator = 0;

	drawPixel(vs, x1, y1, effColor);

	if (height >= 0) {
		horizontalStrips = height + 1;
		do {
			widthAccumulator += width;
			canDrawPixel = false;
			heightAccumulator += originalHeight;
			if (widthAccumulator > height) {
				canDrawPixel = true;
				widthAccumulator -= height;
				nudgeX = 1;
				if (x2 - x1 < 0)
					nudgeX = -1;
				effX1 += nudgeX;
			}

			if (heightAccumulator > height) {
				canDrawPixel = true;
				heightAccumulator -= height;
				nudgeY = 1;
				if (y2 - y1 < 0)
					nudgeY = -1;
				effY1 += nudgeY;
			}

			if (canDrawPixel) {
				drawPixel(vs, effX1, effY1, effColor);
				if (noColorSpecified) {
					if (effColor != white)
						effColor = white;
					else
						effColor = black;
				}
			}

			horizontalStrips--;
		} while (horizontalStrips);
	}
}

void ScummEngine::drawPixel(VirtScreen *vs, int x, int y, int16 color, bool useBackbuffer) {
	int factor = _isIndy4Jap ? 0 : 8;
	int wScale = (vs->number == kBannerVirtScreen && _textSurfaceMultiplier == 2) ? 2 : 1;
	if (x >= 0 && y >= 0 && _screenWidth + factor > x && _screenHeight > y) {
		if (useBackbuffer) {
			*(vs->getBackPixels(x, y + _screenTop - vs->topline)) = color;
		} else {
			// Is it elegant to do the kBannerVirtScreen horizontal scaling here like this? Certainly not,
			// but it is just what the original interpreter does. So it will at least not break anything.
			for (int i = 0; i < wScale; ++i)
				*(vs->getPixels(x * wScale + i, y + _screenTop - vs->topline)) = color;
		}
		markRectAsDirty(vs->number, x * wScale, (x + 1) * wScale, y + _screenTop - vs->topline, y + 1 + _screenTop - vs->topline);
	}
}

/**
 * Moves the screen content by the offset specified via dx/dy.
 * Only the region from x=0 till x=height-1 is affected.
 * @param dx	the horizontal offset.
 * @param dy	the vertical offset.
 * @param height	the number of lines which in which the move will be done.
 */
void ScummEngine::moveScreen(int dx, int dy, int height) {
	// Short circuit check - do we have to do anything anyway?
	if ((dx == 0 && dy == 0) || height <= 0)
		return;

	Graphics::Surface *screen = _system->lockScreen();
	if (!screen)
		return;


	if (_macScreen) {
		screen->move(dx, dy, height + _macScreenDrawOffset * 2);

		// Mask the empty part of the screen
		screen->fillRect(Common::Rect(0, 0, screen->pitch, _macScreenDrawOffset * 2), 0);
		screen->fillRect(Common::Rect(0, screen->h - _macScreenDrawOffset * 2, screen->pitch, screen->h), 0);
	} else {
		screen->move(dx, dy, height);
	}

	_system->unlockScreen();
}

void ScummEngine_v5::clearFlashlight() {
	_flashlight.eraseFlag = false;
	_flashlight.buffer = nullptr;
}

static const byte townsCurveData[] = { 0x01, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0x7F, 0xFF };

static const byte v1FwdCurveData[] = {
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0xFF, 0xFF,
	0x00, 0x00, 0xFF, 0xFF,
	0x00, 0xFF, 0xFF, 0xFF,
	0x00, 0xFF, 0xFF, 0xFF,
	0x00, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF
};

static const byte v1BkwdCurveData[] = {
	0xFF, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0x00, 0x00,
	0xFF, 0xFF, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0x00,
	0xFF, 0xFF, 0xFF, 0x00,
	0xFF, 0xFF, 0xFF, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF
};


static const byte v2FwdCurveData[] = {
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x0F, 0xFF,
	0x00, 0x00, 0xFF, 0xFF,
	0x00, 0x0F, 0xFF, 0xFF,
	0x00, 0xFF, 0xFF, 0xFF,
	0x0F, 0xFF, 0xFF, 0xFF,
	0x0F, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF
};

static const byte v2BkwdCurveData[] = {
	0x00, 0x00, 0x00, 0x00,
	0xFF, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0x00, 0x00,
	0xFF, 0xFF, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0x00,
	0xFF, 0xFF, 0xFF, 0x00,
	0xFF, 0xFF, 0xFF, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF
};

static const byte v4FwdCurveData[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static const byte v4BkwdCurveData[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

void ScummEngine_v5::drawFlashlight() {
	int x, y;
	VirtScreen *vs = &_virtscr[kMainVirtScreen];
	byte blackColor = 0x00;

	// NES uses 0x1d for black
	if (g_scumm->_game.platform == Common::kPlatformNES)
		blackColor = 0x1D;

	// Remove the flash light first if it was previously drawn
	if (_flashlight.eraseFlag) {
		if (_flashlight.buffer) {
			fill(_flashlight.buffer, vs->pitch, blackColor, _flashlight.w, _flashlight.h, vs->format.bytesPerPixel);
		}

		markRectAsDirty(kMainVirtScreen, _flashlight.x, _flashlight.x + _flashlight.w,
				_flashlight.y, _flashlight.y + _flashlight.h, USAGE_BIT_DIRTY);

		_flashlight.eraseFlag = false;
	}

	if (_flashlight.xStrips == 0 || _flashlight.yStrips == 0)
		return;

	// Calculate the position of the flashlight.
	if (_game.id == GID_ZAK || _game.id == GID_MANIAC) {
		x = (_mouse.x + vs->xstart);
		y = (_mouse.y - vs->topline);
	} else {
		Actor *a = derefActor(VAR(VAR_EGO), "drawFlashlight");
		x = a->getPos().x;
		y = a->getPos().y;
	}

	// The original only shows the flashlight in locations whose:
	// - X position is a multiple of 8;
	// - Y position is a multiple of 2.
	//
	// FM-Towns doesn't seem to do so...
	if (_game.platform != Common::kPlatformFMTowns) {
		x &= ~7;
		y &= ~1;
	}

	_flashlight.w = _flashlight.xStrips * 8;
	_flashlight.h = _flashlight.yStrips * 8;
	_flashlight.x = x - _flashlight.w / 2 - _screenStartStrip * 8;
	_flashlight.y = y - _flashlight.h / 2;

	if (_game.id == GID_LOOM && _game.version == 3 && _game.platform != Common::kPlatformFMTowns) {
		_flashlight.x += 4;
		_flashlight.y -= 12;
	}

	// Clip the flashlight at the borders
	if (_flashlight.x < 0)
		_flashlight.x = 0;
	else if (_flashlight.x + _flashlight.w > _gdi->_numStrips * 8)
		_flashlight.x = _gdi->_numStrips * 8 - _flashlight.w;
	if (_flashlight.y < 0)
		_flashlight.y = 0;
	else if (_flashlight.y + _flashlight.h> vs->h)
		_flashlight.y = vs->h - _flashlight.h;

	// Redraw any actors "under" the flashlight
	for (int i = _flashlight.x / 8; i < (_flashlight.x + _flashlight.w) / 8; i++) {
		assert(0 <= i && i < _gdi->_numStrips);
		setGfxUsageBit(_screenStartStrip + i, USAGE_BIT_DIRTY);
		vs->tdirty[i] = 0;
		vs->bdirty[i] = vs->h;
	}

	byte *bgbak;
	_flashlight.buffer = vs->getPixels(_flashlight.x, _flashlight.y);
	bgbak = vs->getBackPixels(_flashlight.x, _flashlight.y);

	blit(_flashlight.buffer, vs->pitch, bgbak, vs->pitch, _flashlight.w, _flashlight.h, vs->format.bytesPerPixel);

	// Apple IIGS, C64 & NES does not round the flashlight
	if (_game.platform != Common::kPlatformApple2GS && _game.platform != Common::kPlatformC64 && _game.platform != Common::kPlatformNES) {
		// Round the corners. Different versions have different rounding parameters.
		if (vs->format.bytesPerPixel == 1) {
			int width, height, heightLoc;
			byte maskValue;
			bool isIndy3VGA = (_game.id == GID_INDY3 && (_game.features & GF_OLD256));

			height = _flashlight.h - 1;

			byte *buffPtr = _flashlight.buffer;

			if (_game.platform == Common::kPlatformFMTowns) {
				for (int i = 0; i < 8; ++i) {
					heightLoc = vs->pitch * height;
					width = _flashlight.w - 1;

					for (byte j = 128, idx = 0; j; j >>= 1, idx++) {
						if ((j & townsCurveData[i]) != 0)
							maskValue = 0xFF; // Pixel ON
						else
							maskValue = 0x00; // Pixel OFF

						buffPtr[idx] &= maskValue;
						buffPtr[idx + heightLoc] &= maskValue;
						buffPtr[idx + width] &= maskValue;
						buffPtr[idx + width + heightLoc] &= maskValue;

						width -= 2;
					}

					buffPtr += vs->pitch;
					height -= 2;
				}
			} else {
				const byte *fwdCurvePtr, *bkwdCurvePtr;

				switch (_game.version) {
				case 1:
					fwdCurvePtr = v1FwdCurveData;
					bkwdCurvePtr = v1BkwdCurveData;
					break;
				case 2:
					fwdCurvePtr = v2FwdCurveData;
					bkwdCurvePtr = v2BkwdCurveData;
					break;
				case 3:
					if (isIndy3VGA) {
						fwdCurvePtr = v4FwdCurveData;
						bkwdCurvePtr = v4BkwdCurveData;
					} else {
						fwdCurvePtr = v2FwdCurveData;
						bkwdCurvePtr = v2BkwdCurveData;
					}

					break;
				default:
					fwdCurvePtr = bkwdCurvePtr = v4FwdCurveData;
				}

				if (_game.version <= 3 && !isIndy3VGA) {
					width = _flashlight.w - 8;

					for (int i = 8, curveHorizLine = 0; i > 0; i--, curveHorizLine += 4) {
						heightLoc = vs->pitch * height;

						// v1 and v2 use a nibble mask on an half resolution buffer! ARGH!
						// We have to double the maskings in order to match the original,
						// without handling an half-res flashlight buffer...
						for (byte j = 0, ptInCurveHorizLine = 0; j < 8; j += 2, ptInCurveHorizLine++) {
							// Top left
							buffPtr[j]     &= (fwdCurvePtr[ptInCurveHorizLine + curveHorizLine] & 0xF0) ? 0xFF : 0x00;
							buffPtr[j + 1] &= (fwdCurvePtr[ptInCurveHorizLine + curveHorizLine] & 0x0F) ? 0xFF : 0x00;

							// Bottom left
							buffPtr[j + heightLoc]     &= (fwdCurvePtr[ptInCurveHorizLine + curveHorizLine] & 0xF0) ? 0xFF : 0x00;
							buffPtr[j + heightLoc + 1] &= (fwdCurvePtr[ptInCurveHorizLine + curveHorizLine] & 0x0F) ? 0xFF : 0x00;

							// Top right
							buffPtr[j + width]     &= (bkwdCurvePtr[ptInCurveHorizLine + curveHorizLine] & 0x0F) ? 0xFF : 0x00;
							buffPtr[j + width + 1] &= (bkwdCurvePtr[ptInCurveHorizLine + curveHorizLine] & 0xF0) ? 0xFF : 0x00;

							// Bottom right
							buffPtr[j + width + heightLoc]     &= (bkwdCurvePtr[ptInCurveHorizLine + curveHorizLine] & 0x0F) ? 0xFF : 0x00;
							buffPtr[j + width + heightLoc + 1] &= (bkwdCurvePtr[ptInCurveHorizLine + curveHorizLine] & 0xF0) ? 0xFF : 0x00;
						}

						height -= 2;
						buffPtr += vs->pitch;
					}
				} else {
					byte maskValueFwd, maskValueBkwd;

					for (int i = 0; i < 8; ++i) {
						heightLoc = vs->pitch * height;

						width = _flashlight.w - 8;

						for (byte j = 8, idx = 0; j; j--, idx++) {
							maskValueFwd = fwdCurvePtr[i * 8 + idx];
							maskValueBkwd = isIndy3VGA ? bkwdCurvePtr[i * 8 + idx] : bkwdCurvePtr[i * 8 + j - 1];

							buffPtr[idx] &= maskValueFwd; // Top left
							buffPtr[idx + heightLoc] &= maskValueFwd; // Bottom left
							buffPtr[idx + width] &= maskValueBkwd; // Top right
							buffPtr[idx + width + heightLoc] &= maskValueBkwd; // Bottom right
						}

						buffPtr += vs->pitch;
						height -= 2;
					}
				}
			}
		} else {
			// The bytesPerPixel == 2 case should only happen for the PC-Engine version of Loom...
			// I'd rather avoid attempting to reverse what happens here in the original and just
			// use the old code...

			static const int corner_data[] = { 8, 6, 4, 3, 2, 2, 1, 1 };
			int minrow = 0;
			int maxcol = (_flashlight.w - 1) * vs->format.bytesPerPixel;
			int maxrow = (_flashlight.h - 1) * vs->pitch;

			for (int i = 0; i < 8; i++, minrow += vs->pitch, maxrow -= vs->pitch) {
				int d = corner_data[i];

				for (int j = 0; j < d; j++) {
					WRITE_UINT16(&_flashlight.buffer[minrow + 2 * j], 0);
					WRITE_UINT16(&_flashlight.buffer[minrow + maxcol - 2 * j], 0);
					WRITE_UINT16(&_flashlight.buffer[maxrow + 2 * j], 0);
					WRITE_UINT16(&_flashlight.buffer[maxrow + maxcol - 2 * j], 0);
				}
			}
		}
	}

	// Not in the original, but this avoids glitches on the borders of the flashlight, since
	// otherwise the next rects refresh would have been on the next drawFlashlight() call...
	markRectAsDirty(kMainVirtScreen, _flashlight.x, _flashlight.x + _flashlight.w,
					_flashlight.y, _flashlight.y + _flashlight.h, USAGE_BIT_DIRTY);

	_flashlight.eraseFlag = true;
}

int ScummEngine_v0::getCurrentLights() const {
	// V0 Maniac doesn't have a ScummVar for VAR_CURRENT_LIGHTS, and just uses
	// an internal variable. Emulate this to prevent overwriting script vars...
	// And V6 games do not use the "lights" at all. There, the whole screen is
	// always visible, and actors are always colored, so we fake the correct
	// light value for it.
	return _currentLights;
}

int ScummEngine::getCurrentLights() const {
	if (_game.version >= 6)
		return LIGHTMODE_room_lights_on | LIGHTMODE_actor_use_colors;
	else
		return VAR(VAR_CURRENT_LIGHTS);
}

bool ScummEngine::isLightOn() const {
	return (getCurrentLights() & LIGHTMODE_room_lights_on) != 0;
}

void ScummEngine::setShake(int mode) {
	if (_shakeEnabled != (mode != 0))
		_fullRedraw = true;

	_shakeEnabled = mode != 0;
	_shakeFrame = 0;
	_system->setShakePos(0, 0);
}

#pragma mark -
#pragma mark --- Image drawing ---
#pragma mark -


void Gdi::prepareDrawBitmap(const byte *ptr, VirtScreen *vs,
					const int x, const int y, const int width, const int height,
	                int stripnr, int numstrip) {
	// Do nothing by default
}

void GdiHE::prepareDrawBitmap(const byte *ptr, VirtScreen *vs,
					const int x, const int y, const int width, const int height,
	                int stripnr, int numstrip) {
	if (_vm->_game.heversion >= 72) {
		_tmskPtr = _vm->findResource(MKTAG('T','M','S','K'), ptr);
	} else
		_tmskPtr = nullptr;
}

void GdiV1::prepareDrawBitmap(const byte *ptr, VirtScreen *vs,
					const int x, const int y, const int width, const int height,
	                int stripnr, int numstrip) {
	if (_objectMode) {
		decodeV1Gfx(ptr, _V1.objectMap, (width / 8) * (height / 8) * 3);
	}
}

void GdiNES::prepareDrawBitmap(const byte *ptr, VirtScreen *vs,
					const int x, const int y, const int width, const int height,
	                int stripnr, int numstrip) {
	if (_objectMode) {
		decodeNESObject(ptr, x - stripnr, y, width, height);
	}
}

#ifdef USE_RGB_COLOR
void GdiPCEngine::prepareDrawBitmap(const byte *ptr, VirtScreen *vs,
					const int x, const int y, const int width, const int height,
	                int stripnr, int numstrip) {
	if (_objectMode) {
		decodePCEngineObject(ptr, x - stripnr, y, width, height);
	}
}
#endif

void GdiV2::prepareDrawBitmap(const byte *ptr, VirtScreen *vs,
					const int x, const int y, const int width, const int height,
	                int stripnr, int numstrip) {
	//
	// Since V3, all graphics data was encoded in strips, which is very efficient
	// for redrawing only parts of the screen. However, V2 is different: here
	// the whole graphics are encoded as one big chunk. That makes it rather
	// difficult to draw only parts of a room/object. We handle the V2 graphics
	// differently from all other (newer) graphic formats for this reason.
	//
	StripTable *table = (_objectMode ? nullptr : _roomStrips);
	const int left = (stripnr * 8);
	const int right = left + (numstrip * 8);
	byte *dst;
	byte *mask_ptr;
	const byte *src;
	byte color, data = 0;
	int run;
	bool dither = false;
	byte dither_table[128];
	byte *ptr_dither_table;
	int theX, theY, maxX;

	memset(dither_table, 0, sizeof(dither_table));

	if (vs->hasTwoBuffers)
		dst = vs->backBuf + y * vs->pitch + x * 8;
	else
		dst = (byte *)vs->getBasePtr(x * 8, y);

	mask_ptr = getMaskBuffer(x, y, 1);


	if (table) {
		run = table->run[stripnr];
		color = table->color[stripnr];
		src = ptr + table->offsets[stripnr];
		theX = left;
		maxX = right;
	} else {
		run = 1;
		color = 0;
		src = ptr;
		theX = 0;
		maxX = width;
	}

	// Decode and draw the image data.
	assert(height <= 128);
	for (; theX < maxX; theX++) {
		ptr_dither_table = dither_table;
		for (theY = 0; theY < height; theY++) {
			if (--run == 0) {
				data = *src++;
				if (data & 0x80) {
					run = data & 0x7f;
					dither = true;
				} else {
					run = data >> 4;
					dither = false;
				}
				color = _roomPalette[data & 0x0f];
				if (run == 0) {
					run = *src++;
				}
			}
			if (!dither) {
				*ptr_dither_table = color;
			}
			if (left <= theX && theX < right) {
				*dst = *ptr_dither_table++;
				dst += vs->pitch;
			}
		}
		if (left <= theX && theX < right) {
			dst -= _vertStripNextInc;
		}
	}


	// Draw mask (zplane) data
	theY = 0;

	if (table) {
		src = ptr + table->zoffsets[stripnr];
		run = table->zrun[stripnr];
		theX = left;
	} else {
		run = *src++;
		theX = 0;
	}
	while (theX < right) {
		const byte runFlag = run & 0x80;
		if (runFlag) {
			run &= 0x7f;
			data = *src++;
		}
		do {
			if (!runFlag)
				data = *src++;

			if (left <= theX) {
				*mask_ptr = data;
				mask_ptr += _numStrips;
			}
			theY++;
			if (theY >= height) {
				if (left <= theX) {
					mask_ptr -= _numStrips * height - 1;
				}
				theY = 0;
				theX += 8;
				if (theX >= right)
					break;
			}
		} while (--run);
		run = *src++;
	}
}

int Gdi::getZPlanes(const byte *ptr, const byte *zplane_list[9], bool bmapImage) const {
	int numzbuf;
	int i;

	if ((_vm->_game.features & GF_SMALL_HEADER) || _vm->_game.version == 8)
		zplane_list[0] = ptr;
	else if (bmapImage)
		zplane_list[0] = _vm->findResource(MKTAG('B','M','A','P'), ptr);
	else
		zplane_list[0] = _vm->findResource(MKTAG('S','M','A','P'), ptr);

	if (_zbufferDisabled)
		numzbuf = 0;
	else if (_numZBuffer <= 1 || (_vm->_game.version <= 2))
		numzbuf = _numZBuffer;
	else {
		numzbuf = _numZBuffer;
		assert(numzbuf <= 9);

		if (_vm->_game.id == GID_LOOM && _vm->_game.platform == Common::kPlatformPCEngine) {
			zplane_list[1] = nullptr;
		} else if (_vm->_game.features & GF_SMALL_HEADER) {
			if (_vm->_game.features & GF_16COLOR)
				zplane_list[1] = ptr + READ_LE_UINT16(ptr);
			else {
				zplane_list[1] = ptr + READ_LE_UINT32(ptr);
				if (_vm->_game.features & GF_OLD256) {
					if (0 == READ_LE_UINT32(zplane_list[1]))
						zplane_list[1] = nullptr;
				}
			}
			for (i = 2; i < numzbuf; i++) {
				zplane_list[i] = zplane_list[i-1] + READ_LE_UINT16(zplane_list[i-1]);
			}
		} else if (_vm->_game.version == 8) {
			// Find the OFFS chunk of the ZPLN chunk
			const byte *zplnOffsChunkStart = ptr + 24 + READ_BE_UINT32(ptr + 12);

			// Each ZPLN contains a WRAP chunk, which has (as always) an OFFS subchunk pointing
			// at ZSTR chunks. These once more contain a WRAP chunk which contains nothing but
			// an OFFS chunk. The content of this OFFS chunk contains the offsets to the
			// Z-planes.
			// We do not directly make use of this, but rather hard code offsets (like we do
			// for all other Scumm-versions, too). Clearly this is a bit hackish, but works
			// well enough, and there is no reason to assume that there are any cases where it
			// might fail. Still, doing this properly would have the advantage of catching
			// invalid/damaged data files, and allow us to exit gracefully instead of segfaulting.
			for (i = 1; i < numzbuf; i++) {
				zplane_list[i] = zplnOffsChunkStart + READ_LE_UINT32(zplnOffsChunkStart + 4 + i*4) + 16;
			}
		} else {
			const uint32 zplane_tags[] = {
				MKTAG('Z','P','0','0'),
				MKTAG('Z','P','0','1'),
				MKTAG('Z','P','0','2'),
				MKTAG('Z','P','0','3'),
				MKTAG('Z','P','0','4')
			};

			for (i = 1; i < numzbuf; i++) {
				zplane_list[i] = _vm->findResource(zplane_tags[i], ptr);
			}
		}
	}

	return numzbuf;
}

/**
 * Draw a bitmap onto a virtual screen. This is main drawing method for room backgrounds
 * and objects, used throughout all SCUMM versions.
 */
void Gdi::drawBitmap(const byte *ptr, VirtScreen *vs, int x, const int y, const int width, const int height,
					int stripnr, int numstrip, byte flag) {
	assert(ptr);
	assert(height > 0);

	byte *dstPtr;
	const byte *smap_ptr;
	const byte *zplane_list[9];
	int numzbuf;
	int sx;
	bool transpStrip = false;

	// Check whether lights are turned on or not
	const bool lightsOn = _vm->isLightOn();

	if ((_vm->_game.features & GF_SMALL_HEADER) || _vm->_game.version == 8) {
		smap_ptr = ptr;
	} else {
		smap_ptr = _vm->findResource(MKTAG('S','M','A','P'), ptr);
		assert(smap_ptr);
	}

	numzbuf = getZPlanes(ptr, zplane_list, false);

	if (y + height > vs->h) {
		warning("Gdi::drawBitmap, strip drawn to %d below window bottom %d", y + height, vs->h);
	}

#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
	if (_vm->_townsPaletteFlags & 2) {
		int cx = (x - _vm->_screenStartStrip) << 3;
		_vm->_textSurface.fillRect(Common::Rect(cx * _vm->_textSurfaceMultiplier, y * _vm->_textSurfaceMultiplier, (cx  + width - 1) * _vm->_textSurfaceMultiplier, (y + height - 1) * _vm->_textSurfaceMultiplier), 0);
	}
#endif

	_vertStripNextInc = height * vs->pitch - 1 * vs->format.bytesPerPixel;

	_objectMode = (flag & dbObjectMode) == dbObjectMode;
	prepareDrawBitmap(ptr, vs, x, y, width, height, stripnr, numstrip);

	sx = x - vs->xstart / 8;
	if (sx < 0) {
		numstrip -= -sx;
		x += -sx;
		stripnr += -sx;
		sx = 0;
	}

	// Compute the number of strips we have to iterate over.
	// TODO/FIXME: The computation of its initial value looks very fishy.
	// It was added as a kind of hack to fix some corner cases, but it compares
	// the room width to the virtual screen width; but the former should always
	// be bigger than the latter (except for MM NES, maybe)... strange
	int limit = MAX(_vm->_roomWidth, (int)vs->w) / 8 - x;
	if (limit > numstrip)
		limit = numstrip;
	if (limit > _numStrips - sx)
		limit = _numStrips - sx;
	for (int k = 0; k < limit; ++k, ++stripnr, ++sx, ++x) {
		if (y < vs->tdirty[sx])
			vs->tdirty[sx] = y;

		if (y + height > vs->bdirty[sx])
			vs->bdirty[sx] = y + height;

		// In the case of a double buffered virtual screen, we draw to
		// the backbuffer, otherwise to the primary surface memory.
		if (vs->hasTwoBuffers)
			dstPtr = vs->backBuf + y * vs->pitch + (x * 8 * vs->format.bytesPerPixel);
		else
			dstPtr = (byte *)vs->getBasePtr(x * 8, y);

		transpStrip = drawStrip(dstPtr, vs, x, y, width, height, stripnr, smap_ptr);

		// COMI and HE games only uses flag value
		if (_vm->_game.version == 8 || _vm->_game.heversion >= 60)
			transpStrip = true;

		if (vs->hasTwoBuffers) {
			byte *frontBuf = (byte *)vs->getBasePtr(x * 8, y);
			if (lightsOn)
				copy8Col(frontBuf, vs->pitch, dstPtr, height, vs->format.bytesPerPixel);
			else
				clear8Col(frontBuf, vs->pitch, height, vs->format.bytesPerPixel);
		}

		decodeMask(x, y, width, height, stripnr, numzbuf, zplane_list, transpStrip, flag);

#if 0
		// HACK: blit mask(s) onto normal screen. Useful to debug masking
		int bytesPerStrip = (_vm->_game.features & GF_16BIT_COLOR) ? 16 : 8;

		for (int i = 0; i < numzbuf; i++) {

			byte *dst1, *dst2;

			dst1 = dst2 = (byte *)vs->getPixels(0,0) + y * vs->pitch + x * bytesPerStrip;
			if (vs->hasTwoBuffers)
				dst2 = vs->backBuf + y * vs->pitch + x * bytesPerStrip;
			byte *mask_ptr = getMaskBuffer(x, y, i);

			for (int h = 0; h < height; h++) {
				int maskbits = *mask_ptr;
				for (int j = 0; j < 8; j++) {
					if (maskbits & 0x80) {
						if (bytesPerStrip == 16) {
							WRITE_UINT16(dst1 + j * 2, _vm->_16BitPalette[12 + i]);
							WRITE_UINT16(dst2 + j * 2, _vm->_16BitPalette[12 + i]);
						} else
							dst1[j] = dst2[j] = 12 + i;
					}
					maskbits <<= 1;
				}
				dst1 += vs->pitch;
				dst2 += vs->pitch;
				mask_ptr += _numStrips;
			}
		}
#endif
	}
}

bool Gdi::drawStrip(byte *dstPtr, VirtScreen *vs, int x, int y, const int width, const int height,
					int stripnr, const byte *smap_ptr) {
	// Do some input verification and make sure the strip/strip offset
	// are actually valid. Normally, this should never be a problem,
	// but if e.g. a savegame gets corrupted, we can easily get into
	// trouble here. See also bug #1191.
	int offset = -1, smapLen;
	if (_vm->_game.features & GF_16COLOR) {
		smapLen = READ_LE_UINT16(smap_ptr);
		if (stripnr * 2 + 2 < smapLen) {
			offset = READ_LE_UINT16(smap_ptr + stripnr * 2 + 2);
		}
	} else if (_vm->_game.features & GF_SMALL_HEADER) {
		smapLen = READ_LE_UINT32(smap_ptr);
		if (stripnr * 4 + 4 < smapLen)
			offset = READ_LE_UINT32(smap_ptr + stripnr * 4 + 4);
	} else if (_vm->_game.version == 8) {
		smapLen = READ_BE_UINT32(smap_ptr + 4);
		// Skip to the BSTR->WRAP->OFFS chunk
		smap_ptr += 24;
		if (stripnr * 4 + 8 < smapLen)
			offset = READ_LE_UINT32(smap_ptr + stripnr * 4 + 8);
	} else {
		smapLen = READ_BE_UINT32(smap_ptr + 4);
		if (stripnr * 4 + 8 < smapLen)
			offset = READ_LE_UINT32(smap_ptr + stripnr * 4 + 8);
	}
	assertRange(0, offset, smapLen-1, "screen strip");

	// Indy4 Amiga always uses the room or verb palette map to match colors to
	// the currently setup palette, thus we need to select it over here too.
	// Done like the original interpreter.
	if (_vm->_game.platform == Common::kPlatformAmiga && _vm->_game.id == GID_INDY4) {
		if (vs->number == kVerbVirtScreen)
			_roomPalette = _vm->_verbPalette;
		else
			_roomPalette = _vm->_roomPalette;
	}

	// WORKAROUND: 256-color versions of Indy3 feature unusual pink and cyan
	// horizontal lines when Indy meets Elsa in Berlin. This has only been fixed
	// in the official FM-TOWNS release (with a few other subtle adjustments),
	// but a simpler fix here is to override the pink and cyan colors in the local
	// palette so that it matches the way these lines have been redrawn in the
	// FM-TOWNS release.  We take care not to apply this palette change to the
	// text or inventory, as they still require the original colors.
	//
	// Also, the background was redrawn for censorship reasons in the German
	// release, so we have to take care of the different resource size there.
	if (_vm->_game.id == GID_INDY3 && (_vm->_game.features & GF_OLD256) && _vm->_game.platform != Common::kPlatformFMTowns
		&& _vm->_roomResource == 46 && vs->number == kMainVirtScreen
		&& (smapLen == 43159 || (smapLen == 42953 && _vm->_language == Common::DE_DEU))
		&& _vm->enhancementEnabled(kEnhMinorBugFixes)) {
		if (_roomPalette[11] == 11 && _roomPalette[86] == 86)
			_roomPalette[11] = 86;
		if (_roomPalette[13] == 13 && _roomPalette[80] == 80)
			_roomPalette[13] = 80;
	}

	// WORKAROUND: In the French VGA floppy version of MI1, the easter egg
	// poking fun at Sierra has a dark blue background instead of white,
	// which causes legibility issues (similar to the ones with the sign
	// about the dogs "only sleeping", which we already fix elsewhere).
	//
	// The other VGA floppy translations are fine, and the issue was
	// fixed in the French VGA Amiga and CD releases.
	if (_vm->_game.id == GID_MONKEY_VGA &&
			_vm->_language == Common::FR_FRA &&
			_vm->_game.platform != Common::kPlatformAmiga &&
			_vm->_currentRoom == 11 &&
			vs->number == kMainVirtScreen &&
			y == 24 && x >= 28 && x <= 52 && height == 56 &&
			_vm->enhancementEnabled(kEnhVisualChanges)) {
		_roomPalette[1] = 15;

		byte result = decompressBitmap(dstPtr, vs->pitch, smap_ptr + offset, height);

		_roomPalette[1] = 1;
		return result;
	}

	return decompressBitmap(dstPtr, vs->pitch, smap_ptr + offset, height);
}

bool GdiNES::drawStrip(byte *dstPtr, VirtScreen *vs, int x, int y, const int width, const int height,
					int stripnr, const byte *smap_ptr) {
	byte *mask_ptr = getMaskBuffer(x, y, 1);
	drawStripNES(dstPtr, mask_ptr, vs->pitch, stripnr, y, height);

	return false;
}

#ifdef USE_RGB_COLOR
bool GdiPCEngine::drawStrip(byte *dstPtr, VirtScreen *vs, int x, int y, const int width, const int height,
					int stripnr, const byte *smap_ptr) {
	byte *mask_ptr = getMaskBuffer(x, y, 1);
	drawStripPCEngine(dstPtr, mask_ptr, vs->pitch, stripnr, y, height);
	return false;
}
#endif

bool GdiV1::drawStrip(byte *dstPtr, VirtScreen *vs, int x, int y, const int width, const int height,
					int stripnr, const byte *smap_ptr) {
	if (_objectMode)
		drawStripV1Object(dstPtr, vs->pitch, stripnr, width, height);
	else
		drawStripV1Background(dstPtr, vs->pitch, stripnr, height);

	return false;
}

bool GdiV2::drawStrip(byte *dstPtr, VirtScreen *vs, int x, int y, const int width, const int height,
					int stripnr, const byte *smap_ptr) {
	// Do nothing here for V2 games - drawing was already handled.
	return false;
}

void Gdi::decodeMask(int x, int y, const int width, const int height,
	                int stripnr, int numzbuf, const byte *zplane_list[9],
	                bool transpStrip, byte flag) {
	int i;
	byte *mask_ptr;
	const byte *z_plane_ptr;

	if (flag & dbDrawMaskOnAll) {
		// Sam & Max uses dbDrawMaskOnAll for things like the inventory
		// box and the speech icons. While these objects only have one
		// mask, it should be applied to all the Z-planes in the room,
		// i.e. they should mask every actor.
		//
		// This flag used to be called dbDrawMaskOnBoth, and all it
		// would do was to mask Z-plane 0. (Z-plane 1 would also be
		// masked, because what is now the else-clause used to be run
		// always.) While this seems to be the only way there is to
		// mask Z-plane 0, this wasn't good enough since actors in
		// Z-planes >= 2 would not be masked.
		//
		// The flag is also used by The Dig and Full Throttle, but I
		// don't know what for. At the time of writing, these games
		// are still too unstable for me to investigate.

		if (_vm->_game.version == 8)
			z_plane_ptr = zplane_list[1] + READ_LE_UINT32(zplane_list[1] + stripnr * 4 + 8);
		else
			z_plane_ptr = zplane_list[1] + READ_LE_UINT16(zplane_list[1] + stripnr * 2 + 8);
		for (i = 0; i < numzbuf; i++) {
			mask_ptr = getMaskBuffer(x, y, i);
			if (transpStrip && (flag & dbAllowMaskOr))
				decompressMaskImgOr(mask_ptr, z_plane_ptr, height);
			else
				decompressMaskImg(mask_ptr, z_plane_ptr, height);
		}
	} else {
		for (i = 1; i < numzbuf; i++) {
			uint32 offs;

			if (!zplane_list[i])
				continue;

			if (_vm->_game.features & GF_OLD_BUNDLE)
				offs = READ_LE_UINT16(zplane_list[i] + stripnr * 2);
			else if (_vm->_game.features & GF_OLD256)
				offs = READ_LE_UINT16(zplane_list[i] + stripnr * 2 + 4);
			else if (_vm->_game.features & GF_SMALL_HEADER)
				offs = READ_LE_UINT16(zplane_list[i] + stripnr * 2 + 2);
			else if (_vm->_game.version == 8)
				offs = READ_LE_UINT32(zplane_list[i] + stripnr * 4 + 8);
			else
				offs = READ_LE_UINT16(zplane_list[i] + stripnr * 2 + 8);

			mask_ptr = getMaskBuffer(x, y, i);

			if (offs) {
				z_plane_ptr = zplane_list[i] + offs;

				if (transpStrip && (flag & dbAllowMaskOr)) {
					decompressMaskImgOr(mask_ptr, z_plane_ptr, height);
				} else {
					decompressMaskImg(mask_ptr, z_plane_ptr, height);
				}

			} else {
				if (!(transpStrip && (flag & dbAllowMaskOr)))
					for (int h = 0; h < height; h++)
						mask_ptr[h * _numStrips] = 0;
			}
		}
	}
}

void GdiHE::decodeMask(int x, int y, const int width, const int height,
	                int stripnr, int numzbuf, const byte *zplane_list[9],
	                bool transpStrip, byte flag) {
	int i;
	byte *mask_ptr;
	const byte *z_plane_ptr;

	for (i = 1; i < numzbuf; i++) {
		uint32 offs;

		if (!zplane_list[i])
			continue;

		offs = READ_LE_UINT16(zplane_list[i] + stripnr * 2 + 8);

		mask_ptr = getMaskBuffer(x, y, i);

		if (offs) {
			z_plane_ptr = zplane_list[i] + offs;

			if (_tmskPtr) {
				const byte *tmsk = _tmskPtr + READ_LE_UINT16(_tmskPtr + stripnr * 2 + 8);
				decompressTMSK(mask_ptr, tmsk, z_plane_ptr, height);
			} else if (transpStrip && (flag & dbAllowMaskOr)) {
				decompressMaskImgOr(mask_ptr, z_plane_ptr, height);
			} else {
				decompressMaskImg(mask_ptr, z_plane_ptr, height);
			}

		} else {
			if (!(transpStrip && (flag & dbAllowMaskOr)))
				for (int h = 0; h < height; h++)
					mask_ptr[h * _numStrips] = 0;
		}
	}
}

void GdiNES::decodeMask(int x, int y, const int width, const int height,
	                int stripnr, int numzbuf, const byte *zplane_list[9],
	                bool transpStrip, byte flag) {
	byte *mask_ptr = getMaskBuffer(x, y, 1);
	drawStripNESMask(mask_ptr, stripnr, y, height);
}

#ifdef USE_RGB_COLOR
void GdiPCEngine::decodeMask(int x, int y, const int width, const int height,
	                int stripnr, int numzbuf, const byte *zplane_list[9],
	                bool transpStrip, byte flag) {
	byte *mask_ptr = getMaskBuffer(x, y, 1);
	drawStripPCEngineMask(mask_ptr, stripnr, y, height);
}
#endif

void GdiV1::decodeMask(int x, int y, const int width, const int height,
	                int stripnr, int numzbuf, const byte *zplane_list[9],
	                bool transpStrip, byte flag) {
	byte *mask_ptr = getMaskBuffer(x, y, 1);
	drawStripV1Mask(mask_ptr, stripnr, width, height);
}

void GdiV2::decodeMask(int x, int y, const int width, const int height,
	                int stripnr, int numzbuf, const byte *zplane_list[9],
	                bool transpStrip, byte flag) {
	// Do nothing here for V2 games - zplane was already handled.
}

#ifdef ENABLE_HE
static const byte bitMasks[9] = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF };

/**
 * Draw a bitmap onto a virtual screen. This is main drawing method for room backgrounds
 * used throughout HE71+ versions.
 *
 * @note This function essentially is a stripped down & special cased version of
 * the generic Gdi::drawBitmap() method.
 */
void Gdi::drawBMAPBg(const byte *ptr, VirtScreen *vs) {
	const byte *zPlanePtr;
	byte *maskPtr;
	const byte *zPlaneList[9];

	const byte *bmapPtr = _vm->findResourceData(MKTAG('B','M','A','P'), ptr);
	assert(bmapPtr);

	byte code = *bmapPtr++;
	byte *dst = vs->getBackPixels(0, 0);

	switch (code) {
	case BMCOMP_NMAJMIN_H4:
	case BMCOMP_NMAJMIN_H5:
	case BMCOMP_NMAJMIN_H6:
	case BMCOMP_NMAJMIN_H7:
	case BMCOMP_NMAJMIN_H8:
		_decomp_shr = code - BMCOMP_NMAJMIN_H0; // Bits per pixel
		_decomp_mask = bitMasks[_decomp_shr];
		drawStripHE(dst, vs->pitch, bmapPtr, vs->w, vs->h, false);
		break;
	case BMCOMP_NMAJMIN_HT4:
	case BMCOMP_NMAJMIN_HT5:
	case BMCOMP_NMAJMIN_HT6:
	case BMCOMP_NMAJMIN_HT7:
	case BMCOMP_NMAJMIN_HT8:
		_decomp_shr = code - BMCOMP_NMAJMIN_HT0; // Bits per pixel
		_decomp_mask = bitMasks[_decomp_shr];
		drawStripHE(dst, vs->pitch, bmapPtr, vs->w, vs->h, false);
		break;
	case BMCOMP_SOLID_COLOR_FILL:
	{
		WizRawPixel color = (WizRawPixel)(*bmapPtr);
		if (_vm->_game.heversion > 99 && _vm->VAR_COLOR_BLACK != 0xFF && _vm->VAR(_vm->VAR_COLOR_BLACK) == color)
			break;

		if (_vm->_game.heversion > 90)
			color = ((ScummEngine_v71he *)_vm)->_wiz->convert8BppToRawPixel(*bmapPtr, (WizRawPixel *)_vm->getHEPaletteSlot(1));

		WizSimpleBitmap dstBitmap;
		dstBitmap.bufferPtr = WizPxShrdBuffer(dst, false);
		dstBitmap.bitmapWidth = vs->w;
		dstBitmap.bitmapHeight = vs->h;
		Common::Rect fillRect(0, 0, (dstBitmap.bitmapWidth - 1), (dstBitmap.bitmapHeight - 1));

		((ScummEngine_v71he *)_vm)->_wiz->pgDrawSolidRect(&dstBitmap, &fillRect, color);
		break;
	}
	default:
		// Alternative russian freddi3 uses badly formatted bitmaps
		debug(0, "Gdi::drawBMAPBg: default case %d", code);
	}

	((ScummEngine_v71he *)_vm)->backgroundToForegroundBlit(Common::Rect(vs->w, vs->h));

	int numzbuf = getZPlanes(ptr, zPlaneList, true);
	if (numzbuf <= 1)
		return;

	uint32 offs;
	for (int stripnr = 0; stripnr < _numStrips; stripnr++) {
		for (int i = 1; i < numzbuf; i++) {
			if (!zPlaneList[i])
				continue;

			offs = READ_LE_UINT16(zPlaneList[i] + stripnr * 2 + 8);
			maskPtr = getMaskBuffer(stripnr, 0, i);

			if (offs) {
				zPlanePtr = zPlaneList[i] + offs;
				decompressMaskImg(maskPtr, zPlanePtr, vs->h);
			}
		}

#if 0
		// HACK: blit mask(s) onto normal screen. Useful to debug masking
		int bytesPerStrip = (_vm->_game.features & GF_16BIT_COLOR) ? 16 : 8;

		for (int i = 0; i < numzbuf; i++) {
			byte *dst1 = (byte *)vs->getPixels(0, 0) + stripnr * bytesPerStrip;
			byte *dst2 = vs->backBuf + stripnr * bytesPerStrip;

			byte *mask_ptr = getMaskBuffer(stripnr, 0, i);
			for (int h = 0; h < vs->h; h++) {
				int maskbits = *mask_ptr;
				for (int j = 0; j < 8; j++) {
					if (maskbits & 0x80) {
						if (bytesPerStrip == 16) {
							WRITE_UINT16(dst1 + j * 2, _vm->_16BitPalette[12 + i]);
							WRITE_UINT16(dst2 + j * 2, _vm->_16BitPalette[12 + i]);
						} else
							dst1[j] = dst2[j] = 12 + i;
					}
					maskbits <<= 1;
				}
				dst1 += vs->pitch;
				dst2 += vs->pitch;
				mask_ptr += _numStrips;
			}
		}
#endif
	}
}

void Gdi::drawBMAPObject(const byte *ptr, VirtScreen *vs, int obj, int x, int y, int w, int h) {
	const byte *bmapPtr = _vm->findResourceData(MKTAG('B','M','A','P'), ptr);
	assert(bmapPtr);

	byte code = *bmapPtr++;
	int scrX = _vm->_screenStartStrip * 8 * _vm->_bytesPerPixel;
	WizPxShrdBuffer dst(_vm->_virtscr[kMainVirtScreen].backBuf + scrX, false);

	int scrWidth = _vm->_game.heversion > 98 ? _vm->_screenWidth : vs->w;
	int scrHeight = _vm->_game.heversion > 98 ? _vm->_screenHeight : vs->h;

	switch (code) {
	case BMCOMP_RLE8BIT:
	case BMCOMP_TRLE8BIT:
	{
		Common::Rect rScreen(0, 0, scrWidth, scrHeight);
		if (_vm->_game.heversion <= 98) {
			rScreen.right -= 1;
			rScreen.bottom -= 1;
		}

		int xCoord = x + (_vm->_game.heversion > 98 ? scrX : -scrX);

		((ScummEngine_v71he *)_vm)->_wiz->auxDecompTRLEImage(dst(), bmapPtr, scrWidth, scrHeight, xCoord, y, w, h, &rScreen, nullptr);
		break;
	}
	case BMCOMP_SOLID_COLOR_FILL:
	{
		ScummEngine_v71he *tmpVm = ((ScummEngine_v71he *)_vm);
		WizRawPixel color = tmpVm->_wiz->convert8BppToRawPixel(*bmapPtr, (WizRawPixel *)_vm->getHEPaletteSlot(1));

		if ((_vm->_game.heversion < 100) || (tmpVm->VAR_WIZ_TRANSPARENT_COLOR != 0xFF && _vm->VAR(tmpVm->VAR_WIZ_TRANSPARENT_COLOR) == *bmapPtr))
			break;

		WizSimpleBitmap dstBitmap;
		dstBitmap.bufferPtr = dst;
		dstBitmap.bitmapWidth = w;
		dstBitmap.bitmapHeight = h;
		Common::Rect fillRect(x + scrX, y, x + scrX + w - 1, y + h - 1);

		((ScummEngine_v71he *)_vm)->_wiz->pgDrawSolidRect(&dstBitmap, &fillRect, color);
		break;
	}
	default:
		error("Gdi::drawBMAPObject(): Unhandled code %d", code);
	}

	Common::Rect renderArea, clipArea, backgroundCoords;

	if (_vm->_game.heversion > 98) {
		((ScummEngine_v71he *)_vm)->_wiz->makeSizedRectAt(&renderArea, x + scrX, y, w, h);

		((ScummEngine_v71he *)_vm)->_wiz->makeSizedRect(&clipArea, scrWidth, scrHeight);
		((ScummEngine_v71he *)_vm)->_wiz->findRectOverlap(&renderArea, &clipArea);

		// Unless the image was entirely clipped, copy newly decompressed
		// pixels from the background buffer into the foreground buffer...
		if (((ScummEngine_v71he *)_vm)->_wiz->isRectValid(renderArea)) {
			((ScummEngine_v71he *)_vm)->backgroundToForegroundBlit(renderArea);
		}
	} else {
		((ScummEngine_v71he *)_vm)->_wiz->makeSizedRectAt(&renderArea, x, y, w, h);
		((ScummEngine_v71he *)_vm)->_wiz->makeSizedRectAt(&backgroundCoords, scrX, 0, scrWidth, scrHeight);
		((ScummEngine_v71he *)_vm)->_wiz->findRectOverlap(&renderArea, &backgroundCoords);

		if (((ScummEngine_v71he *)_vm)->_wiz->isRectValid(renderArea)) {
			// Convert the coords to buffer relative coords and then copy the
			// background pixels to the foreground buffer as well...
			((ScummEngine_v71he *)_vm)->_wiz->moveRect(&renderArea, -backgroundCoords.left, -backgroundCoords.top);
			((ScummEngine_v71he *)_vm)->backgroundToForegroundBlit(renderArea);
		}
	}

}

void ScummEngine_v70he::backgroundToForegroundBlit(Common::Rect rect, int dirtybit) {
	byte *src, *dst;
	VirtScreen *vs = &_virtscr[kMainVirtScreen];

	if (rect.top > vs->h || rect.bottom < 0)
		return;

	if (rect.left > vs->w || rect.right < 0)
		return;

	rect.left = MAX(0, (int)rect.left);
	rect.left = MIN((int)rect.left, (int)vs->w - 1);

	rect.right = MAX(0, (int)rect.right);
	rect.right = MIN((int)rect.right, (int)vs->w - 1);

	rect.top = MAX(0, (int)rect.top);
	rect.top = MIN((int)rect.top, (int)vs->h - 1);

	rect.bottom = MAX(0, (int)rect.bottom);
	rect.bottom = MIN((int)rect.bottom, (int)vs->h - 1);

	const int rw = rect.width() + 1;
	const int rh = rect.height() + 1;

	if (rw == 0 || rh == 0)
		return;

	src = _virtscr[kMainVirtScreen].getBackPixels(rect.left, rect.top);
	dst = _virtscr[kMainVirtScreen].getPixels(rect.left, rect.top);

	assert(rw <= _screenWidth && rw > 0);
	assert(rh <= _screenHeight && rh > 0);
	blit(dst, _virtscr[kMainVirtScreen].pitch, src, _virtscr[kMainVirtScreen].pitch, rw, rh, vs->format.bytesPerPixel);

	rect.bottom++;
	markRectAsDirty(kMainVirtScreen, rect, dirtybit);
}
#endif

/**
 * Reset the background behind an actor or blast object.
 */
void Gdi::resetBackground(int top, int bottom, int strip) {
	VirtScreen *vs = &_vm->_virtscr[kMainVirtScreen];
	byte *backbuff_ptr, *bgbak_ptr;
	int numLinesToProcess;

	if (top < 0)
		top = 0;

	if (bottom > vs->h)
		bottom = vs->h;

	if (top >= bottom)
		return;

	assert(0 <= strip && strip < _numStrips);

	if (top < vs->tdirty[strip])
		vs->tdirty[strip] = top;

	if (bottom > vs->bdirty[strip])
		vs->bdirty[strip] = bottom;

	bgbak_ptr = (byte *)vs->backBuf + top * vs->pitch + (strip + vs->xstart/8) * 8 * vs->format.bytesPerPixel;
	backbuff_ptr = (byte *)vs->getBasePtr((strip + vs->xstart/8) * 8, top);

	numLinesToProcess = bottom - top;
	if (numLinesToProcess) {
		if (_vm->isLightOn()) {
			copy8Col(backbuff_ptr, vs->pitch, bgbak_ptr, numLinesToProcess, vs->format.bytesPerPixel);
		} else {
			clear8Col(backbuff_ptr, vs->pitch, numLinesToProcess, vs->format.bytesPerPixel);
		}
	}
}

bool Gdi::decompressBitmap(byte *dst, int dstPitch, const byte *src, int numLinesToProcess) {
	assert(numLinesToProcess);

	if (_vm->_game.features & GF_16COLOR) {
		drawStripEGA(dst, dstPitch, src, numLinesToProcess);
		return false;
	}

	if ((_vm->_game.platform == Common::kPlatformAmiga) && (_vm->_game.version >= 4))
		_paletteMod = 16;
	else
		_paletteMod = 0;

	byte code = *src++;
	bool transpStrip = false;
	_decomp_shr = code % 10;
	_decomp_mask = 0xFF >> (8 - _decomp_shr);

	switch (code) {
	case BMCOMP_RAW256:
		drawStripRaw(dst, dstPitch, src, numLinesToProcess, false);
		break;

	case BMCOMP_TOWNS_2:
		unkDecode8(dst, dstPitch, src, numLinesToProcess);       /* Ender - Zak256/Indy256 */
		break;

	case BMCOMP_TOWNS_3:
		unkDecode9(dst, dstPitch, src, numLinesToProcess);       /* Ender - Zak256/Indy256 */
		break;

	case BMCOMP_TOWNS_4:
		unkDecode10(dst, dstPitch, src, numLinesToProcess);      /* Ender - Zak256/Indy256 */
		break;

	case BMCOMP_TOWNS_7:
		unkDecode11(dst, dstPitch, src, numLinesToProcess);      /* Ender - Zak256/Indy256 */
		break;

	case BMCOMP_TRLE8BIT:
		// Used in 3DO versions of HE games
		transpStrip = true;
		drawStrip3DO(dst, dstPitch, src, numLinesToProcess, true);
		break;

	case BMCOMP_RLE8BIT:
		drawStrip3DO(dst, dstPitch, src, numLinesToProcess, false);
		break;

	case BMCOMP_PIX32:
		// Used in Amiga version of Monkey Island 1
		drawStripEGA(dst, dstPitch, src, numLinesToProcess);
		break;

	case BMCOMP_ZIGZAG_V4:
	case BMCOMP_ZIGZAG_V5:
	case BMCOMP_ZIGZAG_V6:
	case BMCOMP_ZIGZAG_V7:
	case BMCOMP_ZIGZAG_V8:
		drawStripBasicV(dst, dstPitch, src, numLinesToProcess, false);
		break;

	case BMCOMP_ZIGZAG_H4:
	case BMCOMP_ZIGZAG_H5:
	case BMCOMP_ZIGZAG_H6:
	case BMCOMP_ZIGZAG_H7:
	case BMCOMP_ZIGZAG_H8:
		drawStripBasicH(dst, dstPitch, src, numLinesToProcess, false);
		break;

	case BMCOMP_ZIGZAG_VT4:
	case BMCOMP_ZIGZAG_VT5:
	case BMCOMP_ZIGZAG_VT6:
	case BMCOMP_ZIGZAG_VT7:
	case BMCOMP_ZIGZAG_VT8:
		transpStrip = true;
		drawStripBasicV(dst, dstPitch, src, numLinesToProcess, true);
		break;

	case BMCOMP_ZIGZAG_HT4:
	case BMCOMP_ZIGZAG_HT5:
	case BMCOMP_ZIGZAG_HT6:
	case BMCOMP_ZIGZAG_HT7:
	case BMCOMP_ZIGZAG_HT8:
		transpStrip = true;
		drawStripBasicH(dst, dstPitch, src, numLinesToProcess, true);
		break;

	case BMCOMP_MAJMIN_H4:
	case BMCOMP_MAJMIN_H5:
	case BMCOMP_MAJMIN_H6:
	case BMCOMP_MAJMIN_H7:
	case BMCOMP_MAJMIN_H8:
	case BMCOMP_RMAJMIN_H4:
	case BMCOMP_RMAJMIN_H5:
	case BMCOMP_RMAJMIN_H6:
	case BMCOMP_RMAJMIN_H7:
	case BMCOMP_RMAJMIN_H8:
		drawStripComplex(dst, dstPitch, src, numLinesToProcess, false);
		break;

	case BMCOMP_MAJMIN_HT4:
	case BMCOMP_MAJMIN_HT5:
	case BMCOMP_MAJMIN_HT6:
	case BMCOMP_MAJMIN_HT7:
	case BMCOMP_MAJMIN_HT8:
	case BMCOMP_RMAJMIN_HT4:
	case BMCOMP_RMAJMIN_HT5:
	case BMCOMP_RMAJMIN_HT6:
	case BMCOMP_RMAJMIN_HT7:
	case BMCOMP_RMAJMIN_HT8:
		transpStrip = true;
		drawStripComplex(dst, dstPitch, src, numLinesToProcess, true);
		break;

	case BMCOMP_NMAJMIN_H4:
	case BMCOMP_NMAJMIN_H5:
	case BMCOMP_NMAJMIN_H6:
	case BMCOMP_NMAJMIN_H7:
	case BMCOMP_NMAJMIN_H8:
		drawStripHE(dst, dstPitch, src, 8, numLinesToProcess, false);
		break;

	case BMCOMP_CUSTOM_RU_TR: // Triggered by Russian water
	case BMCOMP_NMAJMIN_HT4:
	case BMCOMP_NMAJMIN_HT5:
	case BMCOMP_NMAJMIN_HT6:
	case BMCOMP_NMAJMIN_HT7:
	case BMCOMP_NMAJMIN_HT8:
		transpStrip = true;
		drawStripHE(dst, dstPitch, src, 8, numLinesToProcess, true);
		break;

	case BMCOMP_TPIX256:
		drawStripRaw(dst, dstPitch, src, numLinesToProcess, true);
		break;

	default:
		error("Gdi::decompressBitmap: default case %d", code);
	}

	return transpStrip;
}

void Gdi::decompressMaskImg(byte *dst, const byte *src, int height) const {
	byte b, c;

	while (height) {
		b = *src++;

		if (b & 0x80) {
			b &= 0x7F;
			c = *src++;

			do {
				*dst = c;
				dst += _numStrips;
				--height;
			} while (--b && height);
		} else {
			do {
				*dst = *src++;
				dst += _numStrips;
				--height;
			} while (--b && height);
		}
	}
}

void GdiHE::decompressTMSK(byte *dst, const byte *tmsk, const byte *src, int height) const {
	byte srcbits = 0;
	byte srcFlag = 0;
	byte maskFlag = 0;

	byte srcCount = 0;
	byte maskCount = 0;
	byte maskbits = 0;

	while (height) {
		if (srcCount == 0) {
			srcCount = *src++;
			srcFlag = srcCount & 0x80;
			if (srcFlag) {
				srcCount &= 0x7F;
				srcbits = *src++;
			}
		}

		if (srcFlag == 0) {
			srcbits = *src++;
		}

		srcCount--;

		if (maskCount == 0) {
			maskCount = *tmsk++;
			maskFlag = maskCount & 0x80;
			if (maskFlag) {
				maskCount &= 0x7F;
				maskbits = *tmsk++;
			}
		}

		if (maskFlag == 0) {
			maskbits = *tmsk++;
		}

		maskCount--;

		*dst = (*dst & (~maskbits)) | (srcbits & maskbits);

		dst += _numStrips;
		height--;
	}
}

void Gdi::decompressMaskImgOr(byte *dst, const byte *src, int height) const {
	byte b, c;

	while (height) {
		b = *src++;

		if (b & 0x80) {
			b &= 0x7F;
			c = *src++;

			do {
				*dst |= c;
				dst += _numStrips;
				--height;
			} while (--b && height);
		} else {
			do {
				*dst |= *src++;
				dst += _numStrips;
				--height;
			} while (--b && height);
		}
	}
}

static void decodeNESTileData(const byte *src, byte *dest) {
	int len = READ_LE_UINT16(src);	src += 2;
	const byte *end = src + len;
	src++;	// skip number-of-tiles byte, assume it is correct
	while (src < end) {
		byte data = *src++;
		for (int j = 0; j < (data & 0x7F); j++)
			*dest++ = (data & 0x80) ? (*src++) : (*src);
		if (!(data & 0x80))
			src++;
	}
}

void ScummEngine::decodeNESBaseTiles() {
	byte *basetiles = getResourceAddress(rtCostume, 37);
	_NESBaseTiles = basetiles[2];
	decodeNESTileData(basetiles, _NESPatTable[1]);
}

static const int v1MMNEScostTables[2][6] = {
	/* desc lens offs data  gfx  pal */
	{ 25,  27,  29,  31,  33,  35},
	{ 26,  28,  30,  32,  34,  36}
};

void ScummEngine::NES_loadCostumeSet(int n) {
	int i;
	_NESCostumeSet = n;

	_NEScostdesc = getResourceAddress(rtCostume, v1MMNEScostTables[n][0]) + 2;
	_NEScostlens = getResourceAddress(rtCostume, v1MMNEScostTables[n][1]) + 2;
	_NEScostoffs = getResourceAddress(rtCostume, v1MMNEScostTables[n][2]) + 2;
	_NEScostdata = getResourceAddress(rtCostume, v1MMNEScostTables[n][3]) + 2;
	decodeNESTileData(getResourceAddress(rtCostume, v1MMNEScostTables[n][4]), _NESPatTable[0]);
	byte *palette = getResourceAddress(rtCostume, v1MMNEScostTables[n][5]) + 2;
	for (i = 0; i < 16; i++) {
		byte c = *palette++;
		_NESPalette[1][i] = c;
	}

}

void GdiNES::decodeNESGfx(const byte *room) {
	const byte *gdata = room + READ_LE_UINT16(room + 0x0A);
	int tileset = *gdata++;
	int width = READ_LE_UINT16(room + 0x04);
	// int height = READ_LE_UINT16(room + 0x06);
	int i, j, n;

	// We have narrow room. so expand it
	if (width < 32) {
		_vm->_NESStartStrip = (32 - width) >> 1;
	} else {
		_vm->_NESStartStrip = 0;
	}

	decodeNESTileData(_vm->getResourceAddress(rtCostume, 37 + tileset), _vm->_NESPatTable[1] + _vm->_NESBaseTiles * 16);
	for (i = 0; i < 16; i++) {
		byte c = *gdata++;
		_vm->_NESPalette[0][i] = c;
	}
	for (i = 0; i < 16; i++) {
		_NES.nametable[i][0] = _NES.nametable[i][1] = 0;
		n = 0;
		while (n < width) {
			byte data = *gdata++;
			for (j = 0; j < (data & 0x7F); j++)
				_NES.nametable[i][2 + n++] = (data & 0x80) ? (*gdata++) : (*gdata);
			if (!(data & 0x80))
				gdata++;
		}
		_NES.nametable[i][width+2] = _NES.nametable[i][width+3] = 0;
	}
	memcpy(_NES.nametableObj,_NES.nametable, 16*64);

	const byte *adata = room + READ_LE_UINT16(room + 0x0C);
	for (n = 0; n < 64;) {
		byte data = *adata++;
		for (j = 0; j < (data & 0x7F); j++)
			_NES.attributes[n++] = (data & 0x80) ? (*adata++) : (*adata);
		if (!(n & 7) && (width == 0x1C))
			n += 8;
		if (!(data & 0x80))
			adata++;
	}
	memcpy(_NES.attributesObj, _NES.attributes, 64);

	const byte *mdata = room + READ_LE_UINT16(room + 0x0E);
	int mask = *mdata++;
	if (mask == 0) {
		_NES.hasmask = false;
		return;
	}
	_NES.hasmask = true;
	if (mask != 1)
		debug(0,"NES room %i has irregular mask count %i",_vm->_currentRoom,mask);
	int mwidth = *mdata++;
	for (i = 0; i < 16; i++) {
		n = 0;
		while (n < mwidth) {
			byte data = *mdata++;
			for (j = 0; j < (data & 0x7F); j++)
				_NES.masktable[i][n++] = (data & 0x80) ? (*mdata++) : (*mdata);
			if (!(data & 0x80))
				mdata++;
		}
	}
	memcpy(_NES.masktableObj, _NES.masktable, 16*8);
}

void GdiNES::decodeNESObject(const byte *ptr, int xpos, int ypos, int width, int height) {
	int x, y;

	_NES.objX = xpos;

	// decode tile update data
	width /= 8;
	ypos /= 8;
	height /= 8;

	for (y = ypos; y < ypos + height; y++) {
		x = xpos;
		while (x < xpos + width) {
			byte len = *ptr++;
			for (int i = 0; i < (len & 0x7F); i++)
				_NES.nametableObj[y][2 + x++] = (len & 0x80) ? (*ptr++) : (*ptr);
			if (!(len & 0x80))
				ptr++;
		}
	}

	int ax, ay;
	// decode attribute update data
	y = height / 2;
	ay = ypos;
	while (y) {
		ax = xpos + 2;
		x = 0;
		int adata = 0;
		while (x < (width >> 1)) {
			if (!(x & 3))
				adata = *ptr++;
			byte *dest = &_NES.attributesObj[((ay << 2) & 0x30) | ((ax >> 2) & 0xF)];

			int aand = 3;
			int aor = adata & 3;
			if (ay & 0x02) {
				aand <<= 4;
				aor <<= 4;
			}
			if (ax & 0x02) {
				aand <<= 2;
				aor <<= 2;
			}
			*dest = ((~aand) & *dest) | aor;

			adata >>= 2;
			ax += 2;
			x++;
		}
		ay += 2;
		y--;
	}

	// decode mask update data
	if (!_NES.hasmask)
		return;
	int mx, mwidth;
	int lmask, rmask;
	mx = *ptr++;
	mwidth = *ptr++;
	lmask = *ptr++;
	rmask = *ptr++;

	for (y = 0; y < height; ++y) {
		byte *dest = &_NES.masktableObj[y + ypos][mx];
		*dest = (*dest & lmask) | *ptr++;
		dest++;
		for (x = 1; x < mwidth; x++) {
			if (x + 1 == mwidth)
				*dest = (*dest & rmask) | *ptr++;
			else
				*dest = *ptr++;
			dest++;
		}
	}
}

void GdiNES::drawStripNES(byte *dst, byte *mask, int dstPitch, int stripnr, int top, int height) {
	const byte darkPalette[16] = { 0x2d,0x1d,0x3d,0x20, 0x2d,0x1d,0x3d,0x20, 0x2d,0x1d,0x3d,0x20, 0x2d,0x1d,0x3d,0x20 };
	const byte* stripPalette;
	top /= 8;
	height /= 8;
	int x = stripnr + 2;	// NES version has a 2 tile gap on each edge

	// MM NES does not paint the background when lit with a flashlight
	if (_vm->isLightOn())
		stripPalette = _vm->_NESPalette[0];
	else
		stripPalette = darkPalette;

	if (_objectMode)
		x += _NES.objX; // for objects, need to start at the left edge of the object, not the screen
	if (x > 63) {
		debug(0,"NES tried to render invalid strip %i",stripnr);
		return;
	}
	for (int y = top; y < top + height; y++) {
		int palette = ((_objectMode ? _NES.attributesObj : _NES.attributes)[((y << 2) & 0x30) | ((x >> 2) & 0xF)] >> (((y & 2) << 1) | (x & 2))) & 0x3;
		int tile = (_objectMode ? _NES.nametableObj : _NES.nametable)[y][x];

		for (int i = 0; i < 8; i++) {
			byte c0 = _vm->_NESPatTable[1][tile * 16 + i];
			byte c1 = _vm->_NESPatTable[1][tile * 16 + i + 8];
			for (int j = 0; j < 8; j++)
				dst[j] = stripPalette[((c0 >> (7 - j)) & 1) | (((c1 >> (7 - j)) & 1) << 1) | (palette << 2)];
			dst += dstPitch;
			*mask = c0 | c1;
			mask += _numStrips;
		}
	}
}

void GdiNES::drawStripNESMask(byte *dst, int stripnr, int top, int height) const {
	top /= 8;
	height /= 8;
	int x = stripnr;	// masks, unlike room graphics, should NOT be adjusted

	if (_objectMode)
		x += _NES.objX; // for objects, need to start at the left edge of the object, not the screen
	if (x > 63) {
		debug(0,"NES tried to mask invalid strip %i",stripnr);
		return;
	}
	for (int y = top; y < top + height; y++) {
		byte c;
		if (_NES.hasmask)
			c = (((_objectMode ? _NES.masktableObj : _NES.masktable)[y][x >> 3] >> (x & 7)) & 1) ? 0xFF : 0x00;
		else
			c = 0;

		for (int i = 0; i < 8; i++) {
			*dst &= c;
			dst += _numStrips;
		}
	}
}

#ifdef USE_RGB_COLOR
void readOffsetTable(const byte *ptr, uint16 **table, int *count) {
	int pos = 0;
	*count = READ_LE_UINT16(ptr) / 2 + 1;
	*table = (uint16 *)malloc(*count * sizeof(uint16));
	for (int i = 0; i < *count; i++) {
		(*table)[i] = READ_LE_UINT16(ptr + pos) + pos + 2;
		pos += 2;
	}
}

void decodeTileColor(byte cmd, byte *colors, int *rowIndex, int numRows) {
	colors[(*rowIndex)++] = ((cmd) >> 4) & 0xF;
	if (*rowIndex < numRows)
		colors[(*rowIndex)++] = (cmd) & 0xF;
}

void GdiPCEngine::decodeStrip(const byte *ptr, uint16 *tiles, byte *colors, uint16 *masks, int numRows, bool isObject) {
	int loopCnt;
	uint16 lastTileData;

	/*
	 * read tiles indices
	 */

	int rowIndex = 0;
	if (isObject) {
		loopCnt = numRows;
	} else {
		tiles[rowIndex++] = 0;
		tiles[numRows - 1] = 0;
		loopCnt = numRows - 1;
	}

	while (true) {
		uint16 cmd = READ_LE_UINT16(ptr);
		ptr += 2;
		if (cmd & 0x8000) {
			tiles[rowIndex - 1] = cmd  & 0x0FFF;
		} else if (cmd & 0x4000) {
			tiles[numRows - 1] = cmd & 0x0FFF;
		} else {
			tiles[rowIndex++] = cmd;
			lastTileData = cmd;
			break;
		}
	}

	while (rowIndex < loopCnt) {
		byte cmd = *ptr++;
		int cnt = cmd & 0x1F;

		if (cmd & 0x80) {
			for (int i = 0; i < cnt; ++i) {
				tiles[rowIndex++] = lastTileData;
			}
		} else if (cmd & 0x40) {
			for (int i = 0; i < cnt; ++i) {
				++lastTileData;
				tiles[rowIndex++] = lastTileData;
			}
		} else {
			for (int i = 0; i < cnt; ++i) {
				lastTileData = READ_LE_UINT16(ptr);
				ptr += 2;
				tiles[rowIndex++] = lastTileData;
			}
		}
	}

	/*
	 * read palette data
	 */

	rowIndex = 0;
	byte cmd = *ptr++;
	if (cmd == 0xFE) {
		while (rowIndex < numRows) {
			decodeTileColor(*ptr++, colors, &rowIndex, numRows);
		}
	} else {
		byte lastCmd = cmd;
		decodeTileColor(cmd, colors, &rowIndex, numRows);
		while (rowIndex < numRows) {
			cmd = *ptr++;
			int cnt = cmd & 0x1F;
			if (cmd & 0x80) {
				for (int j = 0; j < cnt; ++j) {
					decodeTileColor(lastCmd, colors, &rowIndex, numRows);
				}
			} else {
				for (int j = 0; j < cnt; ++j) {
					cmd = *ptr++;
					decodeTileColor(cmd, colors, &rowIndex, numRows);
				}
				lastCmd = cmd;
			}
		}
	}

	/*
	 * read mask indices
	 */

	if (_distaff || _PCE.maskIDSize == 0 || numRows > 18) {
		return;
	}

	rowIndex = 0;
	while (rowIndex < numRows) {
		cmd = *ptr++;
		int cnt = cmd & 0x1F;
		if (cmd & 0x80) {
			uint16 value;
			if (cmd & 0x60) {
				value = (cmd & 0x40) ? 0 : 0xFF;
			} else if (_PCE.maskIDSize == 1) {
				value = *ptr++;
			} else {
				value = READ_LE_UINT16(ptr);
				ptr += 2;
			}
			for (int i = 0; i < cnt; ++i) {
				masks[rowIndex++] = value;
			}
		} else {
			for (int i = 0; i < cnt; ++i) {
				if (_PCE.maskIDSize == 1) {
					masks[rowIndex++] = *ptr++;
				} else {
					masks[rowIndex++] = READ_LE_UINT16(ptr);
					ptr += 2;
				}
			}
		}
	}
}

void GdiPCEngine::decodePCEngineGfx(const byte *room) {
	uint16* stripOffsets;

	decodePCEngineTileData(_vm->findResourceData(MKTAG('T','I','L','E'), room));
	decodePCEngineMaskData(_vm->findResourceData(MKTAG('Z','P','0','0'), room));

	const byte* smap_ptr = _vm->findResourceData(MKTAG('I','M','0','0'), room);
	smap_ptr++; // roomID
	int numStrips = *smap_ptr++;
	int numRows = *smap_ptr++;
	_PCE.maskIDSize = *smap_ptr++;
	smap_ptr++; // unknown

	memset(_PCE.nametable, 0, sizeof(_PCE.nametable));
	memset(_PCE.colortable, 0, sizeof(_PCE.colortable));
	readOffsetTable(smap_ptr, &stripOffsets, &numStrips);
	for (int i = 0; i < numStrips; ++i) {
		const byte *tilePtr = smap_ptr + stripOffsets[i];
		decodeStrip(tilePtr,
			&_PCE.nametable[i * numRows],
			&_PCE.colortable[i * numRows],
			&_PCE.masktable[i * numRows],
			numRows,
			false);
	}
	free(stripOffsets);
}

void GdiPCEngine::decodePCEngineObject(const byte *ptr, int xpos, int ypos, int width, int height) {
	uint16 *stripOffsets;
	int numStrips;
	int numRows = height / 8;

	memset(_PCE.nametableObj, 0, sizeof(_PCE.nametableObj));
	memset(_PCE.colortableObj, 0, sizeof(_PCE.colortableObj));
	readOffsetTable(ptr, &stripOffsets, &numStrips);
	for (int i = 0; i < numStrips; ++i) {
		const byte *tilePtr = ptr + stripOffsets[i];
		decodeStrip(tilePtr,
			&_PCE.nametableObj[i * numRows],
			&_PCE.colortableObj[i * numRows],
			&_PCE.masktableObj[i * numRows],
			numRows,
			true);
	}
	free(stripOffsets);
}

void GdiPCEngine::setTileData(byte *tile, int index, byte byte0, byte byte1) {
	int row = index % 8;
	int plane = (index / 8) * 2;
	int plane02Bit, plane13Bit;
	for (int col = 0; col < 8; ++col) {
		plane02Bit = (byte0 >> (7-col)) & 0x1; // plane=0: bit0, plane=2: bit2
		plane13Bit = (byte1 >> (7-col)) & 0x1; // plane=0: bit1, plane=2: bit3
		tile[row * 8 + col] |= plane02Bit << (plane + 0);
		tile[row * 8 + col] |= plane13Bit << (plane + 1);
	}
}

void GdiPCEngine::decodePCEngineTileData(const byte *ptr) {
	const byte *tilePtr;
	byte* tile;
	uint16* tileOffsets;
	byte byte0, byte1;

	readOffsetTable(ptr, &tileOffsets, &_PCE.numTiles);

	if (_distaff) {
		free(_PCE.staffTiles);
		_PCE.staffTiles = (byte *)calloc(_PCE.numTiles * 8 * 8, sizeof(byte));
	} else {
		free(_PCE.roomTiles);
		_PCE.roomTiles = (byte *)calloc(_PCE.numTiles * 8 * 8, sizeof(byte));
	}

	for (int i = 0; i < _PCE.numTiles; ++i) {
		tile = (_distaff) ? &_PCE.staffTiles[i * 64] : &_PCE.roomTiles[i * 64];
		tilePtr = ptr + tileOffsets[i];

		int index = 0;
		while (index < 16) {
			byte cmd = *tilePtr++;
			byte cnt = (cmd & 0x0F) + 1;
			if (cmd & 0x80) {
				byte0 = (cmd & 0x10) ? 0 : *tilePtr++;
				byte1 = (cmd & 0x40) ? 0 : *tilePtr++;
				while (cnt--) {
					setTileData(tile, index++, byte0, byte1);
				}
			} else {
				while (cnt--) {
					byte0 = (cmd & 0x10) ? 0 : *tilePtr++;
					byte1 = (cmd & 0x40) ? 0 : *tilePtr++;
					setTileData(tile, index++, byte0, byte1);
				}
			}
		}
	}

	free(tileOffsets);
}

void GdiPCEngine::decodePCEngineMaskData(const byte *ptr) {
	const byte *maskPtr;
	byte* mask;
	uint16* maskOffsets;

	if (!ptr) {
		_PCE.numMasks = 0;
		return;
	}

	readOffsetTable(ptr, &maskOffsets, &_PCE.numMasks);

	free(_PCE.masks);
	_PCE.masks = (byte *)malloc(_PCE.numMasks * 8 * sizeof(byte));

	for (int i = 0; i < _PCE.numMasks; ++i) {
		mask = &_PCE.masks[i * 8];
		maskPtr = ptr + maskOffsets[i];

		int index = 0;
		while (index < 8) {
			byte cmd = *maskPtr++;
			int cnt = cmd & 0x1F;
			if (cmd & 0x80) {
				byte value;
				if (cmd & 0x60)
					value = (cmd & 0x40) ? 0x00 : 0xFF;
				else
					value = *maskPtr++;
				for (int j = 0; j < cnt; ++j)
					mask[index++] = ~value;
			} else {
				for (int j = 0; j < cnt; ++j)
					mask[index++] = ~*maskPtr++;
			}
		}
	}

	free(maskOffsets);
}

void GdiPCEngine::drawStripPCEngine(byte *dst, byte *mask, int dstPitch, int stripnr, int top, int height) {
	uint16 tileIdx;
	byte *tile;
	int paletteIdx, paletteOffset, paletteEntry;
	height /= 8;

	for (int y = 0; y < height; y++) {
		tileIdx = (_objectMode ? _PCE.nametableObj : _PCE.nametable)[stripnr * height + y];
		tile = (_distaff) ? &_PCE.staffTiles[tileIdx * 64] : &_PCE.roomTiles[tileIdx * 64];
		paletteIdx = (_objectMode ? _PCE.colortableObj : _PCE.colortable)[stripnr * height + y];
		paletteOffset = paletteIdx * 16;
		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				paletteEntry = tile[row * 8 + col];
				WRITE_UINT16(dst + col * 2, _vm->_16BitPalette[paletteOffset + paletteEntry]);
			}
			dst += dstPitch;
		}
	}
}

void GdiPCEngine::drawStripPCEngineMask(byte *dst, int stripnr, int top, int height) const {
	uint16 maskIdx;
	height /= 8;

	for (int y = 0; y < height; y++) {
		maskIdx = (_objectMode ? _PCE.masktableObj : _PCE.masktable)[stripnr * height + y];
		for (int row = 0; row < 8; row++) {
			if (_PCE.numMasks > 0)
				*dst = _PCE.masks[maskIdx * 8 + row];
			else
				*dst = 0;
			dst += _numStrips;
		}
	}
}
#endif

void GdiV1::drawStripV1Background(byte *dst, int dstPitch, int stripnr, int height) {
	int charIdx;
	height /= 8;
	for (int y = 0; y < height; y++) {
		_V1.colors[3] = (_V1.colorMap[y + stripnr * height] & 7);
		// Check for room color change in V1 zak
		if (_roomPalette[0] == 255) {
			_V1.colors[2] = _roomPalette[2];
			_V1.colors[1] = _roomPalette[1];
		}

		charIdx = _V1.picMap[y + stripnr * height] * 8;
		for (int i = 0; i < 8; i++) {
			byte c = _V1.charMap[charIdx + i];
			dst[0] = dst[1] = _colorMap[_V1.colors[(c >> 6) & 3]];
			dst[2] = dst[3] = _colorMap[_V1.colors[(c >> 4) & 3]];
			dst[4] = dst[5] = _colorMap[_V1.colors[(c >> 2) & 3]];
			dst[6] = dst[7] = _colorMap[_V1.colors[(c >> 0) & 3]];
			dst += dstPitch;
		}
	}
}

void GdiV1::drawStripV1Object(byte *dst, int dstPitch, int stripnr, int width, int height) {
	int charIdx;
	height /= 8;
	width /= 8;
	for (int y = 0; y < height; y++) {
		_V1.colors[3] = (_V1.objectMap[(y + height) * width + stripnr] & 7);
		charIdx = _V1.objectMap[y * width + stripnr] * 8;
		for (int i = 0; i < 8; i++) {
			byte c = _V1.charMap[charIdx + i];
			dst[0] = dst[1] = _colorMap[_V1.colors[(c >> 6) & 3]];
			dst[2] = dst[3] = _colorMap[_V1.colors[(c >> 4) & 3]];
			dst[4] = dst[5] = _colorMap[_V1.colors[(c >> 2) & 3]];
			dst[6] = dst[7] = _colorMap[_V1.colors[(c >> 0) & 3]];
			dst += dstPitch;
		}
	}
}

void GdiV1::drawStripV1Mask(byte *dst, int stripnr, int width, int height) const {
	int maskIdx;
	height /= 8;
	width /= 8;
	for (int y = 0; y < height; y++) {
		if (_objectMode)
			maskIdx = _V1.objectMap[(y + 2 * height) * width + stripnr] * 8;
		else
			maskIdx = _V1.maskMap[y + stripnr * height] * 8;
		for (int i = 0; i < 8; i++) {
			byte c = _V1.maskChar[maskIdx + i];

			// V1/V0 masks are inverted compared to what ScummVM expects
			*dst = c ^ 0xFF;
			dst += _numStrips;
		}
	}
}

void GdiV1::decodeV1Gfx(const byte *src, byte *dst, int size) const {
	int x, z;
	byte color, run, common[4];

	for (z = 0; z < 4; z++) {
		common[z] = *src++;
	}

	x = 0;
	while (x < size) {
		run = *src++;
		if (run & 0x80) {
			color = common[(run >> 5) & 3];
			run &= 0x1F;
			for (z = 0; z <= run; z++) {
				dst[x++] = color;
			}
		} else if (run & 0x40) {
			run &= 0x3F;
			color = *src++;
			for (z = 0; z <= run; z++) {
				dst[x++] = color;
			}
		} else {
			for (z = 0; z <= run; z++) {
				dst[x++] = *src++;
			}
		}
	}
}

/**
 * Create and fill a table with offsets to the graphic and mask strips in the
 * given V2 EGA bitmap.
 * @param src		the V2 EGA bitmap
 * @param width		the width of the bitmap
 * @param height	the height of the bitmap
 * @param table		the strip table to fill
 * @return filled strip table
 */
StripTable *GdiV2::generateStripTable(const byte *src, int width, int height, StripTable *table) const {

	// If no strip table was given to use, allocate a new one
	if (table == nullptr)
		table = (StripTable *)calloc(1, sizeof(StripTable));

	const byte *bitmapStart = src;
	byte color = 0, data = 0;
	int x, y, length = 0;
	byte run = 1;

	// Decode the graphics strips, and memorize the run/color values
	// as well as the byte offset.
	for (x = 0; x < width; x++) {

		if ((x % 8) == 0) {
			assert(x / 8 < 160);
			table->run[x / 8] = run;
			table->color[x / 8] = color;
			table->offsets[x / 8] = src - bitmapStart;
		}

		for (y = 0; y < height; y++) {
			if (--run == 0) {
				data = *src++;
				if (data & 0x80) {
					run = data & 0x7f;
				} else {
					run = data >> 4;
				}
				if (run == 0) {
					run = *src++;
				}
				color = data & 0x0f;
			}
		}
	}

	// The mask data follows immediately after the graphics.
	x = 0;
	y = height;
	width /= 8;

	for (;;) {
		length = *src++;
		const byte runFlag = length & 0x80;
		if (runFlag) {
			length &= 0x7f;
			data = *src++;
		}
		do {
			if (!runFlag)
				data = *src++;
			if (y == height) {
				assert(x < 120);
				table->zoffsets[x] = src - bitmapStart - 1;
				table->zrun[x] = length | runFlag;
			}
			if (--y == 0) {
				if (--width == 0)
					return table;
				x++;
				y = height;
			}
		} while (--length);
	}

	return table;
}

void Gdi::drawStripEGA(byte *dst, int dstPitch, const byte *src, int height) const {
	byte color = 0;
	int run = 0, x = 0, y = 0, z;

	while (x < 8) {
		color = *src++;

		if (color & 0x80) {
			run = color & 0x3f;

			if (color & 0x40) {
				color = *src++;

				if (run == 0) {
					run = *src++;
				}
				for (z = 0; z < run; z++) {
					*(dst + y * dstPitch + x) = (z & 1) ? _roomPalette[(color & 0xf) + _paletteMod] : _roomPalette[(color >> 4) + _paletteMod];

					y++;
					if (y >= height) {
						y = 0;
						x++;
					}
				}
			} else {
				if (run == 0) {
					run = *src++;
				}

				for (z = 0; z < run; z++) {
					*(dst + y * dstPitch + x) = *(dst + y * dstPitch + x - 1);

					y++;
					if (y >= height) {
						y = 0;
						x++;
					}
				}
			}
		} else {
			run = color >> 4;
			if (run == 0) {
				run = *src++;
			}

			for (z = 0; z < run; z++) {
				*(dst + y * dstPitch + x) = _roomPalette[(color & 0xf) + _paletteMod];

				y++;
				if (y >= height) {
					y = 0;
					x++;
				}
			}
		}
	}
}

#define READ_BIT (shift--, dataBit = data & 1, data >>= 1, dataBit)
#define FILL_BITS(n) do {            \
		if (shift < n) {             \
			data |= *src++ << shift; \
			shift += 8;              \
		}                            \
	} while (0)

// NOTE: drawStripHE is actually very similar to drawStripComplex
void Gdi::drawStripHE(byte *dst, int dstPitch, const byte *src, int width, int height, const bool transpCheck) const {
	static const int delta_color[] = { -4, -3, -2, -1, 1, 2, 3, 4 };
	uint32 dataBit, data;
	byte color;
	int shift;

	color = *src++;
	data = READ_LE_UINT24(src);
	src += 3;
	shift = 24;

	int x = width;
	while (1) {
		if (!transpCheck || color != _transparentColor)
			writeRoomColor(dst, color);
		dst += _vm->_bytesPerPixel;
		--x;
		if (x == 0) {
			x = width;
			dst += dstPitch - width * _vm->_bytesPerPixel;
			--height;
			if (height == 0)
				return;
		}
		FILL_BITS(1);
		if (READ_BIT) {
			FILL_BITS(1);
			if (READ_BIT) {
				FILL_BITS(3);
				color += delta_color[data & 7];
				shift -= 3;
				data >>= 3;
			} else {
				FILL_BITS(_decomp_shr);
				color = data & _decomp_mask;
				shift -= _decomp_shr;
				data >>= _decomp_shr;
			}
		}
	}
}

#undef READ_BIT
#undef FILL_BITS


void Gdi::drawStrip3DO(byte *dst, int dstPitch, const byte *src, int height, const bool transpCheck) const {
	if (height == 0)
		return;

	int decSize = height * 8;
	int curSize = 0;

	do {
		uint8 data = *src++;
		uint8 rle = data & 1;
		int len = (data >> 1) + 1;

		len = MIN(decSize, len);
		decSize -= len;

		if (!rle) {
			for (; len > 0; len--, src++, dst++) {
				if (!transpCheck || *src != _transparentColor)
					*dst = _roomPalette[*src];
				curSize++;
				if (!(curSize & 7))
					dst += dstPitch - 8; // Next row
			}
		} else {
			byte color = *src++;
			for (; len > 0; len--, dst++) {
				if (!transpCheck || color != _transparentColor)
					*dst = _roomPalette[color];
				curSize++;
				if (!(curSize & 7))
					dst += dstPitch - 8; // Next row
			}
		}
	} while (decSize > 0);
}


#define READ_BIT (cl--, bit = bits & 1, bits >>= 1, bit)
#define FILL_BITS do {              \
		if (cl <= 8) {              \
			bits |= (*src++ << cl); \
			cl += 8;                \
		}                           \
	} while (0)

void Gdi::drawStripComplex(byte *dst, int dstPitch, const byte *src, int height, const bool transpCheck) const {
	byte color;
	MajMinCodec majMin;

	majMin.setupBitReader(_decomp_shr, src);

	byte lineBuffer[8];
	memset(lineBuffer, 0, 8);

	while (height--) {
		majMin.decodeLine(lineBuffer, 8, 1);
		for (byte i = 0; i < 8; i ++) {
			color = lineBuffer[i];
			if (!transpCheck || color != _transparentColor)
				writeRoomColor(dst, color);
			dst += _vm->_bytesPerPixel;
		}
		dst += dstPitch - 8 * _vm->_bytesPerPixel;
	}
}

void Gdi::drawStripBasicH(byte *dst, int dstPitch, const byte *src, int height, const bool transpCheck) const {
	byte color = *src++;
	uint bits = *src++;
	byte cl = 8;
	byte bit;
	int8 inc = -1;

	do {
		int x = 8;
		do {
			FILL_BITS;
			if (!transpCheck || color != _transparentColor)
				writeRoomColor(dst, color);
			dst += _vm->_bytesPerPixel;
			if (!READ_BIT) {
			} else if (!READ_BIT) {
				FILL_BITS;
				color = bits & _decomp_mask;
				bits >>= _decomp_shr;
				cl -= _decomp_shr;
				inc = -1;
			} else if (!READ_BIT) {
				color += inc;
			} else {
				inc = -inc;
				color += inc;
			}
		} while (--x);
		dst += dstPitch - 8 * _vm->_bytesPerPixel;
	} while (--height);
}

void Gdi::drawStripBasicV(byte *dst, int dstPitch, const byte *src, int height, const bool transpCheck) const {
	byte color = *src++;
	uint bits = *src++;
	byte cl = 8;
	byte bit;
	int8 inc = -1;

	int x = 8;
	do {
		int h = height;
		do {
			FILL_BITS;
			if (!transpCheck || color != _transparentColor)
				writeRoomColor(dst, color);
			dst += dstPitch;
			if (!READ_BIT) {
			} else if (!READ_BIT) {
				FILL_BITS;
				color = bits & _decomp_mask;
				bits >>= _decomp_shr;
				cl -= _decomp_shr;
				inc = -1;
			} else if (!READ_BIT) {
				color += inc;
			} else {
				inc = -inc;
				color += inc;
			}
		} while (--h);
		dst -= _vertStripNextInc;
	} while (--x);
}

#undef READ_BIT
#undef FILL_BITS

/* Ender - Zak256/Indy256 decoders */
#define READ_BIT_256                       \
		do {                               \
			if ((mask <<= 1) == 256) {     \
				buffer = *src++;           \
				mask = 1;                  \
			}                              \
			bits = ((buffer & mask) != 0); \
		} while (0)

#define READ_N_BITS(n, c)                  \
		do {                               \
			c = 0;                         \
			for (int b = 0; b < n; b++) {  \
				READ_BIT_256;              \
				c += (bits << b);          \
			}                              \
		} while (0)

#define NEXT_ROW                           \
		do {                               \
			dst += dstPitch;               \
			if (--h == 0) {                \
				if (!--x)                  \
					return;                \
				dst -= _vertStripNextInc;  \
				h = height;                \
			}                              \
		} while (0)

void Gdi::drawStripRaw(byte *dst, int dstPitch, const byte *src, int height, const bool transpCheck) const {
	int x;

	if (_vm->_game.features & GF_OLD256) {
		uint h = height;
		x = 8;
		for (;;) {
			*dst = _roomPalette[*src++];
			NEXT_ROW;
		}
	} else {
		do {
			for (x = 0; x < 8; x ++) {
				byte color = *src++;
				if (!transpCheck || color != _transparentColor)
					writeRoomColor(dst + x * _vm->_bytesPerPixel, color);
			}
			dst += dstPitch;
		} while (--height);
	}
}

void Gdi::unkDecode8(byte *dst, int dstPitch, const byte *src, int height) const {
	uint h = height;

	int x = 8;
	for (;;) {
		uint run = (*src++) + 1;
		byte color = *src++;

		do {
			*dst = _roomPalette[color];
			NEXT_ROW;
		} while (--run);
	}
}

void Gdi::unkDecode9(byte *dst, int dstPitch, const byte *src, int height) const {
	byte c, bits, color, run;
	int i;
	uint buffer = 0, mask = 128;
	int h = height;
	run = 0;

	int x = 8;
	for (;;) {
		READ_N_BITS(4, c);

		switch (c >> 2) {
		case 0:
			READ_N_BITS(4, color);
			for (i = 0; i < ((c & 3) + 2); i++) {
				*dst = _roomPalette[run * 16 + color];
				NEXT_ROW;
			}
			break;

		case 1:
			for (i = 0; i < ((c & 3) + 1); i++) {
				READ_N_BITS(4, color);
				*dst = _roomPalette[run * 16 + color];
				NEXT_ROW;
			}
			break;

		case 2:
			READ_N_BITS(4, run);
			break;

		default:
			break;
		}
	}
}

void Gdi::unkDecode10(byte *dst, int dstPitch, const byte *src, int height) const {
	int i;
	byte local_palette[256], numcolors = *src++;
	uint h = height;

	for (i = 0; i < numcolors; i++)
		local_palette[i] = *src++;

	int x = 8;

	for (;;) {
		byte color = *src++;
		if (color < numcolors) {
			*dst = _roomPalette[local_palette[color]];
			NEXT_ROW;
		} else {
			uint run = color - numcolors + 1;
			color = *src++;
			do {
				*dst = _roomPalette[color];
				NEXT_ROW;
			} while (--run);
		}
	}
}


void Gdi::unkDecode11(byte *dst, int dstPitch, const byte *src, int height) const {
	int bits, i;
	uint buffer = 0, mask = 128;
	byte inc = 1, color = *src++;

	int x = 8;
	do {
		int h = height;
		do {
			*dst = _roomPalette[color];
			dst += dstPitch;
			for (i = 0; i < 3; i++) {
				READ_BIT_256;
				if (!bits)
					break;
			}
			switch (i) {
			case 1:
				inc = -inc;
				color -= inc;
				break;

			case 2:
				color -= inc;
				break;

			case 3:
				inc = 1;
				READ_N_BITS(8, color);
				break;

			default:
				break;
			}
		} while (--h);
		dst -= _vertStripNextInc;
	} while (--x);
}

#undef NEXT_ROW
#undef READ_BIT_256

#ifdef USE_RGB_COLOR
void GdiHE16bit::writeRoomColor(byte *dst, byte color) const {
	WRITE_UINT16(dst, READ_LE_UINT16(_vm->_hePalettes + 2048 + color * 2));
}
#endif

void Gdi::writeRoomColor(byte *dst, byte color) const {
	// As described in bug #2204 "FOA/Amiga: Palette problem (Regression)"
	// the original AMIGA version of Indy4: The Fate of Atlantis allowed
	// overflowing of the palette index. To have the same result in our code,
	// we need to do an logical AND 0xFF here to keep the result in [0, 255].
	*dst = _roomPalette[(color + _paletteMod) & 0xFF];
}


#pragma mark -
#pragma mark --- Transition effects ---
#pragma mark -

void ScummEngine::fadeIn(int effect) {
	if (_disableFadeInEffect || (_game.id == GID_MANIAC && _game.platform == Common::kPlatformAmiga)) {
		// fadeIn() calls can be disabled in TheDig after a SMUSH movie
		// has been played. Like the original interpreter, we introduce
		// an extra flag to handle this.

		// Screen fades are also disabled in the Amiga version of
		// Maniac Mansion, verified on WinUAE
		_disableFadeInEffect = false;
		_doEffect = false;
		_screenEffectFlag = true;
		return;
	}
	towns_waitForScroll(0);
	updatePalette();

	switch (effect) {
	case 0:
		// seems to do nothing
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		// Some of the transition effects won't work properly unless
		// the screen is marked as clean first. At first I thought I
		// could safely do this every time fadeIn() was called, but
		// that broke the FOA intro. Probably other things as well.
		//
		// Hopefully it's safe to do it at this point, at least.
		_virtscr[kMainVirtScreen].setDirtyRange(0, 0);
		transitionEffect(effect - 1);
		break;
	case 128:
		dissolveEffectSelector();
		break;
	case 129:
		break;
	case 130:
	case 131:
	case 132:
	case 133:
		scrollEffect(133 - effect);
		break;
	case 134:
		dissolveEffect(1, 1);
		break;
	case 135:
		dissolveEffect(1, _virtscr[kMainVirtScreen].h);
		break;
	default:
		error("Unknown screen effect, %d", effect);
	}
	_screenEffectFlag = true;
}

void ScummEngine::fadeOut(int effect) {
	// Screen fades are disabled in the Amiga version of
	// Maniac Mansion, verified on WinUAE
	if (_game.id == GID_MANIAC && _game.platform == Common::kPlatformAmiga) {
		_doEffect = false;
		_screenEffectFlag = false;
		return;
	}

	towns_waitForScroll(0);

	VirtScreen *vs = &_virtscr[kMainVirtScreen];
	vs->setDirtyRange(0, 0);
	if (_game.version < 7)
		camera._last.x = camera._cur.x;

#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
	if (_game.version == 3 && _game.platform == Common::kPlatformFMTowns)
		_textSurface.fillRect(Common::Rect(0, vs->topline * _textSurfaceMultiplier, _textSurface.pitch, (vs->topline + vs->h) * _textSurfaceMultiplier), 0);
#endif

	// V0 wipes the text area before fading out
	if (_game.version == 0) {
		updateDirtyScreen(kTextVirtScreen);
	}

	// TheDig can disable fadeIn(), and may call fadeOut() several times
	// successively. Disabling the _screenEffectFlag check forces the screen
	// to get cleared. This fixes glitches, at least, in the first cutscenes
	// when bypassed of FT and TheDig.
	if ((_game.version == 7 || _screenEffectFlag) && effect != 0) {
		// Fill screen 0 with black
		if (_game.platform == Common::kPlatformNES)
			memset(vs->getPixels(0, 0), 0x1d, vs->pitch * vs->h);
		else
			memset(vs->getPixels(0, 0), 0, vs->pitch * vs->h);

		// Fade to black with the specified effect, if any.
		switch (effect) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			transitionEffect(effect - 1);
			break;
		case 128:
			dissolveEffectSelector();
			break;
		case 129:
			// Just blit screen 0 to the display (i.e. display will be black)
			vs->setDirtyRange(0, vs->h);
			updateDirtyScreen(kMainVirtScreen);
			towns_updateGfx();
			break;
		case 134:
			dissolveEffect(1, 1);
			break;
		case 135:
			dissolveEffect(1, _virtscr[kMainVirtScreen].h);
			break;
		default:
			error("fadeOut: default case %d", effect);
		}
	}

	_screenEffectFlag = false;
}

/**
 * Perform a transition effect. There are four different effects possible:
 * 0: Iris effect
 * 1: Box wipe (a black box expands from the upper-left corner to the lower-right corner)
 * 2: Box wipe (a black box expands from the lower-right corner to the upper-left corner)
 * 3: Inverse box wipe
 * All effects operate on 8x8 blocks of the screen. These blocks are updated
 * in a certain order; the exact order determines how the effect appears to the user.
 * @param a		the transition effect to perform
 */
void ScummEngine::transitionEffect(int a) {
	int delta[16]; // Offset applied during each iteration
	int tab_2[16];
	int i, j;
	int bottom;
	int l, t, r, b;
	int delay, numOfIterations;
	const int height = MIN((int)_virtscr[kMainVirtScreen].h, _screenHeight);

	if (VAR_FADE_DELAY == 0xFF) {
		if (_game.platform == Common::kPlatformC64) {
			delay = kC64Delay;
		} else if (_game.version >= 2) {
			delay = kPictureDelay;
		} else {
			delay = kNoDelay;
		}
	} else {
		delay = VAR(VAR_FADE_DELAY);
	}

	// Older Amiga games handle transition timing a whole frame
	// at a time instead of using quarter frames; the following
	// code gives my best approximation of that behavior
	// and the resulting timing.
	if (_game.platform == Common::kPlatformAmiga) {
		if (_game.id == GID_ZAK) {
			delay = kPictureDelay;
		} else {
			int amigaRest = (delay % 4);
			delay = (delay / 4);
			if (amigaRest > 0) {
				delay += 1;
			}
			delay *= 10;
		}
	}

	// V3+ games have the number of iterations hardcoded; we also
	// have that number hardcoded for MM NES, so let's target that too:
	if (_game.version >= 3 || _game.platform == Common::kPlatformNES) {
		numOfIterations = transitionEffects[a].numOfIterations;
	} else {
		numOfIterations = (a == 0 || a == 4) ? ceil((double)height / 16.0) : height / 8;
	}

	for (i = 0; i < 16; i++) {
		delta[i] = transitionEffects[a].deltaTable[i];
		j = transitionEffects[a].stripTable[i];
		if (j == 24)
			j = height / 8 - 1;
		tab_2[i] = j;
	}

	bottom = height / 8;
	for (j = 0; j < numOfIterations; j++) {
		for (i = 0; i < 4; i++) {
			l = tab_2[i * 4];
			t = tab_2[i * 4 + 1];
			r = tab_2[i * 4 + 2];
			b = tab_2[i * 4 + 3];

			if (t == b) {
				while (l <= r) {
					if (l >= 0 && l < _gdi->_numStrips && t < bottom) {
						_virtscr[kMainVirtScreen].tdirty[l] = _screenTop + t * 8;
						_virtscr[kMainVirtScreen].bdirty[l] = _screenTop + (b + 1) * 8;
					}
					l++;
				}
			} else {
				if (l < 0 || l >= _gdi->_numStrips || b <= t)
					continue;
				if (b > bottom)
					b = bottom;
				if (t < 0)
					t = 0;
				_virtscr[kMainVirtScreen].tdirty[l] = _screenTop + t * 8;
				_virtscr[kMainVirtScreen].bdirty[l] = _screenTop + (b + 1) * 8;
			}
			updateDirtyScreen(kMainVirtScreen);
		}

		for (i = 0; i < 16; i++)
			tab_2[i] += delta[i];

		// Draw the current state to the screen and wait
		// for the appropriate number of quarter frames
		if (!_fastMode) {
			waitForTimer(delay, true);
		}
	}
}

/**
 * Update width*height areas of the screen, in random order, until the whole
 * screen has been updated. For instance:
 *
 * dissolveEffect(1, 1) produces a pixel-by-pixel dissolve
 * dissolveEffect(8, 8) produces a square-by-square dissolve
 * dissolveEffect(virtsrc[0].width, 1) produces a line-by-line dissolve
 */
void ScummEngine::dissolveEffect(int width, int height) {
	// Apparently Mac versions discarded this effect
	if (_macScreen)
		return;

	VirtScreen *vs = &_virtscr[kMainVirtScreen];
	int *offsets;
	int blitsBeforeRefresh, blits, blitsToFreeze;
	int x, y;
	int w, h;
	int i;
	bool canHalt = false;
	bool is1x1Pattern = (width == 1 && height == 1);
	// There's probably some less memory-hungry way of doing this. But
	// since we're only dealing with relatively small images, it shouldn't
	// be too bad.

	w = vs->w / width;
	h = vs->h / height;

	// When used correctly, vs->width % width and vs->height % height
	// should both be zero, but just to be safe...

	if (vs->w % width)
		w++;

	if (vs->h % height)
		h++;

	offsets = (int *) malloc(w * h * sizeof(int));
	if (offsets == nullptr)
		error("dissolveEffect: out of memory");

	// Create a permutation of offsets into the frame buffer

	if (width == 1 && height == 1) {
		// Optimized case for pixel-by-pixel dissolve

		for (i = 0; i < vs->w * vs->h; i++)
			offsets[i] = i;

		for (i = 1; i < w * h; i++) {
			int j;

			j = _rnd.getRandomNumber(i - 1);
			offsets[i] = offsets[j];
			offsets[j] = i;
		}
	} else {
		int *offsets2;

		for (i = 0, x = 0; x < vs->w; x += width)
			for (y = 0; y < vs->h; y += height)
				offsets[i++] = y * vs->pitch + x;

		offsets2 = (int *) malloc(w * h * sizeof(int));
		if (offsets2 == nullptr)
			error("dissolveEffect: out of memory");

		memcpy(offsets2, offsets, w * h * sizeof(int));

		for (i = 1; i < w * h; i++) {
			int j;

			j = _rnd.getRandomNumber(i - 1);
			offsets[i] = offsets[j];
			offsets[j] = offsets2[i];
		}

		free(offsets2);
	}

	// The whole effect has a variable duration, depending on
	// the host machine speed. We want to be accurate, but we
	// also want to actually *see* the effect, given that in modern
	// machines this code would run at lightspeed, so let's declare
	// a blitsBeforeRefresh variable, which serves as a threshold
	// value allowing us to pause rendering every N blits.
	//
	// Pattern 1x1:
	//   The original construct the image piece by piece but blits it
	//   every 8 iterations of a loop with duration h. We try to imitate
	//   this behavior by using blitsBeforeRefresh and by assigning it a value
	//   which is a factor of blitsToFreeze.
	//
	// Pattern NxM:
	//   The original construct the image piece by piece but blits it
	//   every time it finds an offset smaller than the height of the virtual
	//   screen. This is trivial to do in our code, so we just sleep for a
	//   quarter frame every time the condition above is met.
	//
	// If we ever get a blitsToFreeze == 0, we will use 18 in its place
	// since it's the most typical value got out of the calculations.

	blits = 0;
	blitsToFreeze = (h / 8); // The number of blits between which we delay rendering for pattern 1x1
	blitsBeforeRefresh = (w * h) / (blitsToFreeze == 0 ? 18 : blitsToFreeze);

	for (i = 0; i < w * h; i++) {
		x = offsets[i] % vs->pitch;
		y = offsets[i] / vs->pitch;

#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
		if (_game.platform == Common::kPlatformFMTowns)
			towns_drawStripToScreen(vs, x, y + vs->topline, x, y, width, height);
		else
#endif
		if (IS_ALIGNED(width, 4))
			drawStripToScreen(vs, x, width, y, y + height);
		else {
			const byte *src = vs->getPixels(x, y);
			int pitch = vs->pitch;
			y += vs->topline;
			int wd = width;
			int ht = height;

			if (_enableEGADithering) {
				if (is1x1Pattern) {
					*_compositeBuf = *src;
				} else {
					for (int ii = 0; ii < height; ++ii) {
						memcpy(_compositeBuf + width * ii, src, width);
						src += pitch;
					}
				}
				pitch = width;
				src = ditherVGAtoEGA(pitch, x, y, wd, ht);
			}

			_system->copyRectToScreen(src, pitch, x, y, wd, ht);
		}

		// Test for 1x1 pattern...
		canHalt |= is1x1Pattern && ++blits >= blitsBeforeRefresh;

		// If that is true, then reset the blits var...
		if (canHalt) {
			blits = 0;
		}

		// Test for NxM pattern...
		canHalt |= !is1x1Pattern && (offsets[i] < vs->h);

		// Halt rendering for a quarter frame (or a whole frame in case of Amiga)...
		if (canHalt) {
			canHalt = false;
			if (_game.platform == Common::kPlatformAmiga) {
				waitForTimer(4, true);
			} else {
				waitForTimer(1, true);
			}
		}
	}

	free(offsets);
}

void ScummEngine::scrollEffect(int dir) {
#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
	// The FM-Towns versions use smooth scrolling here, but only for left and right.
	if (_enableSmoothScrolling && dir > 1) {
		towns_scriptScrollEffect((dir & 1) * 2 - 1);
		return;
	}
#endif
	VirtScreen *vs = &_virtscr[kMainVirtScreen];

	int x, y;
	const int step = 8;

	// Keep in mind: this effect is only present in v5 and v6, so VAR_FADE_DELAY is
	// never uninitialized. The following check is here for good measure only.
	int delay = (VAR_FADE_DELAY != 0xFF) ? VAR(VAR_FADE_DELAY) : (int)kPictureDelay;

	// Amiga handles timing a whole frame at a time
	// instead of using quarter frames; the following
	// code gives my best approximation of that behavior
	// and the resulting timing
	if (_game.platform == Common::kPlatformAmiga) {
		int amigaRest = (delay % 4);
		delay = (delay / 4);
		if (amigaRest > 0) {
			delay += 1;
		}
		delay *= 10;
	}

	byte *src;
	int m = _textSurfaceMultiplier;

	if (m == 1 && _game.platform == Common::kPlatformMacintosh && _macScreen)
		m = 2;

	int vsPitch = vs->pitch;

	switch (dir) {
	case 0:
		//up
		y = 1 + step;
		while (y < vs->h) {
			moveScreen(0, -step * m, vs->h * m);
#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
			if (_townsScreen) {
				towns_drawStripToScreen(vs, 0, vs->topline + vs->h - step, 0, y - step, vs->w, step);
			} else
#endif
			{
				src = vs->getPixels(0, y - step);
				if (_macScreen) {
					mac_drawBufferToScreen(src, vsPitch, 0, (vs->h - step), vs->w, step, false);
				} else {
					_system->copyRectToScreen(src,
											  vsPitch * m,
											  0, (vs->h - step) * m,
											  vs->w * m, step * m);
				}
			}

			waitForTimer(delay, true);
			y += step;
		}
		break;
	case 1:
		// down
		y = 1 + step;
		while (y < vs->h) {
			moveScreen(0, step * m, vs->h * m);
#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
			if (_townsScreen) {
				towns_drawStripToScreen(vs, 0, vs->topline, 0, vs->h - y, vs->w, step);
			} else
#endif
			{
				src = vs->getPixels(0, vs->h - y);

				if (_macScreen) {
					mac_drawBufferToScreen(src, vsPitch, 0, 0, vs->w, step, false);
				} else {
					_system->copyRectToScreen(src,
											  vsPitch * m,
											  0, 0,
											  vs->w * m, step * m);
				}
			}

			waitForTimer(delay, true);
			y += step;
		}
		break;
	case 2:
		// left
		x = 1 + step;
		while (x < vs->w) {
			moveScreen(-step * m, 0, vs->h * m);
#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
			if (_townsScreen) {
				towns_drawStripToScreen(vs, vs->w - step, vs->topline, x - step, vs->topline, step, vs->h);
			} else
#endif
			{
				src = vs->getPixels(x - step, 0);
				if (_macScreen) {
					mac_drawBufferToScreen(src, vsPitch, (vs->w - step), 0, step, vs->h, false);
				} else {
					_system->copyRectToScreen(src, vsPitch * m, (vs->w - step) * m, 0, step * m, vs->h * m);
				}
			}
			waitForTimer(delay, true);
			x += step;
		}
		break;
	case 3:
		// right
		x = 1 + step;
		while (x < vs->w) {
			moveScreen(step * m, 0, vs->h * m);
#ifndef DISABLE_TOWNS_DUAL_LAYER_MODE
			if (_townsScreen) {
				towns_drawStripToScreen(vs, 0, vs->topline, vs->w - x, vs->topline, step, vs->h);
			} else
#endif
			{
				src = vs->getPixels(vs->w - x, 0);
				if (_macScreen) {
					mac_drawBufferToScreen(src, vsPitch, 0, 0, step, vs->h, false);
				} else {
					_system->copyRectToScreen(src, vsPitch * m, 0, 0, step * m, vs->h * m);
				}
			}

			waitForTimer(delay, true);
			x += step;
		}
		break;
	default:
		break;
	}
}

void ScummEngine::dissolveEffectSelector() {
	// CD Loom (but not EGA Loom!) uses a more fine-grained dissolve
	if (_game.id == GID_LOOM && _game.version == 4) {
		dissolveEffect(1, 1);
	} else if (_game.id == GID_LOOM && _game.platform == Common::kPlatformPCEngine) {
		dissolveEffect(8, 8);
	} else if (_game.platform == Common::kPlatformMacintosh) {
		if (_game.version == 3) {
			transitionEffect(0);
		} else {
			return;
		}
	} else {
		dissolveEffect(8, 4);
	}
}

void ScummEngine::updateScreenShakeEffect() {
	if (!_shakeEnabled) {
		if (_shakeFrame) {
			_shakeFrame = 0;
			_system->setShakePos(0, 0);
		}
		_shakeNextTick = _shakeTickCounter = 0;
		return;
	}

	uint32 now = _system->getMillis();
	if (!_shakeNextTick)
		_shakeNextTick = now;

	while (now >= _shakeNextTick) {
		_shakeFrame = (_shakeFrame + 1) % NUM_SHAKE_POSITIONS;
		_system->setShakePos(0, -shake_positions[_shakeFrame] * _textSurfaceMultiplier);
		// In DOTT (and probably all other imuse games) this runs on the imuse timer which is a PIT 0 Timer at 291.304 Hz.
		// Apparently it is the same timer setting for all sound drivers although it is set up not in the main executable
		// but inside each respective ims driver during the driver load/init process. The screen shakes update every 8 ticks.
		// LOOM uses either 236.696 Hz at 8 ticks delay or 473.297 Hz at 16 ticks delay, depending on the sound card selection.
		// The outcome is the same...
		_shakeTickCounter += ((1000000 / _shakeTimerRate) * 8);
		_shakeNextTick += (_shakeTickCounter / 1000);
		_shakeTickCounter %= 1000;
	}
}

void MajMinCodec::setupBitReader(byte shift, const byte *src) {
	_majMinData.repeatMode = false;
	_majMinData.numBits = 16;
	_majMinData.shift = shift;
	_majMinData.color = *src;
	_majMinData.bits = (*(src + 1) | *(src + 2) << 8);
	_majMinData.dataPtr = src + 3;
}

#define MAJMIN_FILL_BITS()                                        \
		if (_majMinData.numBits <= 8) {                                \
		  _majMinData.bits |= (*_majMinData.dataPtr++) << _majMinData.numBits;   \
		  _majMinData.numBits += 8;                                    \
		}

#define MAJMIN_EAT_BITS(n)                                        \
		_majMinData.numBits -= (n);                                    \
		_majMinData.bits >>= (n);

byte MajMinCodec::readBits(byte n) {
	MAJMIN_FILL_BITS();
	byte _value = _majMinData.bits & ((1 << n) - 1);
	MAJMIN_EAT_BITS(n);
	return _value;
}

void MajMinCodec::skipData(int32 numbytes) {
	decodeLine(nullptr, numbytes, 0);
}

void MajMinCodec::decodeLine(byte *buf, int32 numbytes, int32 dir) {
	byte diff;

	while (numbytes != 0) {
		if (buf) {
			*buf = _majMinData.color;
			buf += dir;
		}

		if (!_majMinData.repeatMode) {
			if (readBits(1)) {
				if (readBits(1)) {
					diff = readBits(3) - 4;
					if (diff) {
						// A color change
						_majMinData.color += diff;
					} else {
						// Color does not change, but rather identical pixels get repeated
						_majMinData.repeatMode = true;
						_majMinData.repeatCount = readBits(8) - 1;
					}
				} else {
					_majMinData.color = readBits(_majMinData.shift);
				}
			}
		} else {
			if (--_majMinData.repeatCount == 0) {
				_majMinData.repeatMode = false;
			}
		}
		numbytes--;
	}
}

} // End of namespace Scumm
