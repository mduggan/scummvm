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

#ifndef SYBERIA_GAME_APPLICATION_H
#define SYBERIA_GAME_APPLICATION_H

#include "common/str.h"
#include "syberia/game/bonus_menu.h"
#include "syberia/game/credits.h"
#include "syberia/game/global_bonus_menu.h"
#include "syberia/game/main_menu.h"
#include "syberia/te/te_visual_fade.h"
#include "syberia/te/te_music.h"

namespace Syberia {

class Application {
public:
	Application();

	void create();
	void destroy();

	void startGame();
	void resume();
	void run();
	void suspend();
	void showNoCellIcon(bool show);
	void showLoadingIcon(bool show);
	void saveCorrupted(const Common::String &fname);
	
	void drawBack();
	void drawFront();
	void performRender();
	//void preloadTextrue(); does nothing..
	
	void fade();
	void blackFade();
	void captureFade();
	bool isFading();
	bool onBlackFadeAnimationFinished();
	bool onMainWindowSizeChanged();
	bool onMousePositionChanged();

	bool isLockCursor();
	bool isLockPad();
	void lockCursor(bool lock);
	void lockCursorFromAction(bool lock);
	
	void loadOptions(const Common::String &fname);
	void saveOptions(const Common::String &fname);

	const Common::String &getHelpText(const Common::String &key);

	const char *inAppUnlockFullVersionID();
	
	BonusMenu &bonusMenu() { return _bonusMenu; }
	GlobalBonusMenu &globalBonusMenu() { return _globalBonusMenu; }
	MainMenu &mainMenu() { return _mainMenu; }
	TeMusic &music() { return _music; }
	Credits &credits() { return _credits; }
	TeVisualFade &visualFade() { return _visFade; }

	bool _finishedGame;
	bool _finishedFremium;

private:
	TeVisualFade _visFade;
	TeMusic _music;

	GlobalBonusMenu _globalBonusMenu;
	BonusMenu _bonusMenu;
	MainMenu _mainMenu;
	Credits _credits;
	
	bool _created;

};

} // end namespace Syberia

#endif // SYBERIA_GAME_APPLICATION_H
