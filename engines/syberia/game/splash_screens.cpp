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
#include "common/path.h"
#include "syberia/syberia.h"
#include "syberia/game/application.h"
#include "syberia/game/splash_screens.h"

namespace Syberia {

SplashScreens::SplashScreens() {
}

void SplashScreens::enter()	{
	if (!_entered) {
		_entered = true;
		static const Common::Path scriptPath("menus/splashes/splash0.lua");
		if (Common::File::exists(scriptPath)) {
			TeLuaGUI::load(scriptPath.toString());
			Application *app = g_engine->getApplication();
			TeLayout *splash = layout(Common::String("splash"));
			TeI3DObject2 *child = nullptr;
			error("TODO: Implement the rest of SplashScreens::enter");
			if (splash) {
				// call something
				//child = something
			}
			//app->something->addChild(child);
			app->performRender();
		}
		onAlarm();
	}
}

bool SplashScreens::onAlarm() {
	error("TODO: Implement me");
	return false;
}

bool SplashScreens::onQuitSplash() {
	_timer.stop();
	Application *app = g_engine->getApplication();
	app->captureFade();
	TeLuaGUI::unload();
	_entered = false;
	app->mainMenu().enter();
	app->fade();
	return false;
}

} // end namespace Syberia
