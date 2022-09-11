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

#ifndef SYBERIA_TE_TE_CURVE_ANIM2_H
#define SYBERIA_TE_TE_CURVE_ANIM2_H

#include "syberia/te/te_animation.h"
#include "syberia/te/te_interpolation.h"

namespace Syberia {

template<class T> static T linearInterpolation(T &obj1, T &obj2, double amount) {
	return (obj1 * (1.0 - amount)) + (obj2 * amount);
};

template<class T, class S>
class TeCurveAnim2 : public TeAnimation {
public:
	typedef void(T::*TMethod)(const S &);

	TeCurveAnim2() {}
	virtual ~TeCurveAnim2() {}

	void setCurve(const Common::Array<float> &curve) {
		// The original writes the curve to a stream to load it back in in
		// the interpolation.. we just skip that with a direct array copy.
		_interp.load(curve);
	}

	void update(double time) {
		_lastUpdateTime = time;
		
		double amount = _interp.interpole(time, _maxTime);
		
		const S interpVal = linearInterpolation<S>(_firstVal, _secondVal, amount);
		(_callbackObj->*_callbackMethod)(interpVal);
		if (_lastUpdateTime >= _maxTime) {
			if (_repeatCount == -1)
				seekToStart();
			else
				onFinished().call();
		}
	}

	S _firstVal;
	S _secondVal;
	T *_callbackObj;
	TMethod _callbackMethod;
	double _maxTime;

private:
	TeInterpolation _interp;
	double _lastUpdateTime;
};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_CURVE_ANIM2_H
