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

#include "syberia/game/cellphone.h"

namespace Syberia {

Cellphone::Cellphone() : _nextNumber(0) {
}

bool Cellphone::addNumber(const Common::String &num) {
	error("TODO: implement me");
}

void Cellphone::currentPage(int offset) {
	error("TODO: implement me");
}

void Cellphone::enter() {
	error("TODO: implement me");
}

void Cellphone::leave() {
	error("TODO: implement me");
}

void Cellphone::load() {
	_nextNumber = 0;
	error("TODO: implement me");
	/*
	TeButtonLayout *btnlayout;
	_gui->load("menus/cellphone.lua");
	btnlayout = _gui->buttonLayout("haut");
	btnlayout->addCallback<CellPhone>(this);
	btnlayout = _gui->buttonLayout("bas");
	btnlayout->addCallback<CellPhone>(this);
	btnlayout = _gui->buttonLayout("appeler");
	btnlayout->addCallback<CellPhone>(this);
	btnlayout = _gui->buttonLayout("fermer");
	btnlayout->addCallback<CellPhone>(this);
	btnlayout = _gui->buttonLayout("background");
	// then do something else..
	*/
}

void Cellphone::loadFromBackup(const Common::XMLParser::ParserNode *node) {
	/*
	 basic algorithm:
	child = node->lastChild;
	while (child != nullptr) {
		if (child->type == ELEMENT) {
			if (if child->userData == "Number") {
				addNumber(this, child->getAttribute("num"));
			}
		}
	child = child->prev;
	}*/
}

// TODO add callbacks Common::Array<Common::Callback1Param<Common::String>> &onCallNumber();
bool Cellphone::onCallNumberValidated() {
	error("TODO: implement me");
}

bool Cellphone::onCloseButtonValidated() {
	error("TODO: implement me");
}

bool Cellphone::onNextNumber() {
	error("TODO: work out how the max num works here.");
	/*
	int numoffset = _nextNumber + 1;
	if (numoffset < _maxnum) {
		currentPage(numoffset);
	}*/
	return false;
}

bool Cellphone::onPreviousNumber() {
	int numoffset = _nextNumber - 1;
	if (numoffset >= 0) {
	  currentPage(numoffset);
	}
	return false;
}

void Cellphone::saveToBackup(Common::XMLParser::ParserNode *xmlnode) {
	error("TODO: implement me");
}

void Cellphone::setVisible(bool visible) {
	error("TODO: implement me");
}

void Cellphone::unload() {
	leave();
	_gui.unload();
}
} // end namespace Syberia
