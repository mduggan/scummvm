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

#ifndef SYBERIA_GAME_INVENTORY_H
#define SYBERIA_GAME_INVENTORY_H

#include "common/str.h"
#include "syberia/game/inventory_object.h"
#include "syberia/te/te_layout.h"

namespace Syberia {

class Cellphone;

class Inventory : public TeLayout {
public:
	Inventory();

	void enter();
	void leave();
	void load();
	void unload();
	void loadCellphone();
	
	//void loadFromBackup(TiXmlNode *node);
	//void saveToBackup(TiXmlNode *node);

	void addObject(const Common::String &objname);
	void addObject(InventoryObject &obj);
	bool isDocument(const Common::String &objname);

	int objectCount(const Common::String &objname);
	Common::String objectDescription(const Common::String &objname);
	Common::String objectName(const Common::String &objname);
	
	bool onMainMenuButton();
	bool onObjectSelected(InventoryObject &obj);
	bool onQuitButton();
	bool onTakeObjectSelected();
	bool onVisibleCellphone();
	bool onZoomed();
	
	void pauseAnims();
	void unPauseAnims();
	
	void removeObject(const Common::String &objname);
	void removeSelectedObject();

	InventoryObject *selectedInventoryObject();
	void selectedObject(const Common::String &objname);
	void selectedObject(InventoryObject &obj);
	const Common::String &selectedObject();

	bool updateLayout();

private:
	
	Common::Array<InventoryObject *> _invObjects;
	Cellphone *_cellphone;
};

} // end namespace Syberia

#endif // SYBERIA_GAME_INVENTORY_H
