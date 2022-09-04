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

#include "syberia/game/confirm.h"
#include "syberia/game/application.h"
#include "syberia/syberia.h"

#include "syberia/te/te_text_layout.h"

namespace Syberia {

Confirm::Confirm() {
}

void Confirm::enter(const Common::String &guiPath, const Common::String &y) {
	_gui.load(guiPath);
	TeLayout *backgroundLayout = _gui.layout("background");
	backgroundLayout->setRatioMode(TeILayout::RATIO_MODE_NONE);

	Application *app = g_engine->getApplication();
	TeButtonLayout *confirmButtonLayout = _gui.buttonLayout("confirm");
	app->_frontLayout.addChild(confirmButtonLayout);

	TeButtonLayout *yesButtonLayout = _gui.buttonLayout("yes");
	if (yesButtonLayout)
		yesButtonLayout->onMouseClickValidated().add<Confirm>(this, &Confirm::onButtonYes);

	TeButtonLayout *noButtonLayout = _gui.buttonLayout("no");
	if (noButtonLayout)
		noButtonLayout->onMouseClickValidated().add<Confirm>(this, &Confirm::onButtonNo);
	
	TeLayout *textLayout = _gui.layout("text");
	if (textLayout) {
		const Common::String textAttributs = _gui.value("textAttributs").toString();
		const Common::String textAttributsDown = _gui.value("textAttributsDown").toString();
		const Common::String *okButtonLoc = app->_loc.value("okButton");
		const Common::String *cancelButtonLoc = app->_loc.value("cancelButton");

		TeTextLayout *textTextLayout = dynamic_cast<TeTextLayout *>(textLayout->child(0));
		textTextLayout->setText(textAttributs + *app->_loc.value(textTextLayout->name()));
		
		if (!okButtonLoc || !cancelButtonLoc) {
			error("Missing translations for ok and cancel");
		}
				
		TeTextLayout *yesUpLayout = _gui.textLayout("yesUpLayout");
		yesUpLayout->setText(textAttributs + *okButtonLoc);

		TeTextLayout *yesDownLayout = _gui.textLayout("yesDownLayout");
		yesDownLayout->setText(textAttributsDown + *okButtonLoc);
		
		TeTextLayout *yesRollOverLayout = _gui.textLayout("yesRollOverLayout");
		yesRollOverLayout->setText(textAttributs + *okButtonLoc);
		
		TeTextLayout *noUpLayout = _gui.textLayout("noUpLayout");
		noUpLayout->setText(textAttributs + *cancelButtonLoc);

		TeTextLayout *noDownLayout = _gui.textLayout("noDownLayout");
		noDownLayout->setText(textAttributsDown + *cancelButtonLoc);
	
		TeTextLayout *noRollOverLayout = _gui.textLayout("noRollOverLayout");
		noRollOverLayout->setText(textAttributs + *cancelButtonLoc);
	}

	warning("TODO: Remove mouse cursor here.");
}

void Confirm::leave() {
	Application *app = g_engine->getApplication();
	TeButtonLayout *confirmButtonLayout = _gui.buttonLayout("confirm");
	if (confirmButtonLayout) {
		app->_frontLayout.removeChild(confirmButtonLayout);
	}
	_gui.unload();
}

bool Confirm::onButtonNo() {
	Application *app = g_engine->getApplication();
	app->captureFade();
	leave();
	_onButtonNoSignal.call();
	app->fade();
	return true;
}

bool Confirm::onButtonYes() {
	Application *app = g_engine->getApplication();
	app->captureFade();
	leave();
	_onButtonYesSignal.call();
	app->fade();
	return true;
}

} // end namespace Syberia
