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
	TeCurveAnim2<TeI3DObject2, TeColor> *fadeInAnim = _gui.colorLinearAnimation("fadeIn");
	if (fadeInAnim->_runTimer._stopped) {
		error("TODO: Implement me.");
	}
}

void Notifier::load() {
	_gui.load("menus/Notifier.lua");
	TeLayout *notifierLayout = _gui.layout("notifier");
	Game *game = g_engine->getGame();
	game->addNoScale2Child(notifierLayout);
	notifierLayout->setVisible(false);
	//TeCurveAnim2<TeI3DObject2,TeColor> *fadeIn = _gui.colorLinearAnimation("fadeIn");
	//TeCurveAnim2<TeI3DObject2,TeColor> *visible = _gui.colorLinearAnimation("visible");
	//TeCurveAnim2<TeI3DObject2,TeColor> *fadeOut = _gui.colorLinearAnimation("fadeOut");

	error("TODO: Finish implementing Notifier::load.");
}

bool Notifier::onFadeInFinished() {
	//TeCurveAnim2<TeI3DObject2,TeColor> *visible = _gui.colorLinearAnimation("visible");
	error("TODO: Implement me.");
}

bool Notifier::onFadeOutFinished() {
	TeLayout *notifierLayout = _gui.layout("notifier");
	notifierLayout->setVisible(false);
	launchNextnotifier();
	return false;
}

bool Notifier::onVisibleFinished() {
	error("TODO: Implement me.");
}

void Notifier::push(const Common::String &name, const Common::String &imgpath) {
	notifierData n = {name, imgpath};
	_notifierDataArray.push_back(n);
	launchNextnotifier();
}

void Notifier::unload() {
	TeLayout *layout = _gui.layout("notifier");
	g_engine->getGame()->removeNoScale2Child(layout);
	_gui.unload();
}



} // end namespace Syberia
