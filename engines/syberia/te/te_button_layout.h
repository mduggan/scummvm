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

#ifndef SYBERIA_TE_TE_BUTTON_LAYOUT_H
#define SYBERIA_TE_TE_BUTTON_LAYOUT_H

#include "syberia/te/te_callback.h"
#include "syberia/te/te_layout.h"
#include "syberia/te/te_signal.h"
#include "syberia/te/te_timer.h"

namespace Syberia {

class TeButtonLayout : public TeLayout {
public:
	TeButtonLayout();

	enum State {
		BUTTON_STATE_DISABLED = 0,
		BUTTON_STATE_1 = 1,
		BUTTON_STATE_UP = 2,
		BUTTON_STATE_3 = 3
	};

	long doubleValidationProtectionTimeoutTime() { return 500; }
	long frozenValidationTimeoutTime() { return 500; }

	virtual bool isMouseIn(const TeVector2s32 &mouseloc) override;
	bool onMouseLeftDown(uint flags);
	bool onMouseLeftDownMaxPriority(uint flags) { return false; }
	bool onMouseLeftUp(uint flags);
	bool onMouseLeftUpMaxPriority(uint flags) { return false; }
	bool onMousePositionChanged(uint flags);
	bool onMousePositionChangedMaxPriority(uint flags) {
		_mousePositionChangedCatched = false;
		return false;
	}

	void reset();

	void resetTimeFromLastValidation();
	long timeFromLastValidation();

	void setDisabledLayout(TeLayout *disabledLayout);
	void setHitZone(TeLayout *hitZoneLayout);
	void setDownLayout(TeLayout *downLayout);
	void setRollOverLayout(TeLayout *rollOverLayout);
	void setUpLayout(TeLayout *upLayout);

	void setDoubleValidationProtectionEnabled(bool enable);
	void setEnable(bool enable);
	virtual void setPosition(const TeVector3f32 &pos) override;

	void setClickPassThrough(bool val) {
		_clickPassThrough = val;
	}
	void setValidationSound(const Common::String &val) {
		_validationSound = val;
	}
	void setValidationSoundVolume(float val) {
		_validationSoundVolume = val;
	}

	void setState(State newState);

	TeSignal0Param &onMouseClickValidated() { return _onMouseClickValidatedSignal; };

private:
	static bool _mousePositionChangedCatched;
	bool _doubleValidationProtectionEnabled;
	static TeTimer *getDoubleValidationProtectionTimer();
	static TeTimer *_doubleValidationProtectionTimer;

	State _currentState;
	bool _clickPassThrough;
	bool _someClickFlag;
	Common::String _validationSound;
	float _validationSoundVolume;

	Common::Array<unsigned int> _intArr;

	TeICallback1ParamPtr<unsigned int> _onMousePositionChangedMaxPriorityCallback;
	TeICallback1ParamPtr<unsigned int> _onMousePositionChangedCallback;
	TeICallback1ParamPtr<unsigned int> _onMouseLeftDownCallback;
	TeICallback1ParamPtr<unsigned int> _onMouseLeftUpMaxPriorityCallback;
	TeICallback1ParamPtr<unsigned int> _onMouseLeftUpCallback;

	TeSignal0Param _onMouseClickValidatedSignal;
	TeSignal0Param _onButtonChangedToDisabledSignal;
	TeSignal0Param _onButtonChangedToState1Signal;
	TeSignal0Param _onButtonChangedToState3Signal;
};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_BUTTON_LAYOUT_H
