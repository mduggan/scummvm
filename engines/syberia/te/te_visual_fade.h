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

#ifndef SYBERIA_TE_TE_VISUAL_FADE_H
#define SYBERIA_TE_TE_VISUAL_FADE_H

#include "common/ptr.h"

#include "syberia/te/te_3d_texture.h"
#include "syberia/te/te_curve_anim2.h"
#include "syberia/te/te_sprite_layout.h"
#include "syberia/te/te_button_layout.h"

namespace Syberia {

class TeVisualFade : public TeSpriteLayout {
public:
	TeVisualFade();

	void animateBlackFade();
	void animateFade();
	void captureFrame();
	void clear() {};
	void init();

	TeSpriteLayout _fadeCaptureSprite;
	TeSpriteLayout _blackFadeSprite;
	TeButtonLayout _buttonLayout;

private:

	TeIntrusivePtr<Te3DTexture> _texturePtr;
	TeCurveAnim2<Te3DObject2, TeColor> _animateFadeCurveAnim;
	TeImage _image;
	// TODO add private members

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_VISUAL_FADE_H
