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

#ifndef SYBERIA_TE_TE_MESH_H
#define SYBERIA_TE_TE_MESH_H

#include "common/array.h"
#include "syberia/te/te_3d_object2.h"
#include "syberia/te/te_color.h"
#include "syberia/te/te_vector2f32.h"
#include "syberia/te/te_vector3f32.h"
#include "syberia/te/te_matrix4x4.h"
#include "syberia/te/te_material.h"

namespace Syberia {


class TeMesh : public Te3DObject2 {
public:
	TeMesh();

private:
	Common::Array<unsigned char> _materialIndexes;
	Common::Array<TeVector3f32> _verticies;
	Common::Array<TeVector3f32> _normals;
	Common::Array<TeVector3f32> _updatedVerticies;
	Common::Array<TeVector3f32> _updatedNormals;
	Common::Array<TeVector2f32> _uvs;
	Common::Array<unsigned short> _indexes;
	Common::Array<unsigned short> _faceCounts;
	Common::Array<TeColor> _colors;
	
	bool _matrixForced;
	TeMatrix4x4 _forceMatrix;

	// TODO add private members

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_MESH_H
