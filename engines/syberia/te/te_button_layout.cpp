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
_currentState(BUTTON_STATE_UP), _clickPassThrough(false), _validationSoundVolume(1.0),
_someClickFlag(false), _doubleValidationProtectionEnabled(true), _upLayout(nullptr),
_downLayout(nullptr), _rolloverLayout(nullptr), _disabledLayout(nullptr),
_hitZoneLayout(nullptr)
{
	_onMousePositionChangedMaxPriorityCallback.reset(new TeCallback1Param<TeButtonLayout, unsigned int>(this, &TeButtonLayout::onMousePositionChangedMaxPriority, FLT_MAX));

	_onMousePositionChangedCallback.reset(new TeCallback1Param<TeButtonLayout, unsigned int>(this, &TeButtonLayout::onMousePositionChanged));
	_onMouseLeftDownCallback.reset(new TeCallback1Param<TeButtonLayout, unsigned int>(this, &TeButtonLayout::onMouseLeftDown));
	_onMouseLeftUpMaxPriorityCallback.reset(new TeCallback1Param<TeButtonLayout, unsigned int>(this, &TeButtonLayout::onMouseLeftUpMaxPriority, FLT_MAX));
	_onMouseLeftUpCallback.reset(new TeCallback1Param<TeButtonLayout, unsigned int>(this, &TeButtonLayout::onMouseLeftUp));

	setEditionColor(TeColor(128, 128, 128, 255));
	if (getDoubleValidationProtectionTimer()->_stopped)
		getDoubleValidationProtectionTimer()->start();
}

bool TeButtonLayout::isMouseIn(const TeVector2s32 &mouseloc) {
	if (!_hitZoneLayout) {
		return TeLayout::isMouseIn(mouseloc);
	} else {
		return _hitZoneLayout->isMouseIn(mouseloc);
	}
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
	State newState = (_currentState == BUTTON_STATE_DISABLED ? BUTTON_STATE_DISABLED : BUTTON_STATE_UP);
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
	error("TODO: Implement TeButtonLayout::timeFromLastValidation.");
}

void TeButtonLayout::setDisabledLayout(TeLayout *disabledLayout) {
	if (_disabledLayout)
		removeChild(_disabledLayout);
	
	_disabledLayout = disabledLayout;
	if (_disabledLayout) {
		addChild(_disabledLayout);
		_disabledLayout->setColor(TeColor(0, 0, 0, 0));
	}
	
	setState(_currentState);
}

void TeButtonLayout::setHitZone(TeLayout *hitZoneLayout) {
	if (_hitZoneLayout)
		removeChild(_hitZoneLayout);
	
	_hitZoneLayout = hitZoneLayout;
	if (_hitZoneLayout) {
		addChild(_hitZoneLayout);
		_hitZoneLayout->setColor(TeColor(0, 0, 0xff, 0xff));
	}
}

void TeButtonLayout::setDownLayout(TeLayout *downLayout) {
	if (_downLayout)
		removeChild(_downLayout);
	
	if (downLayout)
		addChild(downLayout);
	_downLayout = downLayout;
	
	if (sizeType() == RELATIVE_TO_PARENT &&
			size().x() == 1.0f && size().y() == 1.0f &&
			!_upLayout && _downLayout) {
		setSize(_downLayout->size());
	}

	if (_downLayout)
		_downLayout->setColor(TeColor(0, 0, 0, 0));
	
	setState(_currentState);
}

void TeButtonLayout::setRollOverLayout(TeLayout *rollOverLayout) {
	if (_rolloverLayout)
		removeChild(_rolloverLayout);
	
	_rolloverLayout = rollOverLayout;
	if (_rolloverLayout) {
		addChild(_rolloverLayout);
		_rolloverLayout->setColor(TeColor(0, 0, 0, 0));
	}
	
	setState(_currentState);
}

void TeButtonLayout::setUpLayout(TeLayout *upLayout) {
	if (_upLayout)
		removeChild(_upLayout);
	
	if (upLayout)
		addChild(upLayout);
	_upLayout = upLayout;

	if (sizeType() == RELATIVE_TO_PARENT &&
			size().x() == 1.0f && size().y() == 1.0f &&
			!_downLayout && _upLayout) {
		setSize(_upLayout->size());
	}
	
	if (_upLayout)
		_upLayout->setColor(TeColor(0, 0, 0, 0));
	
	setState(_currentState);
}

void TeButtonLayout::setDoubleValidationProtectionEnabled(bool enable) {
	_doubleValidationProtectionEnabled = enable;
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
	if (_currentState != BUTTON_STATE_DISABLED) {
		int somethingCount = 0;
		if (!_intArr.empty()) {
			error("TODO: Implement setPosition logic for up/down state");
		}
		if (_someClickFlag) {
			setState(somethingCount ? BUTTON_STATE_DOWN : BUTTON_STATE_UP);
		}
	}
}

void TeButtonLayout::setState(State newState) {
	if (_currentState != newState) {
		switch (newState) {
		case BUTTON_STATE_UP:
			_onButtonChangedToStateUpSignal.call();
			break;
		case BUTTON_STATE_DOWN:
			_onButtonChangedToStateDownSignal.call();
			break;
		case BUTTON_STATE_ROLLOVER:
			_onButtonChangedToStateRolloverSignal.call();
			break;
		default:
			break;
		}
		_currentState = newState;
	}
	
	if (_upLayout)
		_upLayout->setVisible(_currentState == BUTTON_STATE_UP);
	if (_downLayout)
		_downLayout->setVisible(_currentState == BUTTON_STATE_DOWN);
	if (_disabledLayout)
		_disabledLayout->setVisible(_currentState == BUTTON_STATE_DISABLED);
	if (_rolloverLayout)
		_rolloverLayout->setVisible(_currentState == BUTTON_STATE_ROLLOVER);
}

} // end namespace Syberia
