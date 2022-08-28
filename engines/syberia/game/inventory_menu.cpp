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
#include "syberia/game/inventory_menu.h"

namespace Syberia {

InventoryMenu::InventoryMenu() {
}

void InventoryMenu::enter() {
	error("TODO: implement me");
}

void InventoryMenu::leave() {
	error("TODO: implement me");
}

void InventoryMenu::load() {
	error("TODO: implement me");
}

void InventoryMenu::unload() {
	error("TODO: implement me");
}

bool InventoryMenu::isVisible() {
	static const Common::String layoutName("inventoryMenu");
	TeLayout *menuLayout = layout(layoutName);
	return menuLayout->visible();
}

bool InventoryMenu::onDocumentsButton() {
	error("TODO: implement me");
}

bool InventoryMenu::onInventoryButton() {
	error("TODO: implement me");
}

bool InventoryMenu::onMainMenuButton() {
	Application *app = g_engine->getApplication();
	app->captureFade();
	error("TODO: set some game flag here");
	//Game *game = g_engine->getGame();
	//app->fade();
	return false;
}

bool InventoryMenu::onQuitButton() {
	leave();
	return false;
}

bool InventoryMenu::onSaveButton(){
   return false;
}

} // end namespace Syberia
