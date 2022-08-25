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
#include "syberia/game/game.h"
#include "syberia/game/notifier.h"
#include "syberia/te/te_layout.h"

namespace Syberia {

Notifier::Notifier() {
}

void Notifier::launchNextnotifier() {
	error("TODO: Implement me.");
}

void Notifier::load() {
	error("TODO: Implement me.");
}

bool Notifier::onFadeInFinished() {
	error("TODO: Implement me.");
}

bool Notifier::onFadeOutFinished() {
	error("TODO: Implement me.");
}

bool Notifier::onVisibleFinished() {
	error("TODO: Implement me.");
}

void Notifier::push(const Common::String &s1, const Common::String &s2) {
	notifierData n = {s1, s2};
	_notifierDataArray.push_back(n);
	launchNextnotifier();
}

void Notifier::unload() {
	static const Common::String layoutName("notifier");
	TeLayout *layout = _gui.layout(layoutName);
	g_engine->getGame()->removeNoScale2Child(layout);
	_gui.unload();
}



} // end namespace Syberia
