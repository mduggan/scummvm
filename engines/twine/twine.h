/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef TWINE_TWINE_H
#define TWINE_TWINE_H

#include "backends/keymapper/keymap.h"
#include "common/random.h"
#include "engines/engine.h"

#include "graphics/managed_surface.h"
#include "graphics/pixelformat.h"
#include "graphics/surface.h"
#include "twine/actor.h"
#include "twine/input.h"
#include "twine/detection.h"

namespace TwinE {

/** Definition for European version */
#define EUROPE_VERSION 0
/** Definition for American version */
#define USA_VERSION 1
/** Definition for Modification version */
#define MODIFICATION_VERSION 2

/** Original screen width */
#define DEFAULT_SCREEN_WIDTH 640
/** Original screen height */
#define DEFAULT_SCREEN_HEIGHT 480
/** Scale screen to double size */
#define SCALE 1
/** Original screen width */
#define SCREEN_WIDTH DEFAULT_SCREEN_WIDTH *SCALE
/** Original screen height */
#define SCREEN_HEIGHT DEFAULT_SCREEN_HEIGHT *SCALE
/** Default frames per second */
#define DEFAULT_FRAMES_PER_SECOND 19

/** Number of colors used in the game */
#define NUMOFCOLORS 256

#define TWINE_PLAY_INTROS 1

static const struct TwinELanguage {
	const char *name;
	const char *id;
} LanguageTypes[] = {
    {"English", "EN_"},
    {"French", "FR_"},
    {"German", "DE_"},
    {"Spanish", "SP_"},
    {"Italian", "IT_"},
    {"Portuguese", ""}};

enum MidiFileType {
	MIDIFILE_NONE,
	MIDIFILE_DOS,
	MIDIFILE_WIN
};

/** Config movie types */
enum MovieType {
	CONF_MOVIE_NONE = 0,
	CONF_MOVIE_FLA = 1,
	CONF_MOVIE_FLAWIDE = 2,
	CONF_MOVIE_FLAPCX = 3
};

// TODO: persist on shutdown
/** Configuration file structure

	Used in the engine to load/use certain parts of code according with
	this settings. Check \a lba.cfg file for valid values for each settings.\n
	All the settings with (*) means they are new and only exist in this engine. */
struct ConfigFile {
	/** Index into the LanguageTypes array. */
	int32 LanguageId = 0;
	bool Voice = true;
	/** Enable/Disable game dialogues */
	bool FlagDisplayText = false;
	/** Type of music file to be used */
	MidiFileType MidiType = MIDIFILE_NONE;
	/** *Game version */
	int32 Version = 0;
	/** If you want to use the LBA CD or not */
	int32 UseCD = 0;
	/** Allow various sound types */
	int32 Sound = 0;
	/** Allow various movie types */
	int32 Movie = CONF_MOVIE_FLA;
	/** Use cross fade effect while changing images, or be as the original */
	int32 CrossFade = 0;
	/** Flag used to keep the game frames per second */
	int32 Fps = 0;
	/** Flag to display game debug */
	bool Debug = false;
	/** Use original autosaving system or save when you want */
	int32 UseAutoSaving = 0;
	/** Shadow mode type, value: all, character only, none */
	int32 ShadowMode = 0;
	/** SceZoom mode type */
	bool SceZoom = false;
	/** Flag to toggle Wall Collision */
	int32 WallCollision = 0;
};

class Actor;
class Animations;
class Collision;
class Extra;
class GameState;
class Grid;
class Movements;
class HQRDepack;
class Interface;
class Menu;
class FlaMovies;
class MenuOptions;
class Music;
class Redraw;
class Renderer;
class Resources;
class Scene;
class Screens;
class ScriptLife;
class ScriptMove;
class Holomap;
class Sound;
class Text;
class DebugGrid;
struct Keyboard;
class Debug;
class DebugScene;

class TwinEEngine : public Engine {
private:
	int32 isTimeFreezed = 0;
	int32 saveFreezedTime = 0;
	ActorMoveStruct loopMovePtr; // mainLoopVar1
	PauseToken _pauseToken;

public:
	TwinEEngine(OSystem *system, Common::Language language, uint32 flags);
	~TwinEEngine() override;

	Common::Error run() override;
	bool hasFeature(EngineFeature f) const override;

	bool isLBA1() const { return _gameFlags & TwineGameType::GType_LBA;};
	bool isLBA2() const { return _gameFlags & TwineGameType::GType_LBA2;};

	Actor *_actor;
	Animations *_animations;
	Collision *_collision;
	Extra *_extra;
	GameState *_gameState;
	Grid *_grid;
	Movements *_movements;
	HQRDepack *_hqrdepack;
	Interface *_interface;
	Menu *_menu;
	FlaMovies *_flaMovies;
	MenuOptions *_menuOptions;
	Music *_music;
	Redraw *_redraw;
	Renderer *_renderer;
	Resources *_resources;
	Scene *_scene;
	Screens *_screens;
	ScriptLife *_scriptLife;
	ScriptMove *_scriptMove;
	Holomap *_holomap;
	Sound *_sound;
	Text *_text;
	DebugGrid *_debugGrid;
	Input *_input;
	Debug *_debug;
	DebugScene *_debugScene;

	/** Configuration file structure
	 * Contains all the data used in the engine to configurated the game in particulary ways. */
	ConfigFile cfgfile;

	/** CD Game directory */
	const char *cdDir = "";

	/** Initialize LBA engine */
	void initEngine();
	void initMCGA();
	void initSVGA();

	void initConfigurations();
	/** Initialize all needed stuffs at first time running engine */
	void initAll();
	void processActorSamplePosition(int32 actorIdx);
	/**
	 * Game engine main loop
	 * @return true if we want to show credit sequence
	 */
	int32 runGameEngine();
	/** Allocate video memory, both front and back buffers */
	void allocVideoMemory();
	int getRandomNumber(uint max = 0x7FFF);
	int32 quitGame = 0;
	int32 lbaTime = 0;

	/** Work video buffer */
	Graphics::ManagedSurface workVideoBuffer;
	/** Main game video buffer */
	Graphics::ManagedSurface frontVideoBuffer;

	/** temporary screen table */
	int32 screenLookupTable[2000]{0};

	int32 loopPressedKey = 0;
	int32 previousLoopPressedKey = 0;
	int32 loopInventoryItem = 0;
	int32 loopActorStep = 0;

	/** Disable screen recenter */
	int16 disableScreenRecenter = 0;

	int32 zoomScreen = 0;

	void freezeTime();
	void unfreezeTime();

	/**
	 * Game engine main loop
	 * @return true if we want to show credit sequence
	 */
	bool gameEngineLoop();

	Common::RandomSource _rnd;
	Common::Language _gameLang;
	uint32 _gameFlags;

	/**
	 * Deplay certain seconds till proceed - Can also Skip this delay
	 * @param time time in seconds to delay
	 */
	void delaySkip(uint32 time);

	/**
	 * Set a new palette in the SDL screen buffer
	 * @param palette palette to set
	 */
	void setPalette(const uint32 *palette);

	/** Blit surface in the screen */
	void flip();

	/**
	 * Blit surface in the screen in a determinate area
	 * @param left left position to start copy
	 * @param top top position to start copy
	 * @param right right position to start copy
	 * @param bottom bottom position to start copy
	 */
	void copyBlockPhys(int32 left, int32 top, int32 right, int32 bottom);

	/** Cross fade feature
	 * @param buffer screen buffer
	 * @param palette new palette to cross fade
	 */
	void crossFade(const Graphics::ManagedSurface &buffer, const uint32 *palette);

	/** Handle keyboard pressed keys */
	void readKeys();

	/**
	 * Display text in screen
	 * @param x X coordinate in screen
	 * @param y Y coordinate in screen
	 * @param string text to display
	 * @param center if the text should be centered accoding with the giving positions
	 */
	void drawText(int32 x, int32 y, const char *string, int32 center);
};

} // namespace TwinE

#endif
