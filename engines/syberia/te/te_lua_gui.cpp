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

#include "syberia/te/te_lua_gui.h"

namespace Syberia {

TeLuaGUI::TeLuaGUI() {
}

TeButtonLayout *TeLuaGUI::buttonLayout(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeCheckboxLayout *TeLuaGUI::checkboxLayout(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeClipLayout *TeLuaGUI::clipLayout(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeCurveAnim2<TeI3DObject2,TeColor> *TeLuaGUI::colorLinearAnimation(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeExtendedTextLayout *TeLuaGUI::extendedTextLayout(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeLayout *TeLuaGUI::layout(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeCurveAnim2<TeILayout,TeVector3f32> *TeLuaGUI::layoutAnchorLinearAnimation(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeCurveAnim2<TeI3DObject2, TeVector3f32> *TeLuaGUI::layoutPositionLinearAnimation(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeListLayout *TeLuaGUI::listLayout(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeCurveAnim2<TeI3DObject2,TeQuaternion> *TeLuaGUI::rotationLinearAnimation(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeScrollingLayout *TeLuaGUI::scrollingLayout(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeSpriteLayout *TeLuaGUI::spriteLayout(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeTextLayout *TeLuaGUI::textLayout(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

bool TeLuaGUI::load(const Common::String &path) {
	error("TODO: Implement me.");
	return false;
}

void TeLuaGUI::unload() {
	error("TODO: Implement me.");
}

/*static*/ Common::String TeLuaGUI::value(const Common::String &path) {
	error("TODO: Implement me.");
	return nullptr;
}

} // end namespace Syberia
