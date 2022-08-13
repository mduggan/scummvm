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
#include "common/str-array.h"
#include "common/system.h"
#include "syberia/game/game.h"

namespace Syberia {

Game::Game() {
}

static Common::StringArray split(const Common::String &text, char c) {
	Common::StringArray values;

	Common::String str = text;
	size_t pos;
	while ((pos = str.findFirstOf(c)) != Common::String::npos) {
		values.push_back(Common::String(str.c_str(), pos));
		str = Common::String(str.c_str() + pos + 1);
	}

	values.push_back(str);
	return values;
}

bool Game::addAnimToSet(const Common::String &anim) {
	// Get path to lua script, eg scenes/ValVoralberg/14040/Set14040.lua
	const Common::Path animPath(Common::String("scenes/") + anim + "/");
	
	bool retval = false;
	if (Common::File::exists(animPath)) {
		Common::StringArray parts = split(anim, '/');
		assert(parts.size() >= 2);

		Common::String layoutName = parts[1];
		Common::String path = Common::String("scenes/") + parts[0] + "/" + parts[1] + "/Set" + parts[1];

		_gui2.load(path + ".lua");
		/*
		TeILayout *layout = _gui2.layout("root");
		TeSpriteLayout *spriteLayout2 = findSpriteLayoutByName(layout, layoutName);

		TeLayout *layout2 = TeLuaGUI::layout(&(this->scene).field_0x170,"root");
		long lVar5 = 0;
		if (spriteLayout2) {
			lVar5 = (long)plVar3 + *(long *)(*plVar3 + -0x198);
		}
		(**(code **)(*(long *)((long)&pTVar2->vptr + (long)pTVar2->vptr[-0x33]) + 0x30))
					((long)&pTVar2->vptr + (long)pTVar2->vptr[-0x33],lVar5);
		 */
	  retval = true;
	}

	return retval;
}

void Game::addToHand(const Common::String &objname) {
	_inventory.addObject(objname);
	_inventory.selectedObject(objname);
}

void Game::addToScore(enum EGameScoreID score) {
	_score = static_cast<enum EGameScoreID>(_score + score);
}

void Game::draw() {
	if (_running) {
	  _frameCounter++;
	  _scene.draw();
	}
}

void Game::resetPreviousMousePos() {
	_previousMousePos = TeVector2s32(-1, -1);
}

void Game::setBackground(const Common::String &name) {
	_scene.changeBackground(name);
}

bool Game::unloadPlayerCharacter(const Common::String &character) {
	_scene.unloadPlayerCharacter(character);
	return true;
}

// TODO: Add more functions here.

} // end namespace Syberia
