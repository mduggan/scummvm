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

#include "syberia/game/application.h"
#include "syberia/game/question2.h"
#include "syberia/game/game.h"
#include "syberia/syberia.h"

namespace Syberia {

Question2::Question2() {
}

void Question2::enter() {
	error("TODO: implement me");
}

void Question2::leave() {
	error("TODO: implement me");
}

void Question2::load() {
	error("TODO: implement me");
}

bool Question2::onAnswerValidated(Answer &answer) {
	Game *game = g_engine->getGame();
	error("TODO: TeSignal1Param<TeString_const&>::call here");
	game->showMarkers(false);
	leave();
	return false;
}

void Question2::pushAnswer(const Common::String &name, const Common::String &unk, const Common::String &path) {
	error("TODO: implement me");
}

void Question2::unload() {
	leave();
	_gui.unload();
}


// TODO: Add more functions here.

} // end namespace Syberia
