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

#include "syberia/te/te_text_base2.h"
#include "graphics/opengl/system_headers.h"

namespace Syberia {

TeTextBase2::TeTextBase2() : _drawRect(0, 0), _size(0, 0),
_alignStyle(AlignStyle0), _interLine(0.0f), _globalColor(0xff, 0xff, 0xff, 0xff),
_wrapMode(WrapMode0), _strikethrough(false), _fontSize(10), _valueWasSet(true)
{
	_mesh.setglTexEnv(GL_BLEND);
	_mesh.setShouldDrawMaybe(true);
}

void TeTextBase2::build() {
	warning("TODO: Implement TeTextBase2::build");
}

void TeTextBase2::clear() {
	clearText();
	clearStyles();
}

void TeTextBase2::clearStyles() {
	_lineBreaks.clear();
	_fonts.clear();
	_colors.clear();
}

void TeTextBase2::clearText() {
	_text.clear();
}

bool TeTextBase2::computeLine(unsigned int i, TeTextBase2::Line &line) {
	error("TODO: Implement TeTextBase2::computeLine");

}
void TeTextBase2::computeNbSpaces(Line &line, unsigned int x, unsigned int y) {
	error("TODO: Implement TeTextBase2::computeNbSpaces");
}

TeColor TeTextBase2::currentColor(unsigned int i) {
	error("TODO: Implement TeTextBase2::currentColor");
}

TeIntrusivePtr<TeFont3> TeTextBase2::currentFont(unsigned int i) {
	error("TODO: Implement TeTextBase2::currentFont");
}

void TeTextBase2::draw() {
	error("TODO: Implement TeTextBase2::draw");
}

void TeTextBase2::drawEmptyChar(unsigned int i) {
	error("TODO: Implement TeTextBase2::drawEmptychar");
}

void TeTextBase2::drawLine(unsigned int i, unsigned int j, const TeVector3f32 &pt, TeTextBase2::Line &line) {
	error("TODO: Implement TeTextBase2::drawLine");
}
	
unsigned int TeTextBase2::endOfWord(unsigned int i) {
	error("TODO: Implement TeTextBase2::endOfWord");
}

void TeTextBase2::insertNewLine(unsigned int offset) {
	_lineBreaks.push_back(offset);
}

bool TeTextBase2::isASpace(unsigned int offset) const {
	char c = _text[offset];
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

int TeTextBase2::newLines(unsigned int offset) {
	int result = 0;
	for (unsigned int off : _lineBreaks) {
		if (off == offset)
			result++;
	}
	return result;
}

int TeTextBase2::nextNonSpaceChar(unsigned int offset) {
	while (isASpace(offset))
		offset++;
	return offset; // TODO: or offset - 1?
}

void TeTextBase2::setAlignStyle(TeTextBase2::AlignStyle style) {
	_alignStyle = style;
	_valueWasSet = true;
}

void TeTextBase2::setColor(unsigned int i, const TeColor &color) {
	error("TODO: Implement TeTextBase2::setColor");
}
void TeTextBase2::setFont(unsigned int i, const TeIntrusivePtr<TeFont3> &newfont) {
	error("TODO: Implement TeTextBase2::setFont");
}

void TeTextBase2::setFontSize(unsigned long size) {
	if (_fontSize != size) {
		_fontSize = size;
		_valueWasSet = true;
	}
}

void TeTextBase2::setGlobalColor(const TeColor &color) {
	_globalColor = color;
	_valueWasSet = true;
}

void TeTextBase2::setInterLine(float val) {
	_interLine = val;
	_valueWasSet = true;
}

void TeTextBase2::setRect(const TeVector2s32 &rect) {
	_drawRect = rect;
	_valueWasSet = true;
}

void TeTextBase2::setText(const Common::String &newText) {
	_valueWasSet = true;
	_text = newText;
	int len = newText.size();
	_mesh.setConf(len * 4, len * 6, TeMesh::MeshMode5, 1, len * 2);
}

void TeTextBase2::setWrapMode(TeTextBase2::WrapMode &mode) {
	_wrapMode = mode;
	_valueWasSet = true;
}

TeVector2s32 TeTextBase2::size() {
	if (_valueWasSet)
		build();
	return _size;
}

void TeTextBase2::strikethrough(bool val) {
	if (_strikethrough != val) {
		_strikethrough = val;
		_valueWasSet = true;
	}
}


} // end namespace Syberia
