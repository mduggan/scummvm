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

#include "syberia/te/te_matrix4x4.h"

namespace Syberia {

TeMatrix4x4::TeMatrix4x4() {
}

TeMatrix4x4::TeMatrix4x4(const Math::Matrix<4, 4> &matrix) : Math::Matrix<4, 4>(matrix) {
}

void TeMatrix4x4::scale(const TeVector3f32 &vec) {
	TeMatrix4x4 scaleMatrix;
	scaleMatrix(0, 0) = vec.x();
	scaleMatrix(1, 1) = vec.y();
	scaleMatrix(2, 2) = vec.z();
	Math::Matrix<4,4> result = (*this * scaleMatrix);
	*this = result;
}

TeVector3f32 TeMatrix4x4::mult4x3(const TeVector3f32 &vec) const {
	const float f1 = vec.x();
	const float f2 = vec.y();
	const float f3 = vec.z();
	const float *data = getData();

	return TeVector3f32(data[0] * f1 + data[4] * f2 + data[8] * f3 + data[12],
						data[1] * f1 + data[5] * f2 + data[9] * f3 + data[13],
						data[2] * f1 + data[6] * f2 + data[10] * f3 + data[14]);

}

TeVector3f32 TeMatrix4x4::mult3x3(const TeVector3f32 &vec) const {
	const float f1 = vec.x();
	const float f2 = vec.y();
	const float f3 = vec.z();
	const float *data = getData();

	return TeVector3f32(data[0] * f1 + data[4] * f2 + data[8] * f3,
						data[1] * f1 + data[5] * f2 + data[9] * f3,
						data[2] * f1 + data[6] * f2 + data[10] * f3);

}

Common::String TeMatrix4x4::toString() const {
	const float *data = getData();
	return Common::String::format("[[%.03f %.03f %.03f %.03f]  [%.03f %.03f %.03f %.03f]  [%.03f %.03f %.03f %.03f]  [%.03f %.03f %.03f %.03f]]",
								  data[0], data[1], data[2], data[3],
								  data[4], data[5], data[6], data[7],
								  data[8], data[9], data[10], data[11],
								  data[12], data[13], data[14], data[15]);
}

} // end namespace Syberia
