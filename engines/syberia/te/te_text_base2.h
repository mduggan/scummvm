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

#ifndef SYBERIA_TE_TE_TEXT_BASE2_H
#define SYBERIA_TE_TE_TEXT_BASE2_H

#include "syberia/te/te_color.h"
#include "syberia/te/te_font3.h"
#include "syberia/te/te_intrusive_ptr.h"

namespace Syberia {

class TeTextBase2 {
public:
	TeTextBase2();
	
	class Line {};

	void build();
	void clear();
	void clearStyles();
	void clearText();
	
	bool computeLine(unsigned int i, Line &line);
	void computeNbSpaces(Line &line, unsigned int x, unsigned int y);
	TeColor currentColor(unsigned int i);
	TeIntrusivePtr<TeFont3> currentFont(unsigned int i);
	void draw();
	void drawEmptyChar(unsigned int i);
	// TODO add public members

private:
	// TODO add private members

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_TEXT_BASE2_H
