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

#include "syberia/te/te_animation.h"

namespace Syberia {

Common::Array<TeAnimation *> TeAnimation::_animations;

TeAnimation::TeAnimation() {
}


void TeAnimation::cont() {
	if (_runTimer._stopped) {
		_runTimer.start();
		_animations.push_back(this);
		// double seconds = _runTimer.getTimeFromStart() / 1000.0;
		// TODO: call some virtual function with `seconds`
	}
}

void TeAnimation::removeThisFromAnimations() {
	// Find and remove this one
	Common::Array<TeAnimation *>::iterator iter;
	for (iter = _animations.begin(); iter != _animations.end(); iter++) {
		if (*iter == this) {
			_animations.erase(iter);
			break;
		}
	}
}

void TeAnimation::pause() {
	if (!_runTimer._stopped) {
		removeThisFromAnimations();
		_runTimer.pause();
	}
}

void TeAnimation::stop() {
	if (!_runTimer._stopped) {
		removeThisFromAnimations();
		_runTimer.stop();
		_stopCallbacks.call();
	}
}

void TeAnimation::reset() {
	if (!_runTimer._stopped) {
		removeThisFromAnimations();
		stop();
	}
}

void TeAnimation::seekToStart() {
	_runTimer.stop();
	_runTimer.start();
}

void TeAnimation::pauseAll() {
	// TODO: Original checks some flag on the animation timers.. pausable?
	uint len = _animations.size();
	for (uint i = 0; i < len; i++) {
		_animations[i]->pause();
	}
}

void TeAnimation::resumeAll() {
	uint len = _animations.size();
	for (uint i = 0; i < len; i++) {
		_animations[i]->pause();
	}
}

void TeAnimation::updateAll() {
	uint len = _animations.size();
	for (uint i = 0; i < len; i++) {
		// TODO: update based on _animations[i]->timer.getTimeFromStart()
	}

}

} // end namespace Syberia
