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

#include "syberia/te/te_camera.h"

namespace Syberia {

TeCamera::TeCamera() : _projectionMatrixType(0), _orthogonalParam1(1.0f),
	_orthogonalParam2(0.0f), _orthogonalParam3(1.0f), _orthogonalParam4(0.0f) {
}

void TeCamera::applyProjection() {
	error("TODO: Implement me.");
}

void TeCamera::applyTransformations() {
	error("TODO: Implement me.");
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
	error("TODO: Implement me.");
}

TeMatrix4x4 TeCamera::projectionMatrix() {
	error("TODO: Implement me.");
}

TeVector3f32 TeCamera::projectPoint(const TeVector3f32 &pt) {
	error("TODO: Implement me.");
}

TeVector3f32 TeCamera::projectPoint3f32(const TeVector3f32 &pt) {
	error("TODO: Implement me.");
}

void TeCamera::restore() {
	error("TODO: Implement me.");
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
	error("TODO: Implement me.");
}

void TeCamera::viewport(int x1, int y1, uint width, uint height) {
	error("TODO: Implement me.");
}


} // end namespace Syberia
