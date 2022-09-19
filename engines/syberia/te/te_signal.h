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

#ifndef SYBERIA_TE_TE_SIGNAL_H
#define SYBERIA_TE_TE_SIGNAL_H

#include "common/array.h"
#include "common/ptr.h"
#include "syberia/te/te_callback.h"

namespace Syberia {

template<class C> int _teCallbackSorter(const C &c1, const C &c2) {
	float p1 = c1->priority();
	float p2 = c2->priority();
	if (p1 < p2)
		return 1;
	else if (p1 == p2)
		return 0;
	return -1;
}

typedef Common::SharedPtr<TeICallback0Param> TeICallback0ParamPtr;

class TeSignal0Param : public Common::SortedArray<TeICallback0ParamPtr, const TeICallback0ParamPtr &> {
public:
	TeSignal0Param() : Common::SortedArray<TeICallback0ParamPtr, const TeICallback0ParamPtr &>(_teCallbackSorter) {};

	void call() {
		typename Common::Array<TeICallback0ParamPtr>::iterator i = this->begin();
		typename Common::Array<TeICallback0ParamPtr>::iterator end_ = this->end();
		for (; i < end_; i++) {
			(*i)->call();
		}
	}

	void remove(const TeICallback0ParamPtr &item) {
		typename Common::Array<TeICallback0ParamPtr>::iterator i = this->begin();
		typename Common::Array<TeICallback0ParamPtr>::iterator end_ = this->end();
		for (; i < end_; i++) {
			if ((*i)->equals(item.get())) {
				this->erase(i);
				break;
			}
		}
	}

	template<class T> void add(T *obj, typename TeCallback0Param<T>::TMethod method) {
		this->insert(TeICallback0ParamPtr(new TeCallback0Param<T>(obj, method)));
	}

	template<class T> void remove(T *obj, typename TeCallback0Param<T>::TMethod method) {
		TeICallback0ParamPtr temp(new TeCallback0Param<T>(obj, method));
		this->remove(temp);
	}

};

template<class T> using TeICallback1ParamPtr = Common::SharedPtr<TeICallback1Param<T>>;

template<class T> class TeSignal1Param : public Common::SortedArray<TeICallback1ParamPtr<T>, const TeICallback1ParamPtr<T> &> {
public:
	TeSignal1Param() : Common::SortedArray<TeICallback1ParamPtr<T>, const TeICallback1ParamPtr<T> &>(_teCallbackSorter) {};

	void call(T t) {
		typename Common::Array<TeICallback1ParamPtr<T>>::iterator i = this->begin();
		typename Common::Array<TeICallback1ParamPtr<T>>::iterator end_ = this->end();
		for (; i < end_; i++) {
			(*i)->call(t);
		}
	}

	void remove(const TeICallback1ParamPtr<T> &item) {
		typename Common::Array<TeICallback1ParamPtr<T>>::iterator i = this->begin();
		typename Common::Array<TeICallback1ParamPtr<T>>::iterator end_ = this->end();
		for (; i < end_; i++) {
			if ((*i)->equals(item.get())) {
				this->erase(i);
				break;
			}
		}
	}

	template<class S> void add(S *obj, typename TeCallback1Param<S, T>::TMethod method) {
		this->insert(TeICallback1ParamPtr<T>(new TeCallback1Param<S, T>(obj, method)));
	}

	template<class S> void remove(S *obj, typename TeCallback1Param<S, T>::TMethod method) {
		TeICallback1ParamPtr<T> temp(new TeCallback1Param<S, T>(obj, method));
		this->remove(temp);
	}
};
} // end namespace Syberia

#endif // SYBERIA_TE_TE_SIGNAL_H
