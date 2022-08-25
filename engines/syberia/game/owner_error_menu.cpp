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

#include "common/path.h"
#include "syberia/syberia.h"
#include "syberia/game/application.h"
#include "syberia/game/owner_error_menu.h"

namespace Syberia {

OwnerErrorMenu::OwnerErrorMenu() : _entered(false) {
}

void OwnerErrorMenu::enter() {
	_entered = true;
	static const Common::Path scriptPath("menus/ownerError/ownerError.lua");
	load(scriptPath.toString());
	error("TODO: Finish implementation of OwnerErrorMenu::enter");
	/*
	Application *app = g_engine->getApplication();
	static const Common::String layoutName("menu");
	TeLayout *menuLayout = TeLuaGUI::layout(layoutName);
	 ...
	 */
}

void OwnerErrorMenu::leave() {
	Application *app = g_engine->getApplication();
	app->captureFade();
	TeLuaGUI::unload();
	_entered = false;
	app->mainMenu().enter();
	app->fade();
}

} // end namespace Syberia
