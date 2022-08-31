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
#include "syberia/game/application.h"

namespace Syberia {

Application::Application() : _finishedGame(false), _finishedFremium(false), _captureFade(false) {
}

void Application::create() {
	error("TODO: Implement me.");
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

void Application::run() {
	error("TODO: Implement me.");
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
	error("TODO: Implement me.");
}

void Application::drawFront() {
	error("TODO: Implement me.");
}

void Application::performRender() {
	error("TODO: Implement me.");
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
	error("TODO: Implement me.");
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
