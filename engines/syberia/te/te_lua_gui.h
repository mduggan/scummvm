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

#ifndef SYBERIA_TE_TE_LUA_GUI_H
#define SYBERIA_TE_TE_LUA_GUI_H

#include "common/str.h"
#include "common/hashmap.h"
#include "common/hash-str.h"
#include "syberia/te/te_button_layout.h"
#include "syberia/te/te_checkbox_layout.h"
#include "syberia/te/te_clip_layout.h"
#include "syberia/te/te_color.h"
#include "syberia/te/te_curve_anim2.h"
#include "syberia/te/te_extended_text_layout.h"
#include "syberia/te/te_i_3d_object2.h"
#include "syberia/te/te_i_layout.h"
#include "syberia/te/te_layout.h"
#include "syberia/te/te_list_layout.h"
#include "syberia/te/te_lua_context.h"
#include "syberia/te/te_lua_script.h"
#include "syberia/te/te_object.h"
#include "syberia/te/te_quaternion.h"
#include "syberia/te/te_scrolling_layout.h"
#include "syberia/te/te_sprite_layout.h"
#include "syberia/te/te_text_layout.h"
#include "syberia/te/te_vector3f32.h"

namespace Syberia {

class TeLuaGUI : public TeObject {
public:
	TeLuaGUI();
	virtual ~TeLuaGUI() {};

	virtual void enter() {};
	virtual void leave() {};

	TeButtonLayout *buttonLayout(const Common::String &name);
	TeCheckboxLayout *checkboxLayout(const Common::String &name);
	TeClipLayout *clipLayout(const Common::String &name);
	TeCurveAnim2<TeI3DObject2, TeColor> *colorLinearAnimation(const Common::String &name);
	TeExtendedTextLayout *extendedTextLayout(const Common::String &name);
	TeLayout *layout(const Common::String &name);
	TeCurveAnim2<TeILayout, TeVector3f32> *layoutAnchorLinearAnimation(const Common::String &name);
	TeCurveAnim2<TeI3DObject2, TeVector3f32> *layoutPositionLinearAnimation(const Common::String &name);
	TeListLayout *listLayout(const Common::String &name);
	TeCurveAnim2<TeI3DObject2, TeQuaternion> *rotationLinearAnimation(const Common::String &name);
	TeScrollingLayout *scrollingLayout(const Common::String &name);
	TeSpriteLayout *spriteLayout(const Common::String &name);
	TeTextLayout *textLayout(const Common::String &name);

	bool load(const Common::String &path);
	void unload();

	Common::String value(const Common::String &path);

	template <typename T> using StringMap = Common::HashMap<Common::String, T>;

	StringMap<TeLayout *> &layouts() { return _layouts; };

private:
	TeLuaContext _luaContext;
	TeLuaScript _luaScript;

	bool _loaded;
	Common::String _scriptPath;

	StringMap<TeLayout *> _layouts;
	StringMap<TeButtonLayout *> _buttonLayouts;
	StringMap<TeCheckboxLayout *> _checkboxLayouts;
	StringMap<TeListLayout *> _listLayouts;
	StringMap<TeSpriteLayout *> _spriteLayouts;
	StringMap<TeTextLayout *> _textLayouts;
	StringMap<TeScrollingLayout *> _scrollingLayouts;
	StringMap<TeClipLayout *> _clipLayouts;
	StringMap<TeExtendedTextLayout *> _extendedTextLayouts;

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_LUA_GUI_H
