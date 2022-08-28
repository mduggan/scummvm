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

#ifndef SYBERIA_GAME_GAME_H
#define SYBERIA_GAME_GAME_H

#include "common/types.h"
#include "common/str.h"
#include "syberia/game/documents_browser.h"
#include "syberia/game/inventory.h"
#include "syberia/game/inventory_menu.h"
#include "syberia/game/in_game_scene.h"
#include "syberia/game/notifier.h"
#include "syberia/te/te_lua_gui.h"
#include "syberia/te/te_music.h"
#include "syberia/te/te_checkbox_layout.h"
#include "syberia/te/te_vector2s32.h"

namespace Syberia {

class Game {
public:
	Game();

	class HitObject {
		byte OnChangeWarp();
		byte OnDown();
		byte OnUp();
		byte OnValidated();
		//byte OnVisible(); empty never used?
	};
	
	class RandomSound {
		byte onSoundFinished();
	};
	
	//enum EGameScoreID {}; // Not needed?
	
	bool addAnimToSet(const Common::String &path);
	void addArtworkUnlocked(const Common::String &name, bool bonus);
	void addNoScale2Child(TeILayout *layout); // FIXME: check return type
	void addNoScale2Children();
	void addNoScaleChildren();
	void addRandomSound(const Common::String &s1, const Common::String &s2, float f1, float f2);
	void addToBag(const Common::String &objname);
	void addToHand(const Common::String &objname);
	void addToScore(int score);
	void attachButtonsLayoutGoto() {}; // does nothing?
	void createButtonsLayoutGoto() {}; // does nothing?
	void deleteButtonsLayoutGoto() {}; // does nothing?

	bool changeWarp(const Common::String &zone, const Common::String &scene, bool fadeFlag);
	bool changeWarp2(const Common::String &zone, const Common::String &scene, bool fadeFlag);
	
	void deleteNoScale();
	void draw();
	void enter();
	static TeI3DObject2 *findLayoutByName(TeILayout *ilayout, const Common::String &name);
	static TeSpriteLayout *findSpriteLayoutByName(TeILayout *ilayout, const Common::String &name);

	void finishFreemium();
	void finishGame();
	void initLoadedBackupData();
	void initNoScale();
	void initScene(bool param_1, const Common::String &scenePath);
	void initWarp(const Common::String &zone, const Common::String &scene, bool fadeFlag);
	bool isDocumentOpened();
	bool isMouse() { return false; }
	bool isMoviePlaying();
	bool launchDialog(const Common::String &param_1, uint param_2, const Common::String &param_3,
					  const Common::String &param_4, float param_5);
	void leave(bool flag);
	bool loadBackup(const Common::String &path);
	bool loadCharacter(const Common::String &name);
	bool loadPlayerCharacter(const Common::String &name);
	bool loadScene(const Common::String &name);
	
	bool onAnswered(const Common::String &val);
	bool onCallNumber(const Common::String &val);
	bool onCharacterAnimationFinished(const Common::String &val);
	bool onCharacterAnimationPlayerFinished(const Common::String &val);
	bool onDialogFinished(const Common::String &val);
	bool onDisplacementFinished();
	bool onFinishedCheckBackup(bool result);
	bool onFinishedLoadingBackup(const Common::String &val);
	bool onFinishedSavingBackup(int something);
	bool onInventoryButtonValidated();
	bool onLockVideoButtonValidated();
	bool onMarkersVisible(TeCheckboxLayout::State state);
	bool onMouseClick(uint flags);
	bool onMouseMove();
	bool onSkipVideoButtonValidated();
	bool onVideoFinished();
	
	void pauseMovie();
	void pauseSounds() {}; // does nothing?
	void playMovie(const Common::String &s1, const Common::String &musicPath);
	void playRandomSound(const Common::String &name);
	void playSound(const Common::String &name, int param_2, float param_3);
	void removeNoScale2Child(TeILayout *layout);
	void removeNoScale2Children();
	void removeNoScaleChildren();
	void resetPreviousMousePos();
	void resumeMovie();
	void resumeSounds() {}; // does nothing?
	void saveBackup(const Common::String &saveName);
	void setBackground(const Common::String &name);
	void setCurrentObjectSprite(const Common::String &spritePath);
	bool showMarkers(bool val);
	bool startAnimation(const Common::String &animName, int param_2, bool param_3);
	void startAnimationPart(const Common::String &param_1, int param_2, int param_3, int param_4, bool param_5) {};
	void stopSound(const Common::String &name);
	bool unloadCharacter(const Common::String &character);
	bool unloadCharacters();
	bool unloadPlayerCharacter(const Common::String &character);
	void update();

	InventoryMenu &inventoryMenu() { return _inventoryMenu; }
private:
	TeLuaGUI _gui1;
	TeLuaGUI _gui2;
	TeLuaGUI _gui3;
	TeLuaGUI _gui4;
	
	Inventory _inventory;
	InventoryMenu _inventoryMenu;
	int _score;
	
	bool _running;
	int _frameCounter;
	
	InGameScene _scene;
	
	static char **_objectsTakenIDs;
	
	TeVector2s32 _previousMousePos;

	Common::String _warpZone;
	Common::String _warpScene;
	bool _warpFlag;
	bool _warped;
	
	TeTimer _playedTimer;
	TeLuaScript _luaScript;
	TeMusic _music;
	Notifier _notifier;
	DocumentsBrowser _documentsBrowser;

	static const int NUM_OBJECTS_TAKEN_IDS = 5;
	static const char *OBJECTS_TAKEN_IDS[NUM_OBJECTS_TAKEN_IDS];
	bool _objectsTakenBits[NUM_OBJECTS_TAKEN_IDS];
	int _objectsTakenVal;

	bool _sceneCharacterVisibleFromLoad;
	bool _markersVisible;
};

} // end namespace Syberia

#endif // SYBERIA_GAME_GAME_H
