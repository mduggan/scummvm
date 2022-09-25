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

#include "syberia/game/documents_browser.h"

namespace Syberia {

DocumentsBrowser::DocumentsBrowser() {
	_timer.alarmSignal().add(this, &DocumentsBrowser::onQuitDocumentDoubleClickTimer);
}

void DocumentsBrowser::enter() {
	error("TODO: Implement DocumentsBrowser::enter");
}

void DocumentsBrowser::leave() {
	error("TODO: Implement DocumentsBrowser::leave");
}

void DocumentsBrowser::load() {
	error("TODO: Implement DocumentsBrowser::load");
}

void DocumentsBrowser::loadZoomed() {
	error("TODO: Implement DocumentsBrowser::loadZoomed");
}

bool DocumentsBrowser::onQuitDocumentDoubleClickTimer() {
	error("TODO: Implement DocumentsBrowser::onQuitDocumentDoubleClickTimer");
}

// TODO: Add more functions here.

} // end namespace Syberia
