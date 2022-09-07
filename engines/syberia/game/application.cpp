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

#include "common/textconsole.h"
#include "common/file.h"
#include "common/util.h"

#include "graphics/opengl/glad.h"

#include "syberia/syberia.h"
#include "syberia/game/game.h"
#include "syberia/game/application.h"
#include "syberia/te/te_core.h"
#include "syberia/te/te_resource_manager.h"
#include "syberia/te/te_renderer.h"
#include "syberia/te/te_font3.h"
//#include "syberia/te/te_textbase2.h"

namespace Syberia {

bool Application::_dontUpdateWhenApplicationPaused = false;

Application::Application() : _finishedGame(false), _finishedFremium(false), _captureFade(false), _difficulty(1) {
	TeCore *core = g_engine->getCore();
	core->_coreNotReady = true;
	core->fileFlagSystemSetFlag("platform", "MacOSX");
	core->fileFlagSystemSetFlag("part", "Full");
	core->fileFlagSystemSetFlag("distributor", "DefaultDistributor");
}

void Application::create() {
	TeResourceManager *resmgr = g_engine->getResourceManager();
	_fontComic = resmgr->getResource<TeFont3>("Common/Fonts/ComicRelief.ttf");
	_fontArgh = resmgr->getResource<TeFont3>("Common/Fonts/Argh.ttf");
	_fontArial = resmgr->getResource<TeFont3>("Common/Fonts/arlia.ttf");
	_fontChaucer = resmgr->getResource<TeFont3>("Common/Fonts/CHAUCER.TTF");
	_fontColaborate = resmgr->getResource<TeFont3>("Common/Fonts/Colaborate-Regular.otf");
	_fontProDisplay = resmgr->getResource<TeFont3>("Common/Fonts/ProDisplay.ttf");
	
	// Prebuild some fonts.. cover letters, numbers, a few accented chars, and punctuation.
	warning("TODO: Build some text with TeTextBase2 here.");
	/*
	TeTextBase2 textBase;
	textBase.setText("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/,*?;.:/!\xA7&\xE9\"'(-\xE8_\xE7\xE0)=");
	textBase.setFont(0, _fontComic);
	textBase.setRect(TeVector2s32(1, 1));
	textBase.setFontSize(12);
	textBase.build();
	textBase.setFontSize(14);
	textBase.build();
	textBase.setFontSize(16);
	textBase.build();
	textBase.setFontSize(18);
	textBase.build();
	textBase.setFontSize(30);
	textBase.build();
	textBase.setFont(0, _fontColaborate);
	textBase.setFontSize(18);
	textBase.build();
	textBase.setFont(0, _fontProDisplay);
	textBase.setFontSize(24);
	textBase.build();
	*/

	TeCore *core = g_engine->getCore();
	static const char allLangs[][3] = {"en", "fr", "de", "es", "it", "ru"};
	const Common::Path textsPath("texts");
	
	// Try alternate langs..
	int i = 0;
	Common::Path textFilePath;
	while (i < ARRAYSIZE(allLangs)) {
		textFilePath = textsPath.join(core->language() + ".xml");
		if (Common::File::exists(textFilePath))
			break;
		core->language(allLangs[i]);
		i++;
	}
	if (i == ARRAYSIZE(allLangs)) {
		error("Couldn't find texts/[lang].xml for any language.");
	}

	_loc.load(textFilePath);
	core->addLoc(&_loc);

	const Common::Path helpMenuPath("menus/help/help_");
	Common::Path helpMenuFilePath;
	i = 0;
	while (i < ARRAYSIZE(allLangs)) {
		helpMenuFilePath = helpMenuPath.append(core->language() + ".xml");
		if (Common::File::exists(helpMenuFilePath))
			break;
		core->language(allLangs[i]);
		i++;
	}
	if (i == ARRAYSIZE(allLangs)) {
		error("Couldn't find menus/help/help_[lang].xml for any language.");
	}

	_helpGui.load(helpMenuFilePath);
	
	warning("TODO: set some TeCore flags here..");

	// Game calls these here but does nothing with result?
	//TeGetDeviceDPI();
	//TeGetDeviceReferenceDPI();

	_backLayout.setName("layoutBack");
	_backLayout.setSizeType(TeLayout::CoordinatesType::RELATIVE_TO_PARENT);
	_backLayout.setSize(TeVector3f32(1.0, 1.0, 0.0));
	warning("TODO: call some mainwindow virtual function with _backLayout here..");
	
	_frontOrientationLayout.setName("orientationLayoutFront");
	_frontOrientationLayout.setSizeType(TeLayout::CoordinatesType::RELATIVE_TO_PARENT);
	_frontOrientationLayout.setSize(TeVector3f32(1.0, 1.0, 0.0));
	warning("TODO: call some mainwindow virtual function with _frontOrientationLayout here..");
	
	_frontLayout.setName("layoutFront");
	_frontLayout.setSizeType(TeLayout::CoordinatesType::RELATIVE_TO_PARENT);
	_frontLayout.setSize(TeVector3f32(1.0, 1.0, 0.0));
	
	_visFade.init();
	
	_backLayout.addChild(&_visFade._fadeCaptureSprite);
	_backLayout.addChild(&_visFade._blackFadeSprite);
	_backLayout.addChild(&_visFade._buttonLayout);

	_frontLayout.addChild(&_appSpriteLayout);
	_appSpriteLayout.setSizeType(TeLayout::CoordinatesType::RELATIVE_TO_PARENT);
	_appSpriteLayout.setSize(TeVector3f32(1.0, 1.0, 1.0));
	
	// Note: The games do some loading of a "version.ver" file here to add a
	// watermark to the backLayout, but that file doesn't exist in any of the
	// GOG games so it was probably only used during development.
	const Common::Path verFilePath("version.ver");
	if (Common::File::exists(verFilePath)) {
		warning("Skipping doing anything with version.ver file");
	}
	
	_mouseCursorLayout.setName("mouseCursor");
	
	// Not needed in scummvm:
	//mainWindow->setNativeCursorVisible(false);
	
	_mouseCursorLayout.load("pictures/cursor.png");
	_mouseCursorLayout.setAnchor(TeVector3f32(0.3, 0.1, 0.0));
	_backLayout.addChild(&_mouseCursorLayout);
	
	_lockCursorButton.setName("lockCursorButton");
	_lockCursorButton.setSizeType(TeLayout::CoordinatesType::RELATIVE_TO_PARENT);
	_lockCursorButton.setSize(TeVector3f32(2.0, 0.095, 0.0));
	_lockCursorButton.setPositionType(TeLayout::CoordinatesType::RELATIVE_TO_PARENT);
	_lockCursorButton.setPosition(TeVector3f32(0.95, 0.95, 0.0));
	_backLayout.addChild(&_lockCursorButton);
	
	_lockCursorFromActionButton.setName("lockCursorFromActionButton");
	_lockCursorFromActionButton.setSizeType(TeLayout::CoordinatesType::RELATIVE_TO_PARENT);
	_lockCursorFromActionButton.setSize(TeVector3f32(2.0, 2.0, 0.0));
	_backLayout.addChild(&_lockCursorFromActionButton);

	_autoSaveIcon1.setName("autosaveIcon");
	_autoSaveIcon1.setAnchor(TeVector3f32(0.5, 0.5, 0.0));
	_autoSaveIcon1.setPosition(TeVector3f32(0.2, 0.9, 0.0));
	_autoSaveIcon1.setSize(TeVector3f32(128.0, 64.0, 0.0));
	_autoSaveIcon1.load("menus/inGame/autosave_icon.png");
	_backLayout.addChild(&_autoSaveIcon1);
	
	_autoSaveIconAnim1._runTimer.pausable(false);
	_autoSaveIconAnim1.pause();
	_autoSaveIconAnim1._firstVal = TeColor(255, 255, 255, 0);
	_autoSaveIconAnim1._secondVal = TeColor(255, 255, 255, 255);
	Common::Array<float> curve;
	curve.push_back(0.0);
	curve.push_back(1.0);
	curve.push_back(1.0);
	curve.push_back(0.0);
	_autoSaveIconAnim1.setCurve(curve);
	_autoSaveIconAnim1._maxTime = 4000.0;
	_autoSaveIconAnim1._callbackObj = &_autoSaveIcon1;
	_autoSaveIconAnim1._callbackMethod = &Te3DObject2::setColor;

	_autoSaveIcon2.setName("autosaveIcon");
	_autoSaveIcon2.setAnchor(TeVector3f32(0.5, 0.5, 0.0));
	_autoSaveIcon2.setPosition(TeVector3f32(0.2, 0.7, 0.0));
	_autoSaveIcon2.setSize(TeVector3f32(68.0, 86.0, 0.0));
	_autoSaveIcon2.load("menus/inGame/NoCel.png");
	_backLayout.addChild(&_autoSaveIcon2);

	_autoSaveIconAnim2._runTimer.pausable(false);
	_autoSaveIconAnim2.pause();
	_autoSaveIconAnim2._firstVal = TeColor(255, 255, 255, 0);
	_autoSaveIconAnim2._secondVal = TeColor(255, 255, 255, 255);
	_autoSaveIconAnim1.setCurve(curve);
	_autoSaveIconAnim1._maxTime = 4000.0;
	_autoSaveIconAnim1._callbackObj = &_autoSaveIcon2;
	_autoSaveIconAnim1._callbackMethod = &Te3DObject2::setColor;

	_blackFadeAnimationFinishedSignal.add<Application>(this, &Application::onBlackFadeAnimationFinished);

	warning("TODO: Register for onMousePositionChanged callback from inputmgr here.");

	onMainWindowSizeChanged();
	_splashScreens.enter();
	_created = true;
}

void Application::destroy() {
	error("TODO: Implement me.");
}

void Application::startGame() {
	error("TODO: Implement me.");
}

void Application::resume() {
	error("TODO: Implement me.");
}

bool Application::run() {
	if (_created) {
		TeTimer::updateAll();
		if (!_dontUpdateWhenApplicationPaused) {
			error("TODO: finish commented-out bits.");
			//_inputmgr->update();
			TeAnimation::updateAll();
			//TeVideo::updateAll();
		}
		_captureFade = false;

		TeRenderer *renderer = g_engine->getRenderer();
		Game *game = g_engine->getGame();

		renderer->reset();
		game->update();
		//_soundManager->update(soundmgr);
		performRender();
		if (game->_returnToMainMenu) {
			game->leave(true);
			if (!game->_luaShowOwnerError) {
				_mainMenu.enter();
			} else {
				_ownerErrorMenu.enter();
			}
			game->_returnToMainMenu = false;
		}
		if (_gameFinished) {
			game->leave(false);
			_mainMenu.enter();
			TeLuaGUI finalGui;
			if (Common::File::exists("finalURL.lua")) {
				finalGui.load("finalURL.lua");
				/*TeVariant finalVal =*/ finalGui.value("finalURL");
				warning("TODO: use final URL??");
				// TODO: Not clear how this variant is ever used??
				finalGui.unload();
			}
			_gameFinished = false;
	  }
	  InGameScene::updateScroll();
	  TeObject::deleteNow();
	}
	return true;
}

void Application::suspend() {
	error("TODO: Implement me.");
}

void Application::showNoCellIcon(bool show) {
	error("TODO: Implement me.");
}

void Application::showLoadingIcon(bool show) {
	error("TODO: Implement me.");
}

void Application::saveCorrupted(const Common::String &fname) {
	error("TODO: Implement me.");
}

void Application::drawBack() {
	warning("TODO: apply main window camera.");
	_backLayout.draw();
	TeCamera::restore();
	g_engine->getRenderer()->loadIdentityMatrix();
}

void Application::drawFront() {
	warning("TODO: apply main window camera.");
	_frontOrientationLayout.draw();
	TeCamera::restore();
	g_engine->getRenderer()->loadIdentityMatrix();
}

void Application::performRender() {
	Game *game = g_engine->getGame();
	TeRenderer *renderer = g_engine->getRenderer();

	if (game->_running && _inGameScene._character && true /*some other ingamescene things*/) {
		renderer->shadowMode(TeRenderer::ShadowMode1);
		//_inGameScene._charactersShadow->createTexture(_inGameScene);
		renderer->shadowMode(TeRenderer::ShadowMode0);
		error("TODO: Implement characters shadow thing here.");
	}

	drawBack();
	
	renderer->renderTransparentMeshes();
	renderer->clearBuffer(GL_ACCUM);
	
	if (game->_running && _inGameScene._character && true /*some other ingamescene things*/) {
		Common::SharedPtr<TeCamera> currentCamera = _inGameScene.currentCamera();
		if (currentCamera) {
			currentCamera->apply();
			renderer->shadowMode(TeRenderer::ShadowMode2);
			//_inGameScene._charactersShadow->draw(_inGameScene);
			renderer->shadowMode(TeRenderer::ShadowMode0);
			error("TODO: Implement characters shadow thing here.");
		}
		game->draw();
	}
	
	renderer->renderTransparentMeshes();
	renderer->clearBuffer(GL_ACCUM);
	drawFront();
	renderer->renderTransparentMeshes();
	// What gets called here??
	//_inGameScene.removeModel(<#const Common::String &name#>)
}

//void Application::preloadTextrue(); does nothing..

void Application::fade() {
	_visFade.animateFade();
}

void Application::blackFade() {
	_visFade.animateBlackFade();
}

void Application::captureFade() {
	if (_captureFade)
		return;
	_captureFade = true;
	performRender();
	_visFade.captureFrame();
}

bool Application::isFading() {
	error("TODO: Implement me.");
	return false;
}

bool Application::onBlackFadeAnimationFinished() {
	error("TODO: Implement me.");
	return false;
}

bool Application::onMainWindowSizeChanged() {
	// This sets HD or SD "definition" in the core depending on device DPI.
	// For now just default to SD.
	debug("mainWindowSizeChanged: defaulting to SD.");
	g_engine->getCore()->fileFlagSystemSetFlag("definition", "SD");
	return false;
}

bool Application::onMousePositionChanged() {
	error("TODO: Implement me.");
	return false;
}

bool Application::isLockCursor() {
	error("TODO: Implement me.");
	return false;
}

bool Application::isLockPad() {
	error("TODO: Implement me.");
	return false;
}

void Application::lockCursor(bool lock) {
	error("TODO: Implement me.");
}

void Application::lockCursorFromAction(bool lock) {
	error("TODO: Implement me.");
}

void Application::loadOptions(const Common::String &fname) {
	error("TODO: Implement me.");
}

void Application::saveOptions(const Common::String &fname) {
	error("TODO: Implement me.");
}

const Common::String &Application::getHelpText(const Common::String &key) {
	return _helpGui.value(key);
}

const char *Application::inAppUnlockFullVersionID() {
	error("TODO: Implement me.");
}

// TODO: Add more functions here.

} // end namespace Syberia
