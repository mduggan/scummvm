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

#ifndef SYBERIA_TE_TE_LAYOUT_H
#define SYBERIA_TE_TE_LAYOUT_H

#include "syberia/te/te_3d_object2.h"
#include "syberia/te/te_i_layout.h"
#include "syberia/te/te_i_3d_object2.h"
#include "syberia/te/te_matrix4x4.h"
#include "syberia/te/te_quaternion.h"
#include "syberia/te/te_vector2s32.h"
#include "syberia/te/te_vector3f32.h"

namespace Syberia {

class TeLayout : public TeILayout, public Te3DObject2 {
public:
	TeLayout();

	void addChild(TeI3DObject2 *child);
	void addChildBefore(TeI3DObject2 *newchild, const TeI3DObject2 *current);
	const TeVector3f32 &anchor();
	void disableAutoZ();
	void enableAutoZ();
	bool isAutoZEnabled();

	void draw();

	bool isMouseIn(const TeVector2s32 &mouseloc);

	DrawMode mode();

	bool onChildSizeChanged();
	bool onParentSizeChanged();
	bool onParentWorldTransformationMatrixChanged();

	TeVector3f32 position();
	CoordinatesType positionType() const;
	float ratio() const;
	RatioMode ratioMode() const;
	void removeChild(TeI3DObject2 *child);
	float safeAreaRatio() const;
	void setAnchor(const TeVector3f32 &anchor);
	//void setEditionColor(TeColor col); // does nothing?
	void setMode(DrawMode mode);
	void setParent(TeI3DObject2 *parent);
	void setPosition(const TeVector3f32 &pos);
	void setPositionType(CoordinatesType newtype);
	void setRatio(float val);
	void setRatioMode(RatioMode mode);
	void setRotation(TeQuaternion *rot);
	void setSafeAreaRatio(float ratio);
	void setScale(const TeVector3f32 &scale);
	void setSize(const TeVector3f32 &size);
	void setSizeType(CoordinatesType coordtype);
	void setZPosition(float zpos);

	TeVector3f32 size();
	CoordinatesType sizeType() const;
	TeVector3f32 transformMousePosition(const TeVector2s32 &mousepos);

	//void updateMesh(); // does nothing?
	void updatePosition();
	void updateSize();
	void updateWorldMatrix();
	void updateZ();
	void updateZSize();

	TeVector3f32 userPosition() const;
	TeVector3f32 userSize() const;
	TeVector3f32 worldPosition();
	TeMatrix4x4 worldTransformationMatrix();
	bool worldVisible();
	float xSize();
	float ySize();
	float zSize();

private:
	TeVector3f32 _anchor;
	CoordinatesType _positionType;
	TeVector3f32 _userPosition;
	CoordinatesType _sizeType;
	TeVector3f32 _userSize;
	
										  
	DrawMode _drawMode;
	bool _autoz;
	bool _sizeChanged;
	bool _positionChanged;
	bool _somethingChanged;
	bool _childChanged;
	bool _childOrParentChanged;
	float _ratio;
	RatioMode _ratioMode;
	float _safeAreaRatio;
	float _zPos;
	float _zSize;
};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_LAYOUT_H
