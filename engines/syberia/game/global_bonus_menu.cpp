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

#include "syberia/syberia.h"
#include "syberia/game/application.h"
#include "syberia/game/global_bonus_menu.h"

namespace Syberia {

GlobalBonusMenu::GlobalBonusMenu() : _entered(false) {
}

void GlobalBonusMenu::enter() {
	Application *app = g_engine->getApplication();
	error("TODO: Finish implementing GlobalBonusMenu::enter");
	app->captureFade();
	_entered = true;
	load("menus/bonusmenu/GlobalBonusMenu.lua");
	TeLayout *menu = layout("menu");
	
	// todo: finish this.
}

void GlobalBonusMenu::leave() {
	if (_entered != 0) {
		Application *app = g_engine->getApplication();
		app->captureFade();
		TeLuaGUI::unload();
		app->fade();
		_entered = false;
	}
}

bool GlobalBonusMenu::onSomeButtonValidated(const Common::String &script) {
	Application *app = g_engine->getApplication();
	app->captureFade();
	leave();
	app->bonusMenu().enter(script);
	app->fade();
	return false;
}

bool GlobalBonusMenu::onAraButtonValidated() {
	static const Common::String scriptName("menus/bonusmenu/Ara.lua");
	return onSomeButtonValidated(scriptName);
}

bool GlobalBonusMenu::onBarButtonValidated() {
	static const Common::String scriptName("menus/bonusmenu/Bar.lua");
	return onSomeButtonValidated(scriptName);
}

bool GlobalBonusMenu::onCitButtonValidated() {
	static const Common::String scriptName("menus/bonusmenu/Cit.lua");
	return onSomeButtonValidated(scriptName);
}

bool GlobalBonusMenu::onSyb2ButtonValidated() {
	static const Common::String scriptName("menus/bonusmenu/Syb2.lua");
	return onSomeButtonValidated(scriptName);
}

bool GlobalBonusMenu::onSyb3ButtonValidated() {
	static const Common::String scriptName("menus/bonusmenu/Syb3.lua");
	return onSomeButtonValidated(scriptName);
}

bool GlobalBonusMenu::onValButtonValidated() {
	static const Common::String scriptName("menus/bonusmenu/Val.lua");
	return onSomeButtonValidated(scriptName);
}

bool GlobalBonusMenu::onQuitButton() {
	Application *app = g_engine->getApplication();
	app->captureFade();
	leave();
	app->mainMenu().enter();
	app->fade();
	return true;
}

} // end namespace Syberia
