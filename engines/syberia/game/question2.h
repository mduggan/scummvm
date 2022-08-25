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

#ifndef SYBERIA_GAME_QUESTION2_H
#define SYBERIA_GAME_QUESTION2_H

#include "engines/syberia/te/te_lua_gui.h"
#include "engines/syberia/te/te_layout.h"


namespace Syberia {

class Question2 : TeLayout {
public:
	Question2();

	class Answer : public TeLuaGUI {
		TeLayout *layout();
		void load(const Common::String &name, const Common::String &unk, const Common::String &path);
		void unload();
		bool onButtonValidated();
	};

	void enter();
	void leave();
	void load();
	bool onAnswerValidated(Answer &answer);
	void pushAnswer(const Common::String &name, const Common::String &unk, const Common::String &path);
	void unload();

private:
	TeLuaGUI _gui;

};

} // end namespace Syberia

#endif // SYBERIA_GAME_QUESTION2_H
