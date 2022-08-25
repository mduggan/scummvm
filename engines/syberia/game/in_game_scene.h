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

#ifndef SYBERIA_GAME_IN_GAME_SCENE_H
#define SYBERIA_GAME_IN_GAME_SCENE_H

#include "common/array.h"
#include "common/str.h"

#include "syberia/te/te_scene.h"
#include "syberia/te/te_lua_gui.h"

namespace Syberia {

class Character;
class TeLayout;

class InGameScene : TeScene {
public:
	InGameScene();
	
	class AnimObject {
		void onFinished();
	};
	
	class AnchorZone {
	};

	void activateAnchorZone(const Common::String &name, bool param_2);
	void addAnchorZone(const Common::String &param_1, const Common::String &param_2, float param_3);
	void addBlockingObject(const Common::String &obj) {
		_blockingObjects.push_back(obj);
	}
	void addCallbackAnimation2D(const Common::String &param_1, const Common::String &param_2, float param_3);
	void addMarker(const Common::String &name, const Common::String &param_2, float param_3, float param_4, const Common::String &param_5, const Common::String &param_6);
	static float angularDistance(float a1, float a2);
	bool aroundAnchorZone(const AnchorZone *zone);
	TeLayout *background();
	
	void draw();
	Character *character(const Common::String &name);
	void close();
	bool loadCharacter(const Common::String &name);
	bool loadPlayerCharacter(const Common::String &name);
	bool changeBackground(const Common::String &name);
	void unloadPlayerCharacter(const Common::String &character);
	// TODO add public members

private:
	Common::Array<Common::String> _blockingObjects;
	TeLuaGUI _gui;
	// TODO add private members

};

} // end namespace Syberia

#endif // SYBERIA_GAME_IN_GAME_SCENE_H
