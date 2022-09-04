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

#ifndef SYBERIA_TE_TE_ANIMATION_H
#define SYBERIA_TE_TE_ANIMATION_H

#include "common/array.h"
#include "engines/syberia/te/te_timer.h"
#include "engines/syberia/te/te_signal.h"

namespace Syberia {

class TeAnimation {
public:
	TeAnimation();

	void cont();
	void pause();
	void stop();
	void reset();
	void play() {
		cont();
	}
	virtual void update(double time) = 0;
	
	void seekToStart();
	//void staticDestroy();

	void pauseAll();
	void resumeAll();
	void updateAll();

	TeTimer _runTimer;
	
	TeSignal0Param &onStop() { return _onStopSignal; }
	TeSignal0Param &onFinished() { return _onFinishedSignal; }

private:

	void removeThisFromAnimations();

	TeSignal0Param _onStopSignal;
	TeSignal0Param _onFinishedSignal;

	static Common::Array<TeAnimation *> _animations;

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_ANIMATION_H
