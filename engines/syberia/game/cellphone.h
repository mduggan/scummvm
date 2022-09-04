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

#ifndef SYBERIA_GAME_CELLPHONE_H
#define SYBERIA_GAME_CELLPHONE_H

#include "common/array.h"
#include "common/callback.h"
#include "common/str.h"
#include "common/xmlparser.h"

#include "syberia/te/te_layout.h"
#include "syberia/te/te_text_layout.h"
#include "syberia/te/te_lua_gui.h"

namespace Syberia {

class Cellphone : public TeLayout {
public:
	Cellphone();

	bool addNumber(const Common::String &num);
	void currentPage(int offset);

	void enter();
	void leave();

	void load();
	void loadFromBackup(const Common::XMLParser::ParserNode *node);
	
	bool onCallNumberValidated();
	bool onCloseButtonValidated();
	bool onNextNumber();
	bool onPreviousNumber();
	
	void saveToBackup(Common::XMLParser::ParserNode *xmlnode);
	void setVisible(bool visible);
	
	TeSignal1Param<Common::String> &onCallNumber() {
		return _onCallNumberSignal;
	}
	
	void unload();

private:

	int _nextNumber;
	Common::Array<TeTextLayout*> _textLayoutArray;
	Common::Array<Common::String> _strArray;
	
	TeSignal1Param<Common::String> _onCallNumberSignal;
	
	TeLuaGUI _gui;
};

} // end namespace Syberia

#endif // SYBERIA_GAME_CELLPHONE_H
