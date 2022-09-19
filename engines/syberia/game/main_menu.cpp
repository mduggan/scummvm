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

#include "common/system.h"
#include "common/events.h"

#include "syberia/syberia.h"
#include "syberia/game/confirm.h"
#include "syberia/game/main_menu.h"
#include "syberia/game/application.h"

#include "syberia/te/te_button_layout.h"
#include "syberia/te/te_sprite_layout.h"
#include "syberia/te/te_text_layout.h"
#include "syberia/te/te_music.h"


namespace Syberia {

MainMenu::MainMenu() : _entered(false) {
	onNewGameConfirmedSignal.add(this, &MainMenu::onNewGameConfirmed);
	onActivatedTutoSignal.add(this, &MainMenu::onActivedTuto);
	onDisabledTutoSignal.add(this, &MainMenu::onDisabledTuto);
	onQuitSignal.add(this, &MainMenu::onQuit);
	onFacebookLoggedSignal.add(this, &MainMenu::onFacebookLogged);
}

void MainMenu::enter() {
	Application *app = g_engine->getApplication();
	TeSpriteLayout &appSpriteLayout = app->appSpriteLayout();
	appSpriteLayout.setVisible(true);
	if (appSpriteLayout._tiledSurfacePtr->_frameAnim._runTimer._stopped) {
		appSpriteLayout.load("menus/menu.ogv");
		appSpriteLayout._tiledSurfacePtr->_frameAnim._loopCount = -1;
		appSpriteLayout._tiledSurfacePtr->play();
	}
	app->captureFade();
	
	_entered = true;
	load("menus/mainMenu/mainMenu.lua");
	
	TeLayout *menuLayout = layout("menu");
	appSpriteLayout.addChild(menuLayout);
	
	app->mouseCursorLayout().setVisible(true);
	app->mouseCursorLayout().load("pictures/cursor.png");
	
	TeMusic &music = app->music();
	if (music.isPlaying()) {
		// TODO: something here??
	}
	music.load(value("musicPath").toString());
	music.play();
	music.volume(1.0f);
	
	TeButtonLayout *newGameButton = buttonLayout("newGameButton");
	if (newGameButton)
		newGameButton->onMouseClickValidated().add(this, &MainMenu::onNewGameButtonValidated);

	TeButtonLayout *continueGameButton = buttonLayout("continueGameButton");
	if (continueGameButton) {
		continueGameButton->onMouseClickValidated().add(this, &MainMenu::onContinueGameButtonValidated);
		continueGameButton->setEnable(false);
		warning("TODO: Set continue game button enabled state based on save game manager");
	}

	TeButtonLayout *loadGameButton = buttonLayout("loadGameButton");
	if (loadGameButton)
		loadGameButton->onMouseClickValidated().add(this, &MainMenu::onLoadGameButtonValidated);

	TeButtonLayout *optionsButton = buttonLayout("optionsButton");
	if (optionsButton)
		optionsButton->onMouseClickValidated().add(this, &MainMenu::onOptionsButtonValidated);

	TeButtonLayout *galleryButton = buttonLayout("galleryButton");
	if (galleryButton)
		galleryButton->onMouseClickValidated().add(this, &MainMenu::onGalleryButtonValidated);

	TeButtonLayout *quitButton = buttonLayout("quitButton");
	if (quitButton)
		quitButton->onMouseClickValidated().add(this, &MainMenu::onQuitButtonValidated);

	// TODO: confirmation (menus/confirm/confirmNotSound.lua)
	// if TeSoundManager is not valid.

	TeLayout *panel = layout("panel");
	
	if (panel) {
		const Common::String panelTypoVal = value("panelTypo").toString();
		for (auto *child : panel->childList()) {
			TeTextLayout *childText = dynamic_cast<TeTextLayout *>(child);
			if (!childText)
				continue;
			childText->setName(panelTypoVal + childText->name());
		}
	}
	setCenterButtonsVisibility(true);
	TeTextLayout *versionNum = textLayout("versionNumber");
	if (versionNum) {
		const Common::String versionSectionStr("<section style=\"left\" /><color r=\"255\" g=\"255\" b=\"255\"/><font file=\"Common/Fonts/arial.ttf\" size=\"12\" />");
		versionNum->setText(versionSectionStr + app->getVersionString());
	}
}

void MainMenu::leave() {
	error("TODO: Implement MainMenu::leave.");
}

bool MainMenu::deleteFile(const Common::String &name) {
	error("TODO: Implement MainMenu::deleteFile");
}

bool MainMenu::onActivedTuto() {
	error("TODO: Implement MainMenu::onActivedTuto");
}

bool MainMenu::onBFGRateIt2ButtonValidated() {
	error("TODO: Implement MainMenu::onBFGRateIt2ButtonValidated");
}

bool MainMenu::onBFGRateItButtonValidated() {
	error("TODO: Implement MainMenu::onBFGRateItButtonValidated");
}

bool MainMenu::onBFGRateItQuitButtonValidated() {
	error("TODO: Implement MainMenu function");
}

bool MainMenu::onBFGUnlockGameButtonValidated() {
	error("TODO: Implement MainMenu function");
}

bool MainMenu::onContinueGameButtonValidated() {
	error("TODO: Implement MainMenu function");
}

bool MainMenu::onDisabledTuto() {
	error("TODO: Implement MainMenu function");
}

bool MainMenu::onEnterGameRotateAnimFinished() {
	error("TODO: Implement MainMenu function");
}

bool MainMenu::onGalleryButtonValidated() {
	error("TODO: Implement MainMenu function");
}

bool MainMenu::onHowToButtonValidated() {
	error("TODO: Implement MainMenu function");
}

bool MainMenu::onLoadGameButtonValidated() {
	error("TODO: Implement MainMenu function");
}

bool MainMenu::onNewGameButtonValidated() {
	error("TODO: Implement MainMenu function");
}

bool MainMenu::onNewGameConfirmed() {
	error("TODO: Implement MainMenu::onNewGameConfirmed");
}

bool MainMenu::onOptionsButtonValidated() {
	error("TODO: Implement MainMenu function");
}

bool MainMenu::onQuit() {
	g_engine->setWantToQuit();
	leave();
	return false;
}

bool MainMenu::onQuitButtonValidated() {
	//Confirm::enter("menus/confirm/confirmQuit.lua", "");
	error("TODO: Implement MainMenu function");
}

bool MainMenu::onUnlockGameButtonValidated() {
	error("TODO: Implement MainMenu function");
}

void MainMenu::refresh() {
	// TODO: get a real value
	bool haveSave = false;
	TeButtonLayout *continueGameButton = buttonLayout("continueGameButton");
	if (continueGameButton) {
		continueGameButton->setEnable(haveSave);
	}
}

void MainMenu::setCenterButtonsVisibility(bool visible) {
	bool haveSave = false;

	TeButtonLayout *continuegameunlockButton = buttonLayout("continuegameunlockButton");
	if (continuegameunlockButton) {
		continuegameunlockButton->setVisible(haveSave & visible);
	}

	TeButtonLayout *newGameUnlockButton = buttonLayout("newgameunlockButton");
	if (newGameUnlockButton) {
		newGameUnlockButton->setVisible(visible & !haveSave);
	}

	TeButtonLayout *unlockgameButton = buttonLayout("unlockgameButton");
	if (unlockgameButton) {
		unlockgameButton->setVisible(false);
	}

	TeLayout *rateItButton = layout("rateItButton");
	if (rateItButton) {
		rateItButton->setVisible(false);
	}
}



} // end namespace Syberia
