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
#include "syberia/te/te_camera.h"
#include "syberia/te/te_matrix4x4.h"
#include "syberia/te/te_renderer.h"

namespace Syberia {

TeCamera::TeCamera() : _projectionMatrixType(0), _orthogonalParam1(1.0f),
	_orthogonalParam2(0.0f), _orthogonalParam3(1.0f), _orthogonalParam4(0.0f) {
}

void TeCamera::applyProjection() {
	TeRenderer *renderer = g_engine->getRenderer();
	renderer->setCurrentCamera(Common::SharedPtr<TeCamera>(this));
	renderer->setViewport(_viewportX, _viewportY,
						  (uint)(_viewportW * _widthScale), (uint)(_viewportH * _heightScale));
	renderer->setMatrixMode(TeRenderer::MatrixMode::MM_GL_PROJECTION);
	updateProjectionMatrix();
	renderer->setMatrixMode(TeRenderer::MatrixMode::MM_GL_PROJECTION);
	renderer->loadMatrixToGL(renderer->currentMatrix());
	renderer->setMatrixMode(TeRenderer::MatrixMode::MM_GL_MODELVIEW);
}

void TeCamera::applyTransformations() {
	TeRenderer *renderer = g_engine->getRenderer();
	renderer->setMatrixMode(TeRenderer::MatrixMode::MM_GL_MODELVIEW);
	TeMatrix4x4 matrix = transformationMatrix();
	matrix.inverse();
	renderer->loadMatrix(matrix);
	renderer->loadMatrixToGL(renderer->currentMatrix());
}

void TeCamera::buildOrthoMatrix() {
	error("TODO: Implement me.");
}

void TeCamera::buildPerspectiveMatrix() {
	error("TODO: Implement me.");
}

void TeCamera::buildPerspectiveMatrix2() {
	error("TODO: Implement me.");
}

void TeCamera::buildPerspectiveMatrix3() {
	error("TODO: Implement me.");
}

void TeCamera::draw() {
	error("TODO: Implement me.");
}

void TeCamera::getRay(const TeVector2s32 &param_1, TeVector3f32 &out1, TeVector3f32 &out2) {
	error("TODO: Implement me.");
}

void TeCamera::loadBin(const Common::String &path) {
	error("TODO: Implement me.");
}

void TeCamera::loadBin(const Common::ReadStream &stream) {
	error("TODO: Implement me.");
}

void TeCamera::orthogonalParams(float f1, float f2, float f3, float f4) {
	_orthogonalParam1 = f1;
	_orthogonalParam2 = f2;
	_orthogonalParam3 = f3;
	_orthogonalParam4 = f4;
}

TeMatrix4x4 TeCamera::projectionMatrix() {
	switch(_projectionMatrixType) {
	case 1:
	  buildPerspectiveMatrix();
	  break;
	case 2:
	  buildPerspectiveMatrix2();
	  break;
	case 3:
	  buildPerspectiveMatrix3();
	  break;
	case 4:
	  buildOrthoMatrix();
	}
	return _projectionMatrix;
}

TeVector3f32 TeCamera::projectPoint(const TeVector3f32 &pt) {
	error("TODO: Implement me.");
}

TeVector3f32 TeCamera::projectPoint3f32(const TeVector3f32 &pt) {
	error("TODO: Implement me.");
}

void TeCamera::restore() {
	TeRenderer *renderer = g_engine->getRenderer();
	renderer->setCurrentColor(TeColor(255, 255, 255, 255));
	renderer->setCurrentCamera(Common::SharedPtr<TeCamera>());
}

TeMatrix4x4 TeCamera::transformationMatrix() {
	error("TODO: Implement me.");
}

TeVector3f32 TeCamera::transformCoord(const TeVector3f32 &pt) {
	error("TODO: Implement me.");
}

TeVector3f32 TeCamera::transformPoint2Dto3D(const TeVector2f32 &pt) {
	error("TODO: Implement me.");
}

void TeCamera::updateProjectionMatrix() {
	TeRenderer *renderer = g_engine->getRenderer();
	renderer->setMatrixMode(TeRenderer::MatrixMode::MM_GL_PROJECTION);
	renderer->loadProjectionMatrix(projectionMatrix());
}

void TeCamera::viewport(int x, int y, uint w, uint h) {
	_viewportX = x;
	_viewportY = y;
	_viewportW = w;
	_viewportH = h;
	_onViewportChangedSignal.call();
}


} // end namespace Syberia
