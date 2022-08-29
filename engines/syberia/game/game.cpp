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

#include "common/file.h"
#include "common/path.h"
#include "common/str-array.h"
#include "common/system.h"

#include "syberia/syberia.h"
#include "syberia/game/application.h"
#include "syberia/game/character.h"
#include "syberia/game/in_game_scene.h"
#include "syberia/game/game.h"
#include "syberia/te/te_variant.h"

namespace Syberia {

Game::Game() : _objectsTakenVal(0), _objectsTakenBits{false, false, false, false, false}, _score(0) {
}

/*static*/ const char *Game::OBJECTS_TAKEN_IDS[5] = {
	"BCylindreBarr",
	"VCylindreMusique",
	"VCylindreVal",
	"CCylindreCite",
	"VPoupeeMammouth"
};

static Common::StringArray split(const Common::String &text, char c) {
	Common::StringArray values;

	Common::String str = text;
	size_t pos;
	while ((pos = str.findFirstOf(c)) != Common::String::npos) {
		values.push_back(Common::String(str.c_str(), pos));
		str = Common::String(str.c_str() + pos + 1);
	}

	values.push_back(str);
	return values;
}

bool Game::addAnimToSet(const Common::String &anim) {
	// Get path to lua script, eg scenes/ValVoralberg/14040/Set14040.lua
	const Common::Path animPath(Common::String("scenes/") + anim + "/");
	
	bool retval = false;
	if (Common::File::exists(animPath)) {
		Common::StringArray parts = split(anim, '/');
		assert(parts.size() >= 2);

		Common::String layoutName = parts[1];
		Common::String path = Common::String("scenes/") + parts[0] + "/" + parts[1] + "/Set" + parts[1];

		_gui2.load(path + ".lua");
		/*
		TeILayout *layout = _gui2.layout("root");
		TeSpriteLayout *spriteLayout2 = findSpriteLayoutByName(layout, layoutName);

		TeLayout *layout2 = TeLuaGUI::layout(&(this->scene).field_0x170,"root");
		long lVar5 = 0;
		if (spriteLayout2) {
			lVar5 = (long)plVar3 + *(long *)(*plVar3 + -0x198);
		}
		(**(code **)(*(long *)((long)&pTVar2->vptr + (long)pTVar2->vptr[-0x33]) + 0x30))
					((long)&pTVar2->vptr + (long)pTVar2->vptr[-0x33],lVar5);
		 */
	  retval = true;
	}

	return retval;
}

void Game::addArtworkUnlocked(const Common::String &name, bool bonus) {
	error("TODO: Implemet me");
}

void Game::addNoScale2Child(TeILayout *layout) { // FIXME: check return type
	error("TODO: Implemet me");
}

void Game::addNoScale2Children() {
	error("TODO: Implemet me");
}

void Game::addNoScaleChildren() {
	error("TODO: Implemet me");
}

void Game::addRandomSound(const Common::String &s1, const Common::String &s2, float f1, float f2) {
	error("TODO: Implemet me");
}

void Game::addToBag(const Common::String &objname) {
	if (_inventory.objectCount(objname) != 0)
		return;
	_inventory.addObject(objname);
	Common::String imgpath("Inventory/Objects/");
	imgpath += _inventory.objectName(objname);
	imgpath += ".png";
	_notifier.push(objname, imgpath);
	for (int i = 0; i < NUM_OBJECTS_TAKEN_IDS; i++) {
		if (objname == OBJECTS_TAKEN_IDS[i] && !_objectsTakenBits[i]) {
			_objectsTakenBits[i] = true;
			_objectsTakenVal++;
		}
	}
	// Seeems strange as we're already in Game, but that's what original does?
	Game *game = g_engine->getGame();
	game->_score += 10;
	debug("Updated score: %d", game->_score);
}

void Game::addToHand(const Common::String &objname) {
	_inventory.addObject(objname);
	_inventory.selectedObject(objname);
}

void Game::addToScore(int score) {
	_score += score;
}

bool Game::changeWarp(const Common::String &zone, const Common::String &scene, bool fadeFlag) {
	Application *app = g_engine->getApplication();
	if (fadeFlag) {
		app->blackFade();
	} else {
		app->captureFade();
	}
	_warpZone = zone;
	_warpScene = scene;
	_warpFlag = fadeFlag;
	_warped = true;
	return true;
}

bool Game::changeWarp2(const Common::String &zone, const Common::String &scene, bool fadeFlag) {
	error("TODO: Implemet me");
}

void Game::deleteNoScale() {
	error("TODO: Implemet me");
}

void Game::draw() {
	if (_running) {
	  _frameCounter++;
	  _scene.draw();
	}
}

void Game::enter() {
	error("TODO: Implemet me");
}

/*static*/ TeI3DObject2 *Game::findLayoutByName(TeILayout *ilayout, const Common::String &name) {
	error("TODO: Implemet me");
}

/*static*/ TeSpriteLayout *Game::findSpriteLayoutByName(TeILayout *ilayout, const Common::String &name) {
	error("TODO: Implemet me");
}

void Game::finishFreemium() {
	Application *app = g_engine->getApplication();
	app->_finishedGame = true;
	app->_finishedFremium = true;
}

void Game::finishGame() {
	Application *app = g_engine->getApplication();
	app->_finishedGame = true;
	_playedTimer.stop();
	/* Game does this but does nothing with result?
	if (app->difficulty() == 2) {
	  _playedTimer.getTimeFromStart();
	} */
	app->credits().enter(false);
}

void Game::initLoadedBackupData() {
	error("TODO: Implemet me");
}

void Game::initNoScale() {
	error("TODO: Implemet me");
}

void Game::initScene(bool param_1, const Common::String &scenePath) {
	error("TODO: Implemet me");
}

void Game::initWarp(const Common::String &zone, const Common::String &scene, bool fadeFlag) {
	error("TODO: Implemet me");
}

bool Game::isDocumentOpened() {
	TeLayout *layout = _documentsBrowser.layout("zoomed");
	return layout->visible(); 
}

bool Game::isMoviePlaying() {
	TeButtonLayout *vidButton = _gui4.buttonLayout("videoBackgroundButton");
	if (vidButton)
		return vidButton->visible();
	return false;
}

bool Game::launchDialog(const Common::String &param_1, uint param_2, const Common::String &param_3,
				  const Common::String &param_4, float param_5)  {
	error("TODO: Implemet me");
}

void Game::leave(bool flag) {
	error("TODO: Implemet me");
}

bool Game::loadBackup(const Common::String &path) {
	error("TODO: Implemet me");
}

bool Game::loadCharacter(const Common::String &name) {
	bool result = true;
	Character *character = _scene.character(name);
	if (!character) {
		result = false;
		bool loaded = _scene.loadCharacter(name);
		if (loaded) {
			character = _scene.character(name);
			error("TODO: Implement the signal setup here.");
		}
	}
	return result;
}

bool Game::loadPlayerCharacter(const Common::String &name) {
	bool result = _scene.loadPlayerCharacter(name);
	if (result) {
		error("TODO: Implement the signal setup here.");
	}
	return result;
}

bool Game::loadScene(const Common::String &name) {
	_luaScript.load("scenes/OnGameEnter.lua");
	_luaScript.execute();
	Character *character = _scene._character;
	if (character && character->_model->visible()) {
		_sceneCharacterVisibleFromLoad = true;
	}
	return false;
}

bool Game::onAnswered(const Common::String &val) {
	_luaScript.execute("OnAnswered", TeVariant(val));
	return false;
}

bool Game::onCallNumber(const Common::String &val) {
	_luaScript.execute("OnCallNumber", TeVariant(val));
	return false;
}

bool Game::onCharacterAnimationFinished(const Common::String &val) {
	error("TODO: Implemet me");
}

bool Game::onCharacterAnimationPlayerFinished(const Common::String &val) {
	error("TODO: Implemet me");
}

bool Game::onDialogFinished(const Common::String &val) {
	error("TODO: Implemet me");
}

bool Game::onDisplacementFinished() {
	error("TODO: Implemet me");
}

bool Game::onFinishedCheckBackup(bool result) {
	error("TODO: Implemet me");
}

bool Game::onFinishedLoadingBackup(const Common::String &val) {
	error("TODO: Implemet me");
}

bool Game::onFinishedSavingBackup(int something) {
	error("TODO: Implemet me");
}

bool Game::onInventoryButtonValidated() {
	_inventoryMenu.enter();
	return false;
}

bool Game::onLockVideoButtonValidated() {
	error("TODO: Implemet me");
}

bool Game::onMarkersVisible(TeCheckboxLayout::State state) {
	_markersVisible = (state == 0);
	showMarkers(state == 0);
	return false;
}

bool Game::onMouseClick(uint flags)  {
	error("TODO: Implemet me");
}

bool Game::onMouseMove() {
	error("TODO: Implemet me");
}

bool Game::onSkipVideoButtonValidated() {
	error("TODO: Implemet me");
}

bool Game::onVideoFinished() {
	error("TODO: Implemet me");
}

void Game::pauseMovie() {
	_music.pause();
	_gui4.spriteLayout("video");
	error("TODO: Finish implementation if pauseMovie");
}

void Game::playMovie(const Common::String &s1, const Common::String &musicPath) {
	Application *app = g_engine->getApplication();
	app->captureFade();
	TeButtonLayout *videoBackgroundButton = _gui4.buttonLayout("videoBackgroundButton");
	videoBackgroundButton->setVisible(true);

	TeButtonLayout *skipVideoButton = _gui4.buttonLayout("skipVideoButton");
	skipVideoButton->setVisible(false);

	TeMusic &music = app->music();
	music.stop();
	music.setChannelName("video");
	music.repeat(false);
	music.volume(1.0f);
	music.load(musicPath);

	_running = false;

	TeSpriteLayout *videoSpriteLayout = _gui4.spriteLayout("video");
	videoSpriteLayout->load(s1);
	videoSpriteLayout->setVisible(true);
	music.play();
	videoSpriteLayout->play();

	app->fade();
}

void Game::playRandomSound(const Common::String &name) {
	error("TODO: Implemet me");
}

void Game::playSound(const Common::String &name, int param_2, float param_3) {
	error("TODO: Implemet me");
}

void Game::removeNoScale2Child(TeILayout *layout) {
	error("TODO: Implement me.");
}

void Game::removeNoScale2Children() {
	error("TODO: Implemet me");
}

void Game::removeNoScaleChildren() {
	error("TODO: Implemet me");
}

void Game::resetPreviousMousePos() {
	_previousMousePos = TeVector2s32(-1, -1);
}

void Game::resumeMovie() {
	error("TODO: Implemet me");
}

void Game::saveBackup(const Common::String &saveName) {
	error("TODO: Implemet me");
}

void Game::setBackground(const Common::String &name) {
	_scene.changeBackground(name);
}

void Game::setCurrentObjectSprite(const Common::String &spritePath) {
	error("TODO: Implemet me");
}

bool Game::showMarkers(bool val) {
	error("TODO: Implemet me");
}

bool Game::startAnimation(const Common::String &animName, int param_2, bool param_3) {
	error("TODO: Implemet me");
}

void Game::stopSound(const Common::String &name) {
	error("TODO: Implemet me");
}

bool Game::unloadCharacter(const Common::String &character) {
	error("TODO: Implemet me");
}

bool Game::unloadCharacters() {
	error("TODO: Implemet me");
}

bool Game::unloadPlayerCharacter(const Common::String &character) {
	_scene.unloadPlayerCharacter(character);
	return true;
}

void Game::update() {
	error("TODO: Implemet me");
}

} // end namespace Syberia
