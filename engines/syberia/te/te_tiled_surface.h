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

#ifndef SYBERIA_TE_TE_TILED_SURFACE_H
#define SYBERIA_TE_TE_TILED_SURFACE_H

#include "common/ptr.h"
#include "common/path.h"

#include "syberia/te/te_model.h"
#include "syberia/te/te_frame_anim.h"
#include "syberia/te/te_image.h"
#include "syberia/te/te_3d_texture.h"
#include "syberia/te/te_tiled_texture.h"
#include "syberia/te/te_i_codec.h"
#include "syberia/te/te_intrusive_ptr.h"

namespace Syberia {

class TeTiledSurface : public TeModel, public TeResource {
public:
	TeTiledSurface();

	virtual int bufferSize() { return 1; } // unused?
	void cont();
	void draw();
	virtual void entry() {};
	byte isLoaded();
	bool load(const Common::Path &path);
	bool load(const TeImage &image);
	bool load(const TeIntrusivePtr<Te3DTexture> &texture);

	bool onFrameAnimCurrentFrameChanged();
	void pause();
	void play();

	void setBottomCropping(float val) {
		_bottomCrop = val;
		updateSurface();
	}
	void setLeftCropping(float val) {
		_leftCrop = val;
		updateSurface();
	}
	void setRightCropping(float val) {
		_rightCrop = val;
		updateSurface();
	}
	void setTopCropping(float val) {
		_topCrop = val;
		updateSurface();
	}

	virtual void setBufferSize(long size) {}; // unused?

	void setColorKey(const TeColor &col);
	void setColorKeyActivated(bool val);
	void setColorKeyTolerence(float val);
	void setTiledTexture(const TeIntrusivePtr<TeTiledTexture> &texture);

	void stop();
	void unload();
	void update(const TeImage &image);
	void updateSurface();
	void updateVideoProperties();

	TeFrameAnim _frameAnim;

private:
	float _bottomCrop;
	float _leftCrop;
	float _rightCrop;
	float _topCrop;

	TeColor _colorKey;
	bool _colorKeyActive;
	float _colorKeyTolerence;
	
	bool _shouldDraw;

	TeImage::Format _imgFormat;

	Common::Path _path;

	TeICodec *_codec;
	// TODO add private members

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_TILED_SURFACE_H
