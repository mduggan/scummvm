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

#include "common/textconsole.h"
#include "syberia/syberia.h"
#include "syberia/te/te_visual_fade.h"
#include "syberia/te/te_renderer.h"

namespace Syberia {

TeVisualFade::TeVisualFade() {
}

void TeVisualFade::animateBlackFade() {
	error("TODO: Implement me.");
}

void TeVisualFade::animateFade() {
	error("TODO: Implement me.");
}

void TeVisualFade::captureFrame() {
	error("TODO: Implement me.");
}

void TeVisualFade::init() {
	_fadeCaptureSprite.setName("fadeCaptureSprite");
	_fadeCaptureSprite.setSizeType(TeLayout::CoordinatesType::RELATIVE_TO_PARENT);
	_fadeCaptureSprite.setSize(TeVector3f32(1.0, 1.0, 0.0));
	_fadeCaptureSprite.setVisible(false);

	_blackFadeSprite.setName("blackFadeSprite");
	_blackFadeSprite.setSizeType(TeLayout::CoordinatesType::RELATIVE_TO_PARENT);
	_blackFadeSprite.setSize(TeVector3f32(2.0, 2.0, 0.0));
	_blackFadeSprite.load("pictures/black64x64.png");
	_blackFadeSprite.setColor(TeColor(0xff, 0xff, 0xff, 0));
	_blackFadeSprite.setVisible(false);

	_texturePtr.reset(new Te3DTexture());
	/*	 
	 // create an image the size of the window, no palette, format 6.
	_image.create(windowX, windowY, nullptr, 6);
	 */
	//_texturePtr->load(image);
	g_engine->getRenderer()->enableTexture();
	//_texturePtr->load(image);
}


} // end namespace Syberia
