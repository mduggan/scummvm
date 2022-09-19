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

#include "common/str.h"
#include "common/hashmap.h"

#include "syberia/te/te_color.h"
#include "syberia/te/te_font3.h"
#include "syberia/te/te_intrusive_ptr.h"
#include "syberia/te/te_mesh.h"
#include "syberia/te/te_vector2s32.h"

namespace Syberia {

class TeTextBase2 {
public:
	TeTextBase2();
	
	class Line {};
	enum AlignStyle {
		AlignStyle0
	};

	enum WrapMode {
		WrapModeFixed,
		WrapModeProportional
	};

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
	void drawLine(unsigned int i, unsigned int j, const TeVector3f32 &pt, Line &line);
	unsigned int endOfWord(unsigned int i);
	void insertNewLine(unsigned int offset);
	bool isASpace(unsigned int offset) const;
	int newLines(unsigned int offset);
	int nextNonSpaceChar(unsigned int start);
	void setAlignStyle(AlignStyle style);
	void setColor(unsigned int i, const TeColor &color);
	void setFont(unsigned int i, const TeIntrusivePtr<TeFont3> &newfont);
	void setFontSize(unsigned long size);
	void setGlobalColor(const TeColor &color);
	void setInterLine(float val);
	void setRect(const TeVector2s32 &rect);
	void setText(const Common::String &newText);
	void setWrapMode(WrapMode &mode);
	TeVector2s32 size();
	void strikethrough(bool val);
	bool strikethrough() const { return _strikethrough; }
	const Common::String &text() const { return _text; }
	WrapMode wrapMode() const { return _wrapMode; }
	const TeVector2s32 &size() const { return _size; }

private:
	AlignStyle _alignStyle;
	WrapMode _wrapMode;
	unsigned long _fontSize;
	bool _valueWasSet;
	TeColor _globalColor;
	float _interLine;
	TeVector2s32 _drawRect;
	TeVector2s32 _size;
	Common::String _text;
	bool _strikethrough;

	TeMesh _mesh;

	Common::Array<unsigned int> _lineBreaks;
	Common::HashMap<unsigned int, TeColor> _colors;
	Common::HashMap<unsigned int, TeIntrusivePtr<TeFont3>> _fonts;
};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_TEXT_BASE2_H
