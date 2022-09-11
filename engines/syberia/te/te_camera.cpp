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

#include "common/math.h"

#include "syberia/syberia.h"
#include "syberia/te/te_camera.h"
#include "syberia/te/te_matrix4x4.h"
#include "syberia/te/te_renderer.h"

namespace Syberia {

TeCamera::TeCamera() : _projectionMatrixType(0), _orthogonalParam1(1.0f),
	_orthogonalParam2(0.0f), _orthogonalParam3(1.0f), _orthogonalParam4(0.0f),
	_zsomething1(10.0), _zsomething2(4000.0), _transformA(0), _transformB(0)
{
}

void TeCamera::applyProjection() {
	TeRenderer *renderer = g_engine->getRenderer();
	renderer->setCurrentCamera(this);
	renderer->setViewport(_viewportX, _viewportY,
						  (uint)(_viewportW * _scale.x()), (uint)(_viewportH * _scale.y()));
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
	// TODO: Fix these variable names.
	float fVar5 = FLT_MAX;
	if ((_orthogonalParam2 - _orthogonalParam1) != 0.0) {
	  fVar5 = 1.0 / (_orthogonalParam2 - _orthogonalParam1);
	}
	float fVar4 = FLT_MAX;
	if ((_zsomething2 - _zsomething1) != 0.0) {
	  fVar4 = 1.0 / (_zsomething2 - _zsomething1);
	}
	float fVar6 = FLT_MAX;
	if (_orthogonalParam4 - _orthogonalParam3 != 0.0) {
	  fVar6 = 1.0 / (_orthogonalParam4 - _orthogonalParam3);
	}
	_projectionMatrix.setValue(0, 0, fVar5 + fVar5);
	_projectionMatrix.setValue(0, 1, 0.0);
	_projectionMatrix.setValue(0, 2, 0.0);
	_projectionMatrix.setValue(0, 3, 0.0);

	_projectionMatrix.setValue(1, 0, 0.0);
	_projectionMatrix.setValue(1, 1, fVar6 + fVar6);
	_projectionMatrix.setValue(1, 2, 0.0);
	_projectionMatrix.setValue(1, 3, 0.0);

	_projectionMatrix.setValue(2, 0, 0.0);
	_projectionMatrix.setValue(2, 1, 0.0);
	_projectionMatrix.setValue(2, 2, fVar4 * -2.0);
	_projectionMatrix.setValue(2, 3, 0.0);

	_projectionMatrix.setValue(3, 0, abs((_orthogonalParam2 + _orthogonalParam1) * fVar5));
	_projectionMatrix.setValue(3, 1, abs((_orthogonalParam4 + _orthogonalParam3) * fVar6));
	_projectionMatrix.setValue(3, 2, abs((_zsomething2 + _zsomething1) * fVar4));
	_projectionMatrix.setValue(3, 3, 1.0);

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
	renderer->setCurrentCamera(nullptr);
}

TeMatrix4x4 TeCamera::transformationMatrix() {
	if (!_transformA)
		return Te3DObject2::transformationMatrix();
	
	TeMatrix4x4 retval;
	warning("TODO: TeCamera::transformationMatrix Implement me.");

	retval.setToIdentity();
	return retval;
}

TeVector3f32 TeCamera::transformCoord(const TeVector3f32 &pt) {
	warning("TODO: TeCamera::transformCoord Implement me.");
	return pt;
}

TeVector3f32 TeCamera::transformPoint2Dto3D(const TeVector2f32 &pt) {
	warning("TODO: TeCamera::transformPoint2Dto3D Implement me.");
	return TeVector3f32(pt.getX(), pt.getY(), 0.0);
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
