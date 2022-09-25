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

#include "syberia/syberia.h"
#include "syberia/detection.h"
#include "syberia/console.h"
#include "common/scummsys.h"
#include "common/config-manager.h"
#include "common/debug-channels.h"
#include "common/events.h"
#include "common/system.h"
#include "engines/util.h"
#include "graphics/palette.h"

#include "syberia/game/game.h"
#include "syberia/game/application.h"
#include "syberia/te/te_core.h"
#include "syberia/te/te_renderer.h"
#include "syberia/te/te_resource_manager.h"
//#include "syberia/te/te_sound_manager.h"
#include "syberia/te/te_input_mgr.h"

#include "graphics/opengl/system_headers.h"

namespace Syberia {

SyberiaEngine *g_engine;

SyberiaEngine::SyberiaEngine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst),
	_gameDescription(gameDesc), _randomSource("Syberia"), _resourceManager(nullptr),
	_core(nullptr),	_application(nullptr), _game(nullptr), _renderer(nullptr),
	_soundManager(nullptr), _inputMgr(nullptr) {
	g_engine = this;
}

SyberiaEngine::~SyberiaEngine() {
	delete _application;
	delete _renderer;
	delete _core;
	delete _game;
	//delete _soundManager;
	delete _resourceManager;
	delete _inputMgr;
}

/*static*/
Common::StringArray SyberiaEngine::splitString (const Common::String &text, char c) {
	Common::StringArray values;

	Common::String str = text;
	size_t pos;
	while ((pos = str.findFirstOf(c)) != Common::String::npos) {
		values.push_back(Common::String(str.c_str(), pos));
		str = Common::String(str.c_str() + pos + 1);
	}

	values.push_back(str);
	return values;
}

Application *SyberiaEngine::getApplication() {
	// created in run()
	return _application;
}

TeCore *SyberiaEngine::getCore() {
	if (_core == nullptr)
		_core = new TeCore();
	return _core;
}

Game *SyberiaEngine::getGame() {
	if (_game == nullptr)
		_game = new Game();
	return _game;
}

TeSoundManager *SyberiaEngine::getSoundManager() {
	/*if (_soundManager == nullptr)
		_soundManager = new TeSoundManager();*/
	return _soundManager;
}

TeRenderer *SyberiaEngine::getRenderer() {
	// created in run()
	return _renderer;
}

TeResourceManager *SyberiaEngine::getResourceManager() {
	if (_resourceManager == nullptr)
		_resourceManager = new TeResourceManager();
	return _resourceManager;
}

TeInputMgr *SyberiaEngine::getInputMgr() {
	if (_inputMgr == nullptr)
		_inputMgr = new TeInputMgr();
	return _inputMgr;
}

uint32 SyberiaEngine::getFeatures() const {
	return _gameDescription->flags;
}

Common::String SyberiaEngine::getGameId() const {
	return _gameDescription->gameId;
}

void SyberiaEngine::configureSearchPaths() {
	const Common::FSNode gameDataDir(ConfMan.get("path"));
	SearchMan.addSubDirectoryMatching(gameDataDir, "Resources", 0, 4);
}

int SyberiaEngine::getDefaultScreenWidth() const {
	return 1024;
}

int SyberiaEngine::getDefaultScreenHeight() const {
	return 768;
}

Common::Error SyberiaEngine::run() {
	initGraphics3d(getDefaultScreenWidth(), getDefaultScreenHeight());

	configureSearchPaths();
	// from BasicOpenGLView::prepareOpenGL..
	_application = new Application();
	_renderer = new TeRenderer();
	_renderer->init();
	_renderer->reset();
	_application->create();

	// Set the engine's debugger console
	setDebugger(new Console());

	// If a savegame was selected from the launcher, load it
	int saveSlot = ConfMan.getInt("save_slot");
	if (saveSlot != -1)
		(void)loadGameState(saveSlot);

	// Simple event handling loop
	Common::Event e;

	while (!shouldQuit()) {
		while (g_system->getEventManager()->pollEvent(e)) {
			getInputMgr()->handleEvent(e);
		}

		_application->run();

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		g_system->delayMillis(10);
	}

	return Common::kNoError;
}

Common::Error SyberiaEngine::syncGame(Common::Serializer &s) {
	// The Serializer has methods isLoading() and isSaving()
	// if you need to specific steps; for example setting
	// an array size after reading it's length, whereas
	// for saving it would write the existing array's length
	int dummy = 0;
	s.syncAsUint32LE(dummy);

	return Common::kNoError;
}

} // namespace Syberia
