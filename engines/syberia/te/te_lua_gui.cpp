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

#include "common/file.h"
#include "syberia/syberia.h"
#include "syberia/te/te_core.h"
#include "syberia/te/te_lua_gui.h"
#include "syberia/te/te_lua_gui_lua_callbacks.h"

namespace Syberia {

TeLuaGUI::TeLuaGUI() {
}

TeButtonLayout *TeLuaGUI::buttonLayout(const Common::String &name) {
	StringMap<TeButtonLayout *>::iterator iter = _buttonLayouts.find(name);
	if (iter != _buttonLayouts.end())
		return iter->_value;
	return nullptr;
}

TeCheckboxLayout *TeLuaGUI::checkboxLayout(const Common::String &name) {
	StringMap<TeCheckboxLayout *>::iterator iter = _checkboxLayouts.find(name);
	if (iter != _checkboxLayouts.end())
		return iter->_value;
	return nullptr;
}

TeClipLayout *TeLuaGUI::clipLayout(const Common::String &name) {
	StringMap<TeClipLayout *>::iterator iter = _clipLayouts.find(name);
	if (iter != _clipLayouts.end())
		return iter->_value;
	return nullptr;
}

TeCurveAnim2<TeI3DObject2, TeColor> *TeLuaGUI::colorLinearAnimation(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeExtendedTextLayout *TeLuaGUI::extendedTextLayout(const Common::String &name) {
	StringMap<TeExtendedTextLayout *>::iterator iter = _extendedTextLayouts.find(name);
	if (iter != _extendedTextLayouts.end())
		return iter->_value;
	return nullptr;
}

TeLayout *TeLuaGUI::layout(const Common::String &name) {
	StringMap<TeLayout *>::iterator iter = _layouts.find(name);
	if (iter != _layouts.end())
		return iter->_value;
	return nullptr;
}

TeCurveAnim2<TeILayout, TeVector3f32> *TeLuaGUI::layoutAnchorLinearAnimation(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeCurveAnim2<TeI3DObject2, TeVector3f32> *TeLuaGUI::layoutPositionLinearAnimation(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeListLayout *TeLuaGUI::listLayout(const Common::String &name) {
	StringMap<TeListLayout *>::iterator iter = _listLayouts.find(name);
	if (iter != _listLayouts.end())
		return iter->_value;
	return nullptr;
}

TeCurveAnim2<TeI3DObject2, TeQuaternion> *TeLuaGUI::rotationLinearAnimation(const Common::String &name) {
	error("TODO: Implement me.");
	return nullptr;
}

TeScrollingLayout *TeLuaGUI::scrollingLayout(const Common::String &name) {
	StringMap<TeScrollingLayout *>::iterator iter = _scrollingLayouts.find(name);
	if (iter != _scrollingLayouts.end())
		return iter->_value;
	return nullptr;
}

TeSpriteLayout *TeLuaGUI::spriteLayout(const Common::String &name) {
	StringMap<TeSpriteLayout *>::iterator iter = _spriteLayouts.find(name);
	if (iter != _spriteLayouts.end())
		return iter->_value;
	return nullptr;
}

TeTextLayout *TeLuaGUI::textLayout(const Common::String &name) {
	StringMap<TeTextLayout *>::iterator iter = _textLayouts.find(name);
	if (iter != _textLayouts.end())
		return iter->_value;
	return nullptr;
}

bool TeLuaGUI::load(const Common::String &pathStr) {
	unload();
	_scriptPath = pathStr;
	TeCore *core = g_engine->getCore();
	Common::Path path(pathStr);
	if (!core->_coreNotReady) {
		if (!Common::File::exists(pathStr)) {
			path = pathStr;
			Common::Path dir = path.getParent();
			dir.joinInPlace(core->language());
			path = dir.join(path.getLastComponent());
			if (!Common::File::exists(pathStr)) {
				path = pathStr;
				dir = path.getParent();
				dir.joinInPlace("en");
				path = dir.join(path.getLastComponent());
			}
		}
	}
	_luaContext.setGlobal("Pixel", 0);
	_luaContext.setGlobal("Percent", 1);
	_luaContext.setGlobal("None", 0);
	_luaContext.setGlobal("LetterBox", 1);
	_luaContext.setGlobal("PanScan", 2);
	_luaContext.setGlobal("MultiLine", 0);
	_luaContext.setGlobal("SingleLine", 1);
	_luaContext.setGlobal("Fixed", 0);
	_luaContext.setGlobal("Proportional", 1);
	_luaContext.registerCFunction("TeLayout", layoutBindings);
	_luaContext.registerCFunction("TeListLayout", listLayoutBindings);
	_luaContext.registerCFunction("TeSpriteLayout", spriteLayoutBindings);
	_luaContext.registerCFunction("TeButtonLayout", buttonLayoutBindings);
	_luaContext.registerCFunction("TeCheckboxLayout", checkboxLayoutBindings);
	_luaContext.registerCFunction("TeLayoutPositionLinearAnimation", layoutPositionLinearAnimationBindings);
	_luaContext.registerCFunction("TeLayoutAnchorLinearAnimation", layoutAnchorLinearAnimationBindings);
	_luaContext.registerCFunction("TeTextLayout", textLayoutBindings);
	_luaContext.registerCFunction("TeClipLayout", clipLayoutBindings);
	_luaContext.registerCFunction("TeColorLinearAnimation", colorLinearAnimationBindings);
	_luaContext.registerCFunction("TeRotationLinearAnimation", rotationLinearAnimationBindings);
	_luaContext.registerCFunction("TeScrollingLayout", scrollingLayoutBindings);
	_luaContext.registerCFunction("TeExtendedTextLayout", extendedTextLayoutBindings);
	_luaContext.setInRegistry("__TeLuaGUIThis",this);
	_luaScript.attachToContext(&_luaContext);
	_luaScript.load(path);
	_luaScript.execute();
	_luaScript.unload();
	_loaded = true;
	return true;
}

void TeLuaGUI::unload() {
	error("TODO: TeLuaGUI::unload Implement me.");
}

TeVariant TeLuaGUI::value(const Common::String &path) {
	return _luaContext.global(path);
}

} // end namespace Syberia
