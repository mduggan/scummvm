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

#ifndef SYBERIA_TE_TE_SCENE_H
#define SYBERIA_TE_TE_SCENE_H

#include "common/array.h"
#include "common/ptr.h"
#include "common/str.h"
#include "common/path.h"

#include "syberia/te/te_camera.h"
#include "syberia/te/te_model.h"

namespace Syberia {

class TeScene {
public:
	TeScene();

	void close();

	Common::SharedPtr<TeCamera> camera(const Common::String &name);
	Common::SharedPtr<TeModel> model(const Common::String &name);

	Common::SharedPtr<TeCamera> currentCamera();
	int currentCameraIndex() const { return _currentCameraIndex; }
	Common::String currentCameraName() const;

	void draw();
	virtual void load(Common::Path &path) {};

	void removeModel(const Common::String &name);
	void setCurrentCamera(const Common::String &name);
	void setCurrentCameraIndex(uint index) {
		_currentCameraIndex = index;
	}

	void update();

	Common::Array<Common::SharedPtr<TeCamera>> &cameras() { return _cameras; }
	Common::Array<Common::SharedPtr<TeModel>> &models() { return _models; }

private:
	uint _currentCameraIndex;
	Common::Array<Common::SharedPtr<TeCamera>> _cameras;
	Common::Array<Common::SharedPtr<TeModel>> _models;

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_SCENE_H
