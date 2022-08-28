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

#include "common/textconsole.h"

#include "syberia/syberia.h"
#include "syberia/game/application.h"
#include "syberia/game/cellphone.h"
#include "syberia/game/game.h"
#include "syberia/game/inventory.h"

namespace Syberia {

Inventory::Inventory() {
}

void Inventory::enter() {
	error("TODO: implement me.");
}

void Inventory::leave() {
	error("TODO: implement me.");
}

void Inventory::load() {
	error("TODO: implement me.");
}

void Inventory::unload() {
	error("TODO: implement me.");
}

void Inventory::loadCellphone() {
	_cellphone = new Cellphone();
	_cellphone->load();
}

//void loadFromBackup(TiXmlNode *node);
//void saveToBackup(TiXmlNode *node);

void Inventory::addObject(const Common::String &objname) {
	error("TODO: implement me.");
}

void Inventory::addObject(InventoryObject &obj) {
	error("TODO: implement me.");
}

bool Inventory::isDocument(const Common::String &objname) {
	error("TODO: implement me.");
}

int Inventory::objectCount(const Common::String &objname) {
	error("TODO: implement me.");
}

Common::String Inventory::objectDescription(const Common::String &objname) {
	error("TODO: implement me.");
}

Common::String Inventory::objectName(const Common::String &objname) {
	error("TODO: implement me.");
}

bool Inventory::onMainMenuButton() {
	leave();
	Game *game = g_engine->getGame();
	game->leave(false);
	Application *app = g_engine->getApplication();
	app->mainMenu().enter();
	return true;
}

bool Inventory::onObjectSelected(InventoryObject &obj) {
	error("TODO: implement me.");
}

bool Inventory::onQuitButton() {
	Game *game = g_engine->getGame();
	game->inventoryMenu().leave();
	return true;
}

bool Inventory::onTakeObjectSelected() {
	Game *game = g_engine->getGame();
	game->inventoryMenu().leave();
	return false;
}

bool Inventory::onVisibleCellphone() {
	error("TODO: implement me.");
}

bool Inventory::onZoomed() {
	error("TODO: implement me.");
}

void Inventory::pauseAnims() {
	error("TODO: implement me.");
}

void Inventory::unPauseAnims() {
	error("TODO: implement me.");
}

void Inventory::removeObject(const Common::String &objname) {
	error("TODO: implement me.");
}

void Inventory::removeSelectedObject() {
	error("TODO: implement me.");
}

InventoryObject *Inventory::selectedInventoryObject() {
	error("TODO: implement me.");
}

void Inventory::selectedObject(const Common::String &objname) {
	error("TODO: implement me.");
}

void Inventory::selectedObject(InventoryObject &obj) {
	error("TODO: implement me.");
}

const Common::String &Inventory::selectedObject() {
	error("TODO: implement me.");
}

bool Inventory::updateLayout() {
	error("TODO: implement me.");
}


} // end namespace Syberia
