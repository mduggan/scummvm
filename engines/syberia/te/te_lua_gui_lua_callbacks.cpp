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

#include "common/lua/lua.h"
#include "common/textconsole.h"
#include "syberia/te/te_lua_gui_lua_callbacks.h"
#include "syberia/te/te_layout.h"
#include "syberia/te/te_lua_gui.h"
#include "syberia/te/te_3d_object2.h"
#include "syberia/te/te_object.h"

namespace Syberia {

template <typename T> static T TeLuaTo(lua_State *L, int index) {
	void *ptr = lua_touserdata(L, index);
	if (!ptr)
		return nullptr;
	T obj = reinterpret_cast<T>(ptr);
	return dynamic_cast<T>(obj);
}

static Common::String TeLuaToTeString(lua_State *L, int index) {
	if (!lua_isstring(L, index)) {
		warning("TeLuaToTeString:: not a string");
		return "";
	} else {
		return lua_tolstring(L, index, nullptr);
	}
}

long TeLuaToS32(lua_State *L, int index) {
	if (!lua_isnumber(L, index)) {
		warning("TeLuaToS32:: not a number");
		return 0;
	} else {
		return lua_tointeger(L, index);
	}
}

static long TeLuaToU32(lua_State *L, int index) {
	if (!lua_isnumber(L, index)) {
		warning("TeLuaToU32:: not a number");
		return 0;
	} else {
		return lua_tointeger(L, index);
	}
}

static float TeLuaToF32(lua_State *L, int index) {
	if (!lua_isnumber(L, index)) {
		warning("TeLuaToF32:: not a number");
		return 0.0f;
	} else {
		return lua_tonumber(L, index);
	}
}

static bool TeLuaToBool(lua_State *L,int index) {
  if (lua_type(L, index) != LUA_TBOOLEAN) {
	  warning("TeLuaToBool:: not a bool");
	  return false;
  } else {
	  return lua_toboolean(L, index);
  }
}


static TeColor TeLuaToTeColor(lua_State *L, int index) {
	TeColor retval(255, 255, 255, 255);
	if (lua_type(L, index) != LUA_TTABLE) {
		warning("TeLuaToTeColor:: not a table");
		return retval;
	}

	lua_pushinteger(L, 1);
	index--;
	lua_gettable(L, index);
	if (lua_isnumber(L, -1)) {
		retval.r() = TeLuaToU32(L, -1);
	}
	lua_settop(L, -2);
	lua_pushinteger(L, 2);
	lua_gettable(L, index);

	if (lua_isnumber(L, -1)) {
		retval.g() = TeLuaToU32(L, -1);
	}

	lua_settop(L, -2);
	lua_pushinteger(L, 3);
	lua_gettable(L,index);
	if (lua_isnumber(L, -1)) {
		retval.b() = TeLuaToU32(L, -1);
	}

	lua_settop(L, -2);
	lua_pushinteger(L, 4);
	lua_gettable(L, index);
	if (lua_isnumber(L, -1)) {
		retval.a() = TeLuaToU32(L, -1);
	}

	lua_settop(L, -2);

	return retval;
}


static TeVector3f32 TeLuaToTeVector3f32(lua_State *L, int index,TeVector3f32 defaultVal) {
	TeVector3f32 retval = defaultVal;
	if (lua_type(L, index) != LUA_TTABLE) {
		warning("TeLuaToTeVector3f32:: not a table");
	} else {
		lua_pushinteger(L, 1);
		index--;
		lua_gettable(L, index);
		if (lua_isnumber(L, -1)) {
		  retval.x() = TeLuaToF32(L, -1);
		}

		lua_settop(L, -2);
		lua_pushinteger(L, 2);
		lua_gettable(L, index);
		if (lua_isnumber(L, -1)) {
		  retval.y() = TeLuaToF32(L, -1);
		}

		lua_settop(L, -2);
		lua_pushinteger(L, 3);
		lua_gettable(L, index);
		if (lua_isnumber(L, -1)) {
		  retval.z() = TeLuaToF32(L, -1);
		}
		lua_settop(L, -2);
	}
	return retval;
}


static bool loadCommonLayoutItems(lua_State *L, const char *s, TeLayout *layout) {
	if (!strcmp(s, "name")) {
		layout->setName(TeLuaToTeString(L, -1));
	} else if (!strcmp(s, "sizeType")) {
		layout->setSizeType(static_cast<TeILayout::CoordinatesType>(TeLuaToS32(L, -1)));
	} else if (!strcmp(s, "size")) {
		TeVector3f32 lastSize = layout->userSize();
		TeVector3f32 size = TeLuaToTeVector3f32(L, -1, lastSize);
		layout->setSize(size);
	} else if (!strcmp(s, "ratio")) {
		layout->setRatio(TeLuaToF32(L, -1));
	} else if (!strcmp(s, "ratioMode")) {
		layout->setRatioMode(static_cast<TeILayout::RatioMode>(TeLuaToS32(L, -1)));
	} else if (!strcmp(s, "safeAreaRatio")) {
		layout->setSafeAreaRatio(TeLuaToF32(L, -1));
	} else if (!strcmp(s, "anchor")) {
		TeVector3f32 lastAnchor = layout->anchor();
		TeVector3f32 anchor = TeLuaToTeVector3f32(L, -1, lastAnchor);
		layout->setAnchor(anchor);
	} else if (!strcmp(s, "positionType")) {
		layout->setPositionType(static_cast<TeILayout::CoordinatesType>(TeLuaToS32(L, -1)));
	} else if (!strcmp(s, "position")) {
		TeVector3f32 lastPos = layout->userPosition();
		TeVector3f32 pos = TeLuaToTeVector3f32(L, -1, lastPos);
		layout->setPosition(pos);
	} else {
		return false;
	}
	return true;
}


// TODO: Fix this.
static bool _g_bWidescreen = false;

int layoutBindings(lua_State *L) {
	if (lua_type(L, -1) != LUA_TTABLE) {
		warning("layoutBindings:: the lua value is not a table\n");
		return 0;
	}

	TeLayout *layout = new TeLayout();
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		int type = lua_type(L, -2);
		if (type == LUA_TSTRING) {
			const char *s = lua_tolstring(L, -2, nullptr);
			if (loadCommonLayoutItems(L, s, layout)) {
				// do nothing.
			} else if (!strcmp(s, "visible")) {
				layout->setVisible(TeLuaToBool(L, -1));
			} else if (!strcmp(s, "color")) {
				layout->setColor(TeLuaToTeColor(L, -1));
			} else if (!strcmp(s, "consoleNoStretch")) {
				warning("TODO: Handle _g_bWidescreen");
				if (_g_bWidescreen) {
					layout->setScale(TeVector3f32(0.7500001f, 1.0f ,1.0f));
				}
			} else {
				warning("[TeLuaGUI.layoutBindings] Unreconized attribute : %s\n", s);
			}
		} else if (type == LUA_TNUMBER) {
			Te3DObject2 *obj = TeLuaTo<Te3DObject2*>(L, -1);
			layout->addChild(obj);
		}
		lua_settop(L, -2);
	}

	if (layout->name().empty()) {
		layout->setName(Common::String::format("%p", (void *)layout));
	}
	lua_pushstring(L, "__TeLuaGUIThis");
    lua_gettable(L, LUA_REGISTRYINDEX);
	TeLuaGUI *gui = TeLuaTo<TeLuaGUI*>(L,-1);
	TeLuaGUI::StringMap<TeLayout *> &layouts = gui->layouts();
	if (!layouts.contains(layout->name())) {
		layouts.setVal(layout->name(), layout);
		lua_pushlightuserdata(L, static_cast<Te3DObject2*>(layout));
		return true;
	} else {
		warning("layoutBindings:: multiple objects with name %s\n", layout->name().c_str());
		delete layout;
		return false;
	}
}

int listLayoutBindings(lua_State *L) {
	if (lua_type(L, -1) != LUA_TTABLE) {
		warning("listLayoutBindings:: the lua value is not a table\n");
		return 0;
	}

	error("TODO: Implement listLayoutBindings.");
}

int spriteLayoutBindings(lua_State *L) {
	if (lua_type(L, -1) != LUA_TTABLE) {
		warning("buttonLayoutBindings:: the lua value is not a table\n");
		return 0;
	}

	lua_pushstring(L, "__TeLuaGUIThis");
	lua_gettable(L, LUA_REGISTRYINDEX);
	TeLuaGUI *gui = TeLuaTo<TeLuaGUI*>(L, -1);
	lua_settop(L, -2);
	TeSpriteLayout *layout = new TeSpriteLayout();
	lua_pushnil(L);
	
	bool playNow = !layout->_tiledSurfacePtr->_frameAnim._runTimer._stopped;
	int startingFrame = 0;
	int endingFrame = -1;
	Common::Path imgFullPath;
	while (lua_next(L, -2) != 0) {
		int type = lua_type(L, -2);
		if (type == LUA_TSTRING) {
			const char *s = lua_tolstring(L, -2, 0);
			if (loadCommonLayoutItems(L, s, layout)) {
				// do nothing.
			} else if (!strcmp(s, "image")) {
				Common::String imgPath = TeLuaToTeString(L, -1);
				if (imgPath.substr(0, 2) == "./") {
					imgPath = imgPath.substr(0, 2);
					imgFullPath = gui->_scriptPath.getParent().join(imgPath);
				} else {
					imgFullPath = imgPath;
				}
			} else if (!strcmp(s, "visible")) {
				layout->setVisible(TeLuaToBool(L, -1));
			} else if (!strcmp(s, "color")) {
				layout->setColor(TeLuaToTeColor(L, -1));
			} else if (!strcmp(s, "leftCropping")) {
				layout->_tiledSurfacePtr->setLeftCropping(TeLuaToF32(L, -1));
			} else if (!strcmp(s, "rightCropping")) {
				layout->_tiledSurfacePtr->setRightCropping(TeLuaToF32(L, -1));
			} else if (!strcmp(s, "topCropping")) {
				layout->_tiledSurfacePtr->setTopCropping(TeLuaToF32(L, -1));
			} else if (!strcmp(s, "bottomCropping")) {
				layout->_tiledSurfacePtr->setBottomCropping(TeLuaToF32(L, -1));
			} else if (!strcmp(s, "loopCount")) {
				layout->_tiledSurfacePtr->_frameAnim._loopCount = TeLuaToS32(L, -1);
			} else if (!strcmp(s, "play")) {
				playNow = TeLuaToBool(L, -1);
			} else if (!strcmp(s, "reversed")) {
				layout->_tiledSurfacePtr->_frameAnim._reversed = TeLuaToBool(L, -1);
			} else if (!strcmp(s, "startingFrame")) {
				startingFrame = TeLuaToU32(L, -1);
			} else if (!strcmp(s, "endingFrame")) {
				endingFrame = TeLuaToU32(L, -1);
			} else if (!strcmp(s, "consoleNoStretch")) {
				warning("TODO: Handle _g_bWidescreen");
				if (_g_bWidescreen) {
					layout->setScale(TeVector3f32(0.7500001,1.0,1.0));
				}
			} else {
				warning("[TeLuaGUI.layoutBindings] Unreconized attribute : %s\n", s);
			}
		}
		lua_settop(L, -2);
	}
	if (!imgFullPath.empty())
		layout->load(imgFullPath);

	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		if (lua_type(L, -2) == 3) {
			Te3DObject2 *object = TeLuaTo<Te3DObject2*>(L, -1);
			layout->addChild(object);
		}
		lua_settop(L, -2);
	}

	if (layout->name().empty()) {
		layout->setName(Common::String::format("%p", (void *)layout));
	}

	if (playNow) {
		layout->play();
	} else {
		layout->stop();
	}
	
	warning("Finish codec bit of spriteLayoutBindings to make movies work.");
	/*
	TeICodec *codec = layout->_tiledSurfacePtr->_codec;
	if (codec) {
	  fVar15 = (float)(*(code *)(pTVar3->vptr).vptr[0x10])();
	  pTVar6 = layout;
	  (((pTVar5->tiledSurfacePtr).ptr)->frameAnim).field_0x70 =
		   (double)(((float)startingFrame / fVar15) * 1000.0) * 1000.0;
	  pTVar4 = (layout->tiledSurfacePtr).ptr;
	  if (endingFrame == 0xffffffffffffffff) {
		(pTVar4->frameAnim).field_0x78 = 0x488f3fffe0c00000;
	  }
	  else {
		fVar15 = (float)(*(code *)(pTVar4->codec->vptr).vptr[0x10])();
		if ((long)endingFrame < 0) {
		  fVar16 = (float)(endingFrame & 1 | endingFrame >> 1);
		  fVar16 = fVar16 + fVar16;
		}
		else {
		  fVar16 = (float)endingFrame;
		}
		(((pTVar6->tiledSurfacePtr).ptr)->frameAnim).field_0x78 =
			 (double)((fVar16 / fVar15) * 1000.0) * 1000.0;
	  }
	}*/
	
	if (!gui->spriteLayout(layout->name())) {
		TeLuaGUI::StringMap<TeSpriteLayout *> &spriteLayouts = gui->spriteLayouts();
		spriteLayouts.setVal(layout->name(), layout);
		lua_pushlightuserdata(L, static_cast<Te3DObject2*>(layout));
		return true;
	} else {
		warning("layoutBindings:: multiple objects with name %s\n", layout->name().c_str());
		delete layout;
		return false;
	}
}

int buttonLayoutBindings(lua_State *L) {
	if (lua_type(L, -1) != LUA_TTABLE) {
		warning("buttonLayoutBindings:: the lua value is not a table\n");
		return 0;
	}

	TeButtonLayout *layout = new TeButtonLayout();
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		int type = lua_type(L, -2);
		if (type == LUA_TSTRING) {
			const char *s = lua_tolstring(L, -2, nullptr);
			if (loadCommonLayoutItems(L, s, layout)) {
				// do nothing.
			} else if (!strcmp(s, "upLayout")) {
				layout->setUpLayout(TeLuaTo<TeLayout*>(L, -1));
			} else if (!strcmp(s, "downLayout")) {
				layout->setDownLayout(TeLuaTo<TeLayout*>(L, -1));
			} else if (!strcmp(s, "disabledLayout")) {
				layout->setDisabledLayout(TeLuaTo<TeLayout*>(L, -1));
			} else if (!strcmp(s, "rollOverLayout")) {
				layout->setRollOverLayout(TeLuaTo<TeLayout*>(L, -1));
			} else if (!strcmp(s, "hitZone")) {
				layout->setHitZone(TeLuaTo<TeLayout*>(L, -1));
			} else if (!strcmp(s, "enable")) {
				layout->setEnable(TeLuaToBool(L, -1));
			} else if (!strcmp(s, "clickPassThrough")) {
				layout->setClickPassThrough(TeLuaToBool(L, -1));
			} else if (!strcmp(s, "validationSound")) {
				layout->setValidationSound(TeLuaToTeString(L, -1));
			} else if (!strcmp(s, "validationSoundVolume")) {
				layout->setValidationSoundVolume(TeLuaToF32(L, -1));
			} else if (!strcmp(s, "visible")) {
				layout->setVisible(TeLuaToBool(L, -1));
			} else if (!strcmp(s, "color")) {
				layout->setColor(TeLuaToTeColor(L, -1));
			} else if (!strcmp(s, "consoleNoStretch")) {
				warning("TODO: Handle _g_bWidescreen");
				if (_g_bWidescreen) {
					layout->setScale(TeVector3f32(0.7500001,1.0,1.0));
				}
			} else {
				warning("[TeLuaGUI.buttonLayoutBindings] Unreconized attribute : %s\n", s);
			}
		} else if (type == LUA_TNUMBER) {
			Te3DObject2 *obj = TeLuaTo<Te3DObject2*>(L, -1);
			layout->addChild(obj);
		}
		lua_settop(L, -2);
	}

	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		if (lua_type(L, -2) == 3) {
			Te3DObject2 *object = TeLuaTo<Te3DObject2*>(L, -1);
			layout->addChild(object);
		}
		lua_settop(L, -2);
	}

	lua_pushstring(L, "__TeLuaGUIThis");
	lua_gettable(L, LUA_REGISTRYINDEX);
	TeLuaGUI *gui = TeLuaTo<TeLuaGUI*>(L,-1);
	TeLuaGUI::StringMap<TeButtonLayout *> &blayouts = gui->buttonLayouts();
	if (!blayouts.contains(layout->name())) {
		blayouts.setVal(layout->name(), layout);
		lua_pushlightuserdata(L, static_cast<Te3DObject2*>(layout));
		return true;
	} else {
		warning("buttonLayoutBindings:: multiple objects with name %s\n", layout->name().c_str());
		delete layout;
		return false;
	}
}

int checkboxLayoutBindings(lua_State *L) {
	if (lua_type(L, -1) != LUA_TTABLE) {
		warning("checkboxLayoutBindings:: the lua value is not a table\n");
		return 0;
	}

	error("TODO: Implement checkboxLayoutBindings.");
}

int layoutPositionLinearAnimationBindings(lua_State *L) {
	if (lua_type(L, -1) != LUA_TTABLE) {
		warning("layoutPositionLinearAnimationBindings:: the lua value is not a table\n");
		return 0;
	}

	error("TODO: Implement layoutPositionLinearAnimationBindings.");
}

int layoutAnchorLinearAnimationBindings(lua_State *L) {
	if (lua_type(L, -1) != LUA_TTABLE) {
		warning("layoutAnchorLinearAnimationBindings:: the lua value is not a table\n");
		return 0;
	}

	error("TODO: Implement layoutAnchorLinearAnimationBindings.");
}

int textLayoutBindings(lua_State *L) {
	if (lua_type(L, -1) != LUA_TTABLE) {
		warning("textLayoutBindings:: the lua value is not a table\n");
		return 0;
	}

	TeTextLayout *layout = new TeTextLayout();
	lua_pushnil(L);
	while (lua_next(L, -2) != 0) {
		int type = lua_type(L, -2);
		if (type == LUA_TSTRING) {
			const char *s = lua_tolstring(L, -2, nullptr);
			if (loadCommonLayoutItems(L, s, layout)) {
				// do nothing.
			} else if (!strcmp(s, "text")) {
				layout->setText(TeLuaToTeString(L, -1));
			} else if (!strcmp(s, "interLine") || !strcmp(s, "interline")) {
				layout->setInterLine(TeLuaToF32(L, -1));
			} else if (!strcmp(s, "visible")) {
				layout->setVisible(TeLuaToBool(L, -1));
			} else if (!strcmp(s, "color")) {
				layout->setColor(TeLuaToTeColor(L, -1));
			} else if (!strcmp(s, "wrapMode")) {
				layout->setWrapMode(static_cast<TeTextBase2::WrapMode>(TeLuaToS32(L, -1)));
			} else if (!strcmp(s, "textSizeType")) {
				layout->setTextSizeType(TeLuaToS32(L, -1));
			} else if (!strcmp(s, "textSizeProportionalToWidth")) {
				layout->setTextSizeProportionalToWidth(TeLuaToS32(L, -1));
			} else if (!strcmp(s, "consoleNoStretch")) {
				warning("TODO: Handle _g_bWidescreen");
				if (_g_bWidescreen) {
					layout->setScale(TeVector3f32(0.7500001,1.0,1.0));
				}
			} else {
				warning("[TeLuaGUI.textLayoutBindings] Unreconized attribute : %s", s);
			}
			lua_settop(L, -2);
		}
	}

	if (layout->name().empty()) {
		layout->setName(Common::String::format("%p", (void *)layout));
	}
	lua_pushstring(L, "__TeLuaGUIThis");
	lua_gettable(L, LUA_REGISTRYINDEX);
	
	TeLuaGUI *gui = TeLuaTo<TeLuaGUI*>(L,-1);
	TeLuaGUI::StringMap<TeLayout *> &layouts = gui->layouts();
	if (!layouts.contains(layout->name())) {
		layouts.setVal(layout->name(), layout);
		lua_pushlightuserdata(L, static_cast<Te3DObject2*>(layout));
		return true;
	} else {
		warning("textLayoutBindings:: multiple objects with name %s\n", layout->name().c_str());
		delete layout;
		return false;
	}
}

int clipLayoutBindings(lua_State *L) {
	if (lua_type(L, -1) != LUA_TTABLE) {
		warning("clipLayoutBindings:: the lua value is not a table\n");
		return 0;
	}

	error("TODO: Implement clipLayoutBindings.");
}

int colorLinearAnimationBindings(lua_State *L) {
	if (lua_type(L, -1) != LUA_TTABLE) {
		warning("colorLinearAnimationBindings:: the lua value is not a table\n");
		return 0;
	}

	error("TODO: Implement colorLinearAnimationBindings.");
}

int rotationLinearAnimationBindings(lua_State *L) {
	if (lua_type(L, -1) != LUA_TTABLE) {
		warning("rotationLinearAnimationBindings:: the lua value is not a table\n");
		return 0;
	}

	error("TODO: Implement rotationLinearAnimationBindings.");
}

int scrollingLayoutBindings(lua_State *L) {
	if (lua_type(L, -1) != LUA_TTABLE) {
		warning("scrollingLayoutBindings:: the lua value is not a table\n");
		return 0;
	}

	error("TODO: Implement scrollingLayoutBindings.");
}

int extendedTextLayoutBindings(lua_State *L) {
	if (lua_type(L, -1) != LUA_TTABLE) {
		warning("extendedTextLayoutBindings:: the lua value is not a table\n");
		return 0;
	}

	error("TODO: Implement extendedTextLayoutBindings.");
}



} // end namespace Syberia
