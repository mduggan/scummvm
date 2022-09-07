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

#include "graphics/opengl/glad.h"

#include "syberia/syberia.h"
#include "syberia/te/te_3d_texture.h"
#include "syberia/te/te_resource_manager.h"
#include "syberia/te/te_renderer.h"

namespace Syberia {

static const uint NO_TEXTURE = 0xffffffff;

Te3DTexture::Te3DTexture() : _glTexture(NO_TEXTURE), _createdTexture(false), _numFrames(1), _frameRate(0) {
}

void Te3DTexture::bind() {
	TeRenderer *renderer = g_engine->getRenderer();
	glBindTexture(GL_TEXTURE_2D, _glTexture);
	renderer->setMatrixMode(TeRenderer::MM_GL_TEXTURE);
	renderer->loadMatrix(_matrix);
	TeMatrix4x4 matrix = renderer->currentMatrix();
	renderer->loadMatrixToGL(matrix);
	renderer->setMatrixMode(TeRenderer::MM_GL_MODELVIEW);
}

void Te3DTexture::copyCurrentRender(uint xoffset, uint yoffset, uint x, uint y) {
	error("TODO: Implmement Te3DTexture::copyCurrentRender");
}

void Te3DTexture::create() {
	_flipY = false;
	_translateX = _translateY = _texWidth = _texHeight = 0;
	_somethingOffsetX = _somethingOffsetY = _width = _height = 0;
	_format = TeImage::INVALID;
	_loaded = 0;
	if (!_createdTexture)
		glGenTextures(1, &_glTexture);
	if (_glTexture == NO_TEXTURE) {
		_createdTexture = false;
		return;
	}

	_createdTexture = true;
	glBindTexture(GL_TEXTURE_2D, _createdTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Te3DTexture::destroy() {
	if (_createdTexture) {
	  glDeleteTextures(1, &_glTexture);
	}
	_createdTexture = false;
	_loaded = false;
}

void Te3DTexture::ForceTexData(uint gltextures, uint xsize, uint ysize) {
	error("TODO: Implement me");
}

bool Te3DTexture::hasAlpha() {
	uint format = getFormat();
	return (format == 6 || format == 9 || format == 0xb || format == 1 || format == 0);
}

/*static*/
Common::SharedPtr<Te3DTexture> Te3DTexture::load2(const Common::Path &path, uint size) {
	Common::Path fullPath = path.append(".3dtex");
	
	TeResourceManager *resMgr = g_engine->getResourceManager();
	if (!resMgr->exists(fullPath)) {
		Common::SharedPtr<Te3DTexture> retval(new Te3DTexture());
		retval->load(fullPath);
		retval->setAccessName(fullPath);
		resMgr->addResource(retval.get());
		return retval;
	} else {
		return resMgr->getResource<Te3DTexture>(fullPath);
	}
}

bool Te3DTexture::load(const Common::Path &path) {
	TeResourceManager *resmgr = g_engine->getResourceManager();
	Common::SharedPtr<TeImage> img = resmgr->getResource<TeImage>(path);
	load(*img);
	setAccessName(path.append(".3dtex"));
	return true;
}

bool Te3DTexture::load(const TeImage &img) {
	Common::Path accessName = img.getAccessName();
	setAccessName(accessName.append(".3dtex"));
	
	_width = img.w;
	_height = img.h;
	warning("set some other fields from the image here.");
	
	TeVector2s32 optimizedSz = optimisedSize(img.bufSize());
	_texWidth = optimizedSz._x;
	_texHeight = optimizedSz._y;

	glBindTexture(GL_TEXTURE_2D, _glTexture);
	glPixelStorei(GL_UNPACK_SWAP_BYTES, 0);
	glPixelStorei(GL_UNPACK_LSB_FIRST, 0);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	const void *imgdata = img.getPixels();
	if (_format == 5) {
		GLenum glpxformat = GL_RGB;
		if (_glPixelFormat != GL_INVALID_ENUM) {
			glpxformat = _glPixelFormat;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, glpxformat, _texWidth, _texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.pitch, img.h,
						GL_RGB, GL_UNSIGNED_BYTE, imgdata);
		glpxformat = GL_RGB;
	} else if (_format == 6) {
		GLenum glpxformat = GL_RGBA;
		if (_glPixelFormat != GL_INVALID_ENUM) {
			glpxformat = _glPixelFormat;
		}
	  	glTexImage2D(GL_TEXTURE_2D, 0, glpxformat, _texWidth, _texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img.pitch, img.h,
						GL_RGBA, GL_UNSIGNED_BYTE, imgdata);
	} else {
		warning("Te3DTexture::load can't send image format %d to GL.", _format);
	}
	
	_matrix.setToIdentity();

	_matrix.scale(TeVector3f32((float)_width / _texWidth, (float)_height / _texHeight, 1.0));
	_matrix.translate(TeVector3f32((float)_translateX / _width, (float)_translateY / _height, 0.0));
	_matrix.scale(TeVector3f32(1.0 - (float)(_somethingOffsetX + _translateX) / _width,
			   1.0 - (float)(_somethingOffsetY + _translateY) / _height, 1.0));
	if (_flipY) {
		_matrix.translate(TeVector3f32(0.0, 1.0, 0.0));
		_matrix.translate(TeVector3f32(1.0, -1.0, 1.0));
	}
	_loaded = true;
	return true;
}

/*static*/
TeVector2s32 Te3DTexture::optimisedSize(const TeVector2s32 &size) {
	warning("TODO: Te3DTexture::optimizedSize. Returning original size for now.");
	return size;
}

void Te3DTexture::unbind() {
	TeMatrix4x4 matrix;
	TeRenderer *renderer = g_engine->getRenderer();
	renderer->setMatrixMode(TeRenderer::MM_GL_TEXTURE);
	renderer->loadMatrix(matrix);
	matrix = renderer->currentMatrix();
	glLoadMatrixf(matrix.getData());
	glBindTexture(GL_TEXTURE_2D,0);
	renderer->setMatrixMode(TeRenderer::MM_GL_MODELVIEW);
}

bool Te3DTexture::unload() {
	glBindTexture(GL_TEXTURE_2D, _glTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	_loaded = false;
	return true;
}

void Te3DTexture::update(const TeImage &img, uint xoff, uint yoff) {
	if (!img.w || !img.h)
		return;

	setAccessName(img.getAccessName().append(".3dtex"));
	glBindTexture(GL_TEXTURE_2D, _glTexture);
	glPixelStorei(GL_UNPACK_SWAP_BYTES, 0);
	glPixelStorei(GL_UNPACK_LSB_FIRST, 0);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	const void *imgdata = img.getPixels();
	if (_format == 5) {
		glTexSubImage2D(GL_TEXTURE_2D, 0, xoff, yoff, img.pitch, img.h, GL_RGB, GL_UNSIGNED_BYTE, imgdata);
	} else if (_format == 6) {
		glTexSubImage2D(GL_TEXTURE_2D, 0, xoff, yoff, img.pitch, img.h, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);
	} else {
		warning("Te3DTexture::update can't send image format %d to GL.", _format);
	}
	return;
}

// TODO: Add more functions here.

} // end namespace Syberia
