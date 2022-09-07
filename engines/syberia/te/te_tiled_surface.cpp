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

#include "syberia/syberia.h"

#include "syberia/te/te_tiled_surface.h"
#include "syberia/te/te_frame_anim.h"
#include "syberia/te/te_resource_manager.h"

namespace Syberia {

static void getRangeIntersection(float start1,float end1,float start2,float end2,float *pstart,float *pend) {
	*pstart = MAX(start1, start2);
	*pend = MIN(end1, end2);
}

TeTiledSurface::TeTiledSurface() : _colorKeyActive(false), _colorKeyTolerence(0),
_bottomCrop(0), _topCrop(0), _leftCrop(0), _rightCrop(0), _codec(nullptr), _imgFormat(TeImage::INVALID) {
	_frameAnim.frameChangedSignal().add<TeTiledSurface>(this, &TeTiledSurface::onFrameAnimCurrentFrameChanged);
}

void TeTiledSurface::cont() {
	_frameAnim.cont();
}

void TeTiledSurface::draw() {
	if (_tiledTexture && _tiledTexture->isLoaded())
		TeModel::draw();
}

byte TeTiledSurface::isLoaded() {
	return _tiledTexture && _tiledTexture->isLoaded();
}

bool TeTiledSurface::load(const Common::Path &path) {
	//(**(code **)((long)(this->super).vptr + 0x1c0))(this);

	TeResourceManager *resmgr = g_engine->getResourceManager();
	_path = path;

	Common::SharedPtr<TeTiledTexture> texture;
	if (resmgr->exists(path.append(".tt"))) {
		texture = resmgr->getResource<TeTiledTexture>(path.append(".tt"));
	}

	if (!texture) {
		TeCore *core = g_engine->getCore();

		_codec = core->createVideoCodec(path);
		if (!_codec)
			return false;

		texture.reset(new TeTiledTexture());

		if (_codec->load(path)) {
			texture->setAccessName(path);
			resmgr->addResource(texture.get());
			_imgFormat = _codec->imageFormat();

			if (_imgFormat == TeImage::INVALID) {
				warning("TeTiledSurface::load: Wrong image format on file %s", path.toString().c_str());
				return false;
			}
			TeImage img;
			TeVector2s32 newSize = Te3DTexture::optimisedSize(TeVector2s32(_codec->width(), _codec->height()));
			int bufy = _codec->height() + 4;
			if (newSize._y < (int)_codec->height() + 4) {
			  bufy = newSize._y;
			}
			int bufx = _codec->width() + 4;
			if (newSize._x < (int)_codec->width() + 4) {
			  bufx = newSize._x;
			}

			Common::SharedPtr<TePalette> nullpal;
			img.create(_codec->width(), _codec->height(), nullpal, (TeImage::Format)_imgFormat, bufx, bufy);

			if (_codec->update(0, img)) {
				texture->load(img);
			}
	  }
	}

	setTiledTexture(texture);
	return true;
}

bool TeTiledSurface::load(const TeImage &image) {
	error("TODO: Implement me TeTiledSurface::load");
}

bool TeTiledSurface::load(const Common::SharedPtr<Te3DTexture> &texture) {
	error("TODO: Implement me TeTiledSurface::load");
}

bool TeTiledSurface::onFrameAnimCurrentFrameChanged() {
   error("TODO: Implement me TeTiledSurface::onFrameAnimCurrentFrameChanged");
}

void TeTiledSurface::pause() {
	_frameAnim.pause();
}

void TeTiledSurface::play() {
	if (_codec) {
		_frameAnim._nbFrames = _codec->nbFrames();
		_frameAnim._frameRate = _codec->frameRate();
		_frameAnim.play();
	}
}

void TeTiledSurface::setColorKey(const TeColor &col) {
	_colorKey = col;
	if (_codec)
		_codec->setColorKey(col);
}

void TeTiledSurface::setColorKeyActivated(bool val) {
	_colorKeyActive = true;
	if (_codec)
		_codec->setColorKeyActivated(val);
}

void TeTiledSurface::setColorKeyTolerence(float val) {
	_colorKeyTolerence = val;
	if (_codec)
		_codec->setColorKeyTolerence(val);
}

void TeTiledSurface::setTiledTexture(const Common::SharedPtr<TeTiledTexture> &texture) {
	_tiledTexture = texture;
	if (texture) {
		_meshes.resize(texture->numberOfColumns() * texture->numberOfRow());

		setAccessName(texture->getAccessName().append(".surface"));
		updateSurface();
	} else {
		_meshes.clear();
	}
}

void TeTiledSurface::stop() {
	_frameAnim.stop();
}

void TeTiledSurface::unload() {
	if (_codec) {
		delete _codec;
		_codec = nullptr;
	}
	setTiledTexture(Common::SharedPtr<TeTiledTexture>());
}

void TeTiledSurface::update(const TeImage &image) {
	_tiledTexture->update(image);
	setTiledTexture(_tiledTexture);
}

void TeTiledSurface::updateSurface() {
	if (!_tiledTexture)
		return;

	const long cols = _tiledTexture->numberOfColumns();
	const long rows = _tiledTexture->numberOfRow();
	for (long row = 0; row < rows; row++) {
		for (long col = 0; col < cols; col++) {

			error("TODO: Implement me TeTiledSurface::updateSurface (%ld, %ld)", rows, cols);
		}
	}
}

void TeTiledSurface::updateVideoProperties() {
	if (_codec) {
		_codec->setColorKeyActivated(_colorKeyActive);
		_codec->setColorKey(_colorKey);
		_codec->setColorKeyTolerence(_colorKeyTolerence);
	}
}

// TODO: Add more functions here.

} // end namespace Syberia
