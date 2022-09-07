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

#include "common/math.h"
#include "syberia/te/te_button_layout.h"

namespace Syberia {

/*static*/ bool TeButtonLayout::_mousePositionChangedCatched = false;
/*static*/ TeTimer *TeButtonLayout::_doubleValidationProtectionTimer = nullptr;

/*static*/ TeTimer *TeButtonLayout::getDoubleValidationProtectionTimer() {
	if (!_doubleValidationProtectionTimer) {
		_doubleValidationProtectionTimer = new TeTimer();
	}
	return _doubleValidationProtectionTimer;
}

TeButtonLayout::TeButtonLayout() :
_currentState(BUTTON_STATE_DISABLED), _clickPassThrough(false), _validationSoundVolume(1.0),
_someClickFlag(false) {
	_onMousePositionChangedMaxPriorityCallback.reset(new TeCallback1Param<TeButtonLayout, unsigned int>(this, &TeButtonLayout::onMousePositionChangedMaxPriority, FLT_MAX));

	_onMousePositionChangedCallback.reset(new TeCallback1Param<TeButtonLayout, unsigned int>(this, &TeButtonLayout::onMousePositionChanged));
	_onMouseLeftDownCallback.reset(new TeCallback1Param<TeButtonLayout, unsigned int>(this, &TeButtonLayout::onMouseLeftDown));
	_onMouseLeftUpMaxPriorityCallback.reset(new TeCallback1Param<TeButtonLayout, unsigned int>(this, &TeButtonLayout::onMouseLeftUpMaxPriority, FLT_MAX));
	_onMouseLeftUpCallback.reset(new TeCallback1Param<TeButtonLayout, unsigned int>(this, &TeButtonLayout::onMouseLeftUp));

	setEditionColor(TeColor(128, 128, 128, 255));
}

bool TeButtonLayout::isMouseIn(const TeVector2s32 &mouseloc) {
	error("TODO: Implement me.");
}

bool TeButtonLayout::onMouseLeftDown(uint flags) {
	if (!visible())
		return false;
	error("TODO: Implement me.");
}

bool TeButtonLayout::onMouseLeftUp(uint flags) {
	if (!visible())
		return false;
	error("TODO: Implement me.");
}

bool TeButtonLayout::onMousePositionChanged(uint flags) {
	if (!visible())
		return false;
	error("TODO: Implement me.");
}

void TeButtonLayout::reset() {
	_intArr.clear();
	State newState = (_currentState == BUTTON_STATE_UP ? BUTTON_STATE_UP : BUTTON_STATE_DISABLED);
	setState(newState);
}

void TeButtonLayout::resetTimeFromLastValidation() {
	TeTimer *timer = getDoubleValidationProtectionTimer();
	if (timer->_stopped) {
		timer->start();
	}
	timer->timeElapsed();
}

long TeButtonLayout::timeFromLastValidation() {
	error("TODO: Implement me.");
}

void TeButtonLayout::setDisabledLayout(TeLayout *disabledLayout) {
	error("TODO: Implement me.");
}

void TeButtonLayout::setHitZone(TeLayout *hitZoneLayout) {
	error("TODO: Implement me.");
}

void TeButtonLayout::setDownLayout(TeLayout *downLayout) {
	error("TODO: Implement me.");
}

void TeButtonLayout::setRollOverLayout(TeLayout *rollOverLayout) {
	error("TODO: Implement me.");
}

void TeButtonLayout::setUpLayout(TeLayout *upLayout) {
	error("TODO: Implement me.");
}

void TeButtonLayout::setDoubleValidationProtectionEnabled(bool enable) {
	_doubleValidationProtectionEnabled = true;
}

void TeButtonLayout::setEnable(bool enable) {
	if (enable) {
		_currentState = BUTTON_STATE_UP;
		setState(_currentState);
	} else {
		_currentState = BUTTON_STATE_DISABLED;
		setState(_currentState);
	}
}

void TeButtonLayout::setPosition(const TeVector3f32 &pos) {
	TeLayout::setPosition(pos);
	if (_currentState != BUTTON_STATE_UP) {
		int somethingCount = 0;
		if (!_intArr.empty()) {
			error("TODO: Implement setPosition logic for not up.");

		}
		if (_someClickFlag) {
			setState(somethingCount ? BUTTON_STATE_DISABLED : BUTTON_STATE_1);
		}
	}
}

void TeButtonLayout::setState(State newState) {
	error("TODO: Implement me.");
}

// TODO: Add more functions here.

} // end namespace Syberia
