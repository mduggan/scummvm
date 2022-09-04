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

#include "common/file.h"
#include "common/path.h"
#include "common/textconsole.h"
#include "syberia/game/in_game_scene.h"
#include "syberia/game/character.h"

namespace Syberia {

InGameScene::InGameScene() {
}

void InGameScene::draw() {
	error("TODO: implement me");
}

bool InGameScene::changeBackground(const Common::String &name) {
	if (Common::File::exists(name)) {
		TeSpriteLayout *spriteLayout = _bgGui.spriteLayout("root");
		assert(spriteLayout);
		spriteLayout->load(name);
		return true;
	}
	return false;
}


/*static*/ float InGameScene::angularDistance(float a1, float a2) {
	float result;

	result = a2 - a1;
	if (result >= -3.141593 && result > 3.141593) {
		result = result + -6.283185;
	} else {
		result = result + 6.283185;
	}
	return result;
}

TeLayout *InGameScene::background() {
	return _bgGui.layout("background");
}

void InGameScene::close() {
	reset();
	error("TODO: Implement me");
}

void InGameScene::reset() {
	if (_character)
		_character->setFreeMoveZone(Common::SharedPtr<TeFreeMoveZone>());
	freeSceneObjects();
	_bgGui.unload();
	unloadSpriteLayouts();
	_gui2.unload();
	_gui3.unload();
}

void InGameScene::freeSceneObjects() {
	error("TODO: Implement me");
}

void InGameScene::unloadSpriteLayouts() {
	error("TODO: Implement me");
}

Character *InGameScene::character(const Common::String &name) {
	error("TODO: Implement me");
}

bool InGameScene::loadCharacter(const Common::String &name) {
	error("TODO: Implement me");
}

bool InGameScene::loadPlayerCharacter(const Common::String &name) {
	error("TODO: Implement me");
}

void InGameScene::unloadPlayerCharacter(const Common::String &name) {
	error("TODO: Implement me");
}

void InGameScene::unloadCharacter(const Common::String &name) {
	error("TODO: Implement me");
}

bool InGameScene::findKate() {
	for (auto &m : models()) {
		if (m->name() == "Kate")
			return true;
	}
	return false;
}

} // end namespace Syberia
