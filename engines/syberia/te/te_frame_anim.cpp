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

#include "common/util.h"
#include "common/math.h"
#include "syberia/te/te_frame_anim.h"

namespace Syberia {

TeFrameAnim::TeFrameAnim() : _nbFrames(0), _frameRate(25.0), _reversed(false), _minFrame(0),
_numFramesToShow(-1), _startTime(0), _endTime(FLT_MAX) {
}

void TeFrameAnim::update(double amount) {
	int minFrame = MIN((long)_minFrame, _nbFrames);
	int maxFrame = MIN((long)minFrame + _numFramesToShow, _nbFrames);
	double frameNo = _frameRate * amount / 1000.0;

	int loopsDone;
	int framesToPlay = maxFrame - minFrame;
	if (framesToPlay <= 0 && _nbFrames > 0)
		framesToPlay = _nbFrames;
	
	int frameToShow;
	if (framesToPlay == 0) {
		frameToShow = 0;
		loopsDone = -1;
	} else if (framesToPlay > 0) {
		loopsDone = (int)((int)frameNo / framesToPlay);
		if (!_reversed) {
			frameToShow = (int)frameNo % framesToPlay + minFrame;
		} else {
			frameToShow = (maxFrame - 1) - (int)frameNo % framesToPlay;
		}
	} else {
		// else, we don't know the total frames.. just keep asking for higher.
		frameToShow = (int)frameNo;
	}

	if (_loopCount == -1 || loopsDone < _loopCount) {
		if (frameToShow == _lastFrameShown)
			return;

		_lastFrameShown = frameToShow;
		_frameChangedSignal.call();
	} else {
		stop();
		_onFinishedSignal.call();
	}
}

} // end namespace Syberia
