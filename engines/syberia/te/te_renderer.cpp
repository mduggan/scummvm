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
#include "common/debug.h"

#include "graphics/opengl/glad.h"

#include "syberia/te/te_renderer.h"
#include "syberia/te/te_light.h"

namespace Syberia {

TeRenderer::TeRenderer() : _textureEnabled(false), _shadowMode(ShadowMode0) {
}

void TeRenderer::addTransparentMesh(const TeMesh &mesh, unsigned long i1, unsigned long i2, unsigned long i3) {
	error("TODO: Implement me.");
}

void TeRenderer::clearBuffer(TeRenderer::Buffer buf) {
	glClear(buf);
}

void TeRenderer::create() {
	_textureEnabled = false;
	_currentCamera.reset();
	_shadowMode = ShadowMode0;
	_numTransparentMeshes = 0;
	error("TODO: Set another field 0 here.. 0x1f8?");
}

TeMatrix4x4 TeRenderer::currentMatrix() {
	return _matriciesStacks[_matrixMode].currentMatrix();
}

void TeRenderer::disableTexture() {
	glDisable(GL_TEXTURE_2D);
	_textureEnabled = false;
}

void TeRenderer::disableWireFrame() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void TeRenderer::disableZBuffer() {
	glDisable(GL_DEPTH_TEST);
	glDepthMask(false);
}

void TeRenderer::drawLine(const TeVector3f32 &from, const TeVector3f32 &to) {
	error("TODO: Implement me.");
}

void TeRenderer::enableTexture() {
	glEnable(GL_TEXTURE_2D);
	_textureEnabled = true;
}

void TeRenderer::enableWireFrame() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void TeRenderer::enableZBuffer() {
	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);
}

void TeRenderer::init() {
	glDisable(GL_CULL_FACE);
	TeLight::disableAll();
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_DONT_CARE);
	glClearDepth(1.0);
	glClearStencil(0);
	_clearColor = TeColor(0, 0, 0, 255);
	glClearColor(0, 0, 0, 1.0);
	debug("[TeRenderer::init] Vendor : %s\n", glGetString(GL_VENDOR));
	debug("[TeRenderer::init] Renderer : %s\n", glGetString(GL_RENDERER));
	debug("[TeRenderer::init] Version : %s\n", glGetString(GL_VERSION));
	int bits;
	glGetIntegerv(GL_STENCIL_BITS, &bits);
	debug("[TeRenderer::init] Sentil buffer bits : %d\n", bits);
	glGetIntegerv(GL_DEPTH_BITS, &bits);
	debug("[TeRenderer::init] Depth buffer bits : %d\n", bits);
	debug("[TeRenderer::init] Extensions : %s\n", glGetString(GL_EXTENSIONS));
	//TeOpenGLExtensions::loadExtensions(); // this does nothing in the game?
	_currentColor = TeColor(255, 255, 255, 255);
	_scissorEnabled = false;
	_scissorX = _scissorY = _scissorWidth = _scissorHeight = 0;
}


void TeRenderer::loadIdentityMatrix() {
	_matriciesStacks[_matrixMode].loadIdentity();
}

void TeRenderer::loadMatrix(const TeMatrix4x4 &matrix) {
	_matriciesStacks[_matrixMode].loadMatrix(matrix);
}

void TeRenderer::loadMatrixToGL(const TeMatrix4x4 &matrix) {
	glLoadMatrixf(matrix.getData());
}

void TeRenderer::loadProjectionMatrix(const TeMatrix4x4 &matrix) {
	glMatrixMode(GL_PROJECTION);
	_matrixMode = MM_GL_PROJECTION;
	_matriciesStacks[MM_GL_PROJECTION].loadIdentity();
	_matriciesStacks[_matrixMode].loadMatrix(matrix);
	glMatrixMode(GL_MODELVIEW);
	_matrixMode = MM_GL_MODELVIEW;
	_matriciesStacks[MM_GL_MODELVIEW].loadIdentity();
}

void TeRenderer::multiplyMatrix(const TeMatrix4x4 &matrix) {
	_matriciesStacks[_matrixMode].multiplyMatrix(matrix);
}

void TeRenderer::optimiseTransparentMeshProperties() {
	error("TODO: Implement me.");
}

void TeRenderer::popMatrix() {
	_matriciesStacks[_matrixMode].popMatrix();
}

void TeRenderer::pushMatrix() {
	_matriciesStacks[_matrixMode].pushMatrix();
}

Common::String TeRenderer::renderer() {
	return Common::String((const char *)glGetString(GL_RENDERER));
}

void TeRenderer::renderTransparentMeshes() {
	error("TODO: Implement me.");
}

void TeRenderer::reset() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	_matrixMode = MM_GL_PROJECTION;
	_matriciesStacks[MM_GL_PROJECTION].loadIdentity();
	glMatrixMode(GL_MODELVIEW);
	_matrixMode = MM_GL_MODELVIEW;
	_matriciesStacks[MM_GL_MODELVIEW].loadIdentity();
}

void TeRenderer::rotate(const TeQuaternion &rot) {
	error("TODO: Implement me.");
}

void TeRenderer::rotate(float f1, float f2, float f3, float f4) {
	error("TODO: Implement me.");
}

void TeRenderer::scale(float xs, float ys, float zs) {
	error("TODO: Implement me.");
}

void TeRenderer::setClearColor(const TeColor &col) {
	_clearColor = col;
	glClearColor(col.r() / 255.0, col.g() / 255.0, col.b() / 255.0, col.a() / 255.0);
}

void TeRenderer::setCurrentColor(const TeColor &col) {
	if (col == _currentColor)
		return;
		
	glColor4ub(col.r(), col.g(), col.b(), col.a());
	_currentColor = col;
}

void TeRenderer::setMatrixMode(enum MatrixMode mode) {
	GLenum glmode = 0;
	if (mode == MM_GL_TEXTURE)
		glmode = GL_TEXTURE;
	else if (mode == MM_GL_MODELVIEW)
		glmode = GL_MODELVIEW;
	else if (mode == MM_GL_PROJECTION)
		glmode = GL_PROJECTION;
	if (glmode)
		glMatrixMode(glmode);
	_matrixMode = mode;
}

void TeRenderer::setScissor(int x, int y, int w, int h) {
	_scissorX = x;
	_scissorY = y;
	_scissorWidth = w;
	_scissorHeight = h;
}

void TeRenderer::setViewport(int x, int y, int w, int h) {
	glViewport(x, y, w, h);
}

void TeRenderer::shadowMode(enum ShadowMode mode) {
	_shadowMode = mode;
	if (mode == ShadowMode0) {
		glDisable(GL_CULL_FACE);
		glShadeModel(GL_SMOOTH);
		return;
	}
	
	if (mode == ShadowMode1) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	} else { // ShadowMode2
		glDisable(GL_CULL_FACE);
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_FLAT);
	TeLight::disableAll();
}

void TeRenderer::translate(float x, float y, float z) {
	error("TODO: Implement me.");
}

Common::String TeRenderer::vendor() {
	return Common::String((const char *)glGetString(GL_VENDOR));
}

} // end namespace Syberia
