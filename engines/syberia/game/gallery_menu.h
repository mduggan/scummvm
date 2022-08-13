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

#ifndef SYBERIA_GAME_GALLERY_MENU_H
#define SYBERIA_GAME_GALLERY_MENU_H

#include "common/array.h"
#include "syberia/te/te_music.h"
#include "syberia/te/te_lua_gui.h"

namespace Syberia {

class GalleryMenu : public TeLuaGUI {
public:
	GalleryMenu();

	class GalleryBtnObject {
		bool OnValidated();
	};
	
	void enter();
	void leave();

	bool onQuitButton();
	bool onLockVideoButtonValidated();
	bool onSkipVideoButtonValidated();
	bool onVideoFinished();

private:
	TeMusic _music;
	Common::Array<GalleryBtnObject *> _btnObjects;
};

} // end namespace Syberia

#endif // SYBERIA_GAME_GALLERY_MENU_H
