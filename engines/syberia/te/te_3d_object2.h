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

#ifndef SYBERIA_TE_TE_3D_OBJECT2_H
#define SYBERIA_TE_TE_3D_OBJECT2_H

#include "common/array.h"
#include "syberia/te/te_color.h"
#include "syberia/te/te_i_3d_object2.h"
#include "syberia/te/te_matrix4x4.h"
#include "syberia/te/te_quaternion.h"
#include "syberia/te/te_vector3f32.h"

namespace Syberia {

class Te3DObject2 : public TeI3DObject2 {
public:
	Te3DObject2();
	virtual ~Te3DObject2() {};

	void addChild(Te3DObject2 *child);
	void addChildBefore(Te3DObject2 *newChild, Te3DObject2 *ref);
	Te3DObject2 *child(long offset);
	long childCount() {
		return _children.size();
	}

	long childIndex(Te3DObject2 *child);

	Common::Array<Te3DObject2 *> &childList() {
		return _children;
	}

	bool childListChanged() const {
		return _childListChanged;
	}

	const TeColor &color() const {
		return _color;
	}

	bool colorInheritance() const {
		return _colorInheritance;
	}
	
	static void deserialize(Common::ReadStream *stream, Te3DObject2 *dest);
	static void serialize(Common::ReadStream *stream, Te3DObject2 *src);

	virtual void draw() {};
	const Common::String &name() const {
		return _name;
	}
	Te3DObject2 *parent() {
		return _parent;
	}
	const TeVector3f32 &position() const {
		return _position;
	}
	void removeChild(Te3DObject2 *dest);
	void removeChildren();
	void rotate(TeQuaternion *rot);
	const TeQuaternion &rotation() const {
		return _rotation;
	}
	const TeVector3f32 &scale() const {
		return _scale;
	}
	void setColor(const TeColor &col);
	void setColorInheritance(bool val) {
		_colorInheritance = val;
	}
	bool setName(const Common::String &name) {
		_name = name;
		return true;
	}
	void setParent(TeI3DObject2 *newparent);
	void setPosition(const TeVector3f32 &pos);
	void setRotation(const TeQuaternion &rot);
	void setScale(const TeVector3f32 &scale);
	void setSize(const TeVector3f32 &size);
	void setVisible(bool visible);
	void setZPosition(float zpos);
	const TeVector3f32 &size() const {
		return _size;
	}
	TeMatrix4x4 transformationMatrix() const;
	void translate(TeVector3f32 *vec);
	void updateZ() {};
	virtual bool visible() const {
		return _visible;
	}
	
	TeColor worldColor();
	TeVector3f32 worldPosition();
	TeQuaternion worldRotation();
	TeVector3f32 worldScale();
	TeMatrix4x4 worldTransformationMatrix();
	bool worldVisible();
	float xSize() const { return _size.x(); };
	float ySize() const { return _size.y(); };
	float zSize() const { return _size.z(); };

private:
	Common::Array<Te3DObject2 *> _children;
	bool _childListChanged;
	TeColor _color;
	bool _colorInheritance;
	Common::String _name;
	Te3DObject2 *_parent;
	TeVector3f32 _position;
	TeQuaternion _rotation;
	TeVector3f32 _scale;
	TeVector3f32 _size;
	bool _visible;
	// TODO add private members

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_3D_OBJECT2_H
