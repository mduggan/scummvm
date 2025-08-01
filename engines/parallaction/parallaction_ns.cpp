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

#include "common/system.h"
#include "common/config-manager.h"
#include "common/textconsole.h"

#include "parallaction/parallaction.h"
#include "parallaction/exec.h"
#include "parallaction/input.h"
#include "parallaction/parser.h"
#include "parallaction/saveload.h"
#include "parallaction/sound.h"
#include "parallaction/walk.h"


namespace Parallaction {

#ifdef USE_TTS

static const char *openingCreditsFirstLine[] = {
	"Programmatore Assoluto: Paolo Costabel",	// Italian
	"Programmeur Absolu: Paolo Costabel",		// French
	"Absolute Programmer: Paolo Costabel",		// English
	"Absoluter Programmierer: Paolo Costabel"	// German
};

// Transcribed for TTS
// First four names of the end credits are best voiced as one block; the remaining credits
// are voiced one at a time elsewhere
static const char *endCreditsFirstSection[] = {
	// Italian
	"Il Dottor Buoz\n"
	"Lo Studente\n"
	"Il Poliziotto\n"
	"Secondo il Secondino",
	// French
	"Le Professeur Buoz\n"
	"Le \311tudiant\n"
	"Le Police\n"
	"Secondo le Garde",
	// English
	"Doctor Buoz\n"
	"Student\n"
	"Police\n"
	"Secondo the Warder",
	// German
	"Professor Buoz\n"
	"Der Student\n"
	"Die Polizei\n"
	"Secondo der W\344chter"
};

#endif

#define INITIAL_FREE_SARCOPHAGUS_SLOT_X	200


class LocationName {

	Common::String _slide;
	Common::String _character;
	Common::String _location;

	bool _hasCharacter;
	bool _hasSlide;
	Common::String _buf;

public:
	LocationName() {
		_hasSlide = false;
		_hasCharacter = false;
	}

	void bind(const char*);

	const char *location() const {
		return _location.c_str();
	}

	bool hasCharacter() const {
		return _hasCharacter;
	}

	const char *character() const {
		return _character.c_str();
	}

	bool hasSlide() const {
		return _hasSlide;
	}

	const char *slide() const {
		return _slide.c_str();
	}

	const char *c_str() const {
		return _buf.c_str();
	}
};



/*
	bind accept the following input formats:

	1 - [S].slide.[L]{.[C]}
	2 - [L]{.[C]}

	where:

	[S] is the slide to be shown
	[L] is the location to switch to (immediately in case 2, or right after slide [S] in case 1)
	[C] is the character to be selected, and is optional

	The routine tells one form from the other by searching for the '.slide.'

	NOTE: there exists one script in which [L] is not used in the case 1, but its use
	is commented out, and would definitely crash the current implementation.
*/
void LocationName::bind(const char *s) {
	_buf = s;
	_hasSlide = false;
	_hasCharacter = false;

	Common::StringArray list;
	char *tok = strtok(_buf.begin(), ".");
	while (tok) {
		list.push_back(tok);
		tok = strtok(nullptr, ".");
	}

	if (list.size() < 1 || list.size() > 4)
		error("changeLocation: ill-formed location name '%s'", s);

	if (list.size() > 1) {
		if (list[1] == "slide") {
			_hasSlide = true;
			_slide = list[0];

			list.remove_at(0);		// removes slide name
			list.remove_at(0);		// removes 'slide'
		}

		if (list.size() == 2) {
			_hasCharacter = true;
			_character = list[1];
		}
	}

	_location = list[0];
	_buf = s;		// kept as reference
}

Parallaction_ns::Parallaction_ns(OSystem* syst, const PARALLACTIONGameDescription *gameDesc) : Parallaction(syst, gameDesc),
	_locationParser(nullptr), _programParser(nullptr), _walker(nullptr) {
	_soundManI = nullptr;
	_score = 0;
	_inTestResult = 0;
	_callables = nullptr;
	num_foglie = 0;
	_sarcophagusDeltaX = 0;
	_movingSarcophagus = 0;
	_freeSarcophagusSlotX = 0;
	_intro = 0;

	_testResultLabels[0] = nullptr;
	_testResultLabels[1] = nullptr;
}

Common::Error Parallaction_ns::init() {

	_screenWidth = 320;
	_screenHeight = 200;

	if (getPlatform() == Common::kPlatformDOS) {
		_disk = new DosDisk_ns(this);
	} else {
		if (getFeatures() & GF_DEMO) {
			Common::strcpy_s(_location._name, "fognedemo");
		}
		_disk = new AmigaDisk_ns(this);
	}

	_disk->init();

	if (getPlatform() == Common::kPlatformDOS) {
		_soundManI = new DosSoundMan_ns(this);
		_soundManI->setMusicVolume(ConfMan.getInt("music_volume"));
	} else {
		_soundManI = new AmigaSoundMan_ns(this);
	}

	_soundMan = new SoundMan(_soundManI);

	initResources();
	initFonts();
	_locationParser = new LocationParser_ns(this);
	_locationParser->init();
	_programParser = new ProgramParser_ns(this);
	_programParser->init();

	_cmdExec = new CommandExec_ns(this);
	_programExec = new ProgramExec_ns(this);

	_walker = new PathWalker_NS(this);

	_sarcophagusDeltaX = 0;
	_movingSarcophagus = false;
	_freeSarcophagusSlotX = INITIAL_FREE_SARCOPHAGUS_SLOT_X;

	num_foglie = 0;

	_intro = false;
	_inTestResult = false;
	_endCredits = false;

	_location._animations.push_front(_char._ani);

	_saveLoad = new SaveLoad_ns(this, _saveFileMan);

	initInventory();
	setupBalloonManager();

	_score = 1;

	_testResultLabels[0] = nullptr;
	_testResultLabels[1] = nullptr;

	Parallaction::init();

	return Common::kNoError;
}

Parallaction_ns::~Parallaction_ns() {
	freeFonts();

	// TODO: we may want to add a ~Character instead
	freeCharacter();
	_char._ani.reset();

	destroyInventory();

	delete _locationParser;
	delete _programParser;
	freeLocation(true);

	_location._animations.remove(_char._ani);

	delete _walker;

	destroyTestResultLabels();
}

void Parallaction_ns::destroyTestResultLabels() {
	for (int i = 0; i < 2; ++i) {
		_gfx->unregisterLabel(_testResultLabels[i]);
		delete _testResultLabels[i];
		_testResultLabels[i] = nullptr;
	}
}

void Parallaction_ns::freeFonts() {

	delete _dialogueFont;
	delete _labelFont;
	delete _menuFont;
	delete _introFont;

	_menuFont  = nullptr;
	_dialogueFont = nullptr;
	_labelFont = nullptr;
	_introFont = nullptr;
}


void Parallaction_ns::callFunction(uint index, void* parm) {
	assert(index < 25);	// magic value 25 is maximum # of callables for Nippon Safes

	(this->*_callables[index])(parm);
}

bool Parallaction_ns::processGameEvent(int event) {
	if (event == kEvNone) {
		return true;
	}

	bool c = true;
	_input->stopHovering();

	switch (event) {
	case kEvSaveGame:
		_saveLoad->saveGame();
		break;

	case kEvLoadGame:
		_saveLoad->loadGame();
		break;

	default:
		break;
	}

	_input->setArrowCursor();
	_input->setMouseState(MOUSE_ENABLED_SHOW);

	return c;
}

Common::Error Parallaction_ns::go() {
	_saveLoad->renameOldSavefiles();

	_globalFlagsNames = _disk->loadTable("global");

	startGui();

	while (!shouldQuit()) {
		runGame();
	}

	return Common::kNoError;
}

void Parallaction_ns::changeBackground(const char* background, const char* mask, const char* path) {
	Palette pal;

	uint16 v2 = 0;
	if (!scumm_stricmp(background, "final")) {
		_gfx->clearScreen();
		for (uint16 _si = 0; _si < 32; _si++) {
			pal.setEntry(_si, v2, v2, v2);
			v2 += 4;
		}

		_system->delayMillis(20);
		_gfx->setPalette(pal);
		_gfx->updateScreen();
		return;
	}

	if (path == nullptr) {
		path = mask;
	}

	BackgroundInfo *info = new BackgroundInfo;
	_disk->loadScenery(*info, background, mask, path);
	_gfx->setBackground(kBackgroundLocation, info);
}


void Parallaction_ns::runPendingZones() {
	if (_activeZone) {
		ZonePtr z = _activeZone;	// speak Zone or sound
		_activeZone.reset();
		runZone(z);
	}
}

//	changeLocation handles transitions between locations, and is able to display slides
//	between one and the other.
//
void Parallaction_ns::changeLocation() {
	if (_newLocationName.empty()) {
		return;
	}

	char location[200];
	Common::strlcpy(location, _newLocationName.c_str(), 200);
	Common::strlcpy(_location._name, _newLocationName.c_str(), 100);

	debugC(1, kDebugExec, "changeLocation(%s)", location);

	MouseTriState oldMouseState = _input->getMouseState();
	_input->setMouseState(MOUSE_DISABLED);

	if (!_intro) {
		// prevent music changes during the introduction
		_soundManI->playLocationMusic(location);
	}

	_input->stopHovering();
	// this is still needed to remove the floatingLabel
	_gfx->freeLabels();

	_zoneTrap.reset();

	_input->setArrowCursor();

	_gfx->showGfxObj(_char._ani->gfxobj, false);

	LocationName locname;
	locname.bind(location);

	freeLocation(false);

	if (locname.hasSlide()) {
		showSlide(locname.slide());
		GfxObj *label = _gfx->createLabel(_menuFont, _location._slideText[0].c_str(), 1);
		_gfx->showLabel(label, CENTER_LABEL_HORIZONTAL, 14);
		_gfx->updateScreen();

		_input->waitForButtonEvent(kMouseLeftUp);
		_gfx->unregisterLabel(label);
		delete label;

		if (!locname.hasCharacter()) {
			setTTSVoice(_characterVoiceID);
		}
	}

	if (locname.hasCharacter()) {
		changeCharacter(locname.character());
	}

	Common::strlcpy(g_saveData1, locname.location(), 30);
	parseLocation(g_saveData1);

	if (_location._startPosition.x != -1000) {
		_char._ani->setX(_location._startPosition.x);
		_char._ani->setY(_location._startPosition.y);
		_char._ani->setF(_location._startFrame);
		_location._startPosition.y = -1000;
		_location._startPosition.x = -1000;
	}


	_gfx->setBlackPalette();
	_gfx->updateScreen();

	// BUG #3459: kEngineChangeLocation flag must be cleared *before* commands
	// and acommands are executed, so that it can be set again if needed.
	g_engineFlags &= ~kEngineChangeLocation;

	_cmdExec->run(_location._commands);

	doLocationEnterTransition();

	_cmdExec->run(_location._aCommands);

	if (_location._hasSound)
		_soundManI->playSfx(_location._soundFile, 0, true);

	if (!_intro) {
		_input->setMouseState(oldMouseState);
		// WORKAROUND: Fix a script bug in the Multilingual DOS version of
		// Nippon Safes: the mouse cursor is incorrectly hidden outside the
		// cave at the end of the game. Fix it here.
		if (!strcmp(_location._name, "ingressocav"))
			_input->setMouseState(MOUSE_ENABLED_SHOW);
#ifdef USE_TTS
		else if (!strcmp(_location._name, "final")) {	// End credits
			setTTSVoice(kNarratorVoiceID);
			sayText(endCreditsFirstSection[getInternLanguage()], Common::TextToSpeechManager::INTERRUPT);
			_endCredits = true;
		}
#endif
	}
	
#ifdef USE_TTS
	if (!strcmp(_location._name, "title1")) {		// First screen of opening credits
		setTTSVoice(kNarratorVoiceID);
		sayText(openingCreditsFirstLine[getInternLanguage()], Common::TextToSpeechManager::INTERRUPT);
	}
#endif

	debugC(1, kDebugExec, "changeLocation() done");
	_newLocationName.clear();
}


void Parallaction_ns::parseLocation(const char *filename) {
	debugC(1, kDebugParser, "parseLocation('%s')", filename);

	allocateLocationSlot(filename);
	Script *script = _disk->loadLocation(filename);

	// TODO: the following two lines are specific to Nippon Safes
	// and should be moved into something like 'initializeParsing()'
	_location._hasSound = false;

	_locationParser->parse(script);

	delete script;

	// this loads animation scripts
	AnimationList::iterator it = _location._animations.begin();
	for ( ; it != _location._animations.end(); ++it) {
		if (!(*it)->_scriptName.empty()) {
			loadProgram(*it, (*it)->_scriptName.c_str());
		}
	}

	debugC(1, kDebugParser, "parseLocation('%s') done", filename);
	return;
}



void Parallaction_ns::changeCharacter(const char *name) {
	debugC(1, kDebugExec, "changeCharacter(%s)", name);

	_char.setName(name);

	if (!scumm_stricmp(_char.getFullName(), _characterName1)) {
		debugC(3, kDebugExec, "changeCharacter: nothing done");
		return;
	}

	freeCharacter();

	_char._ani->gfxobj = _gfx->loadCharacterAnim(_char.getFullName());

	if (scumm_stricmp(_char.getBaseName(), g_doughName) == 0) {
		_characterVoiceID = kDoug;
	} else if (scumm_stricmp(_char.getBaseName(), g_donnaName) == 0) {
		_characterVoiceID = kDonna;
	} else if (scumm_stricmp(_char.getBaseName(), g_dinoName) == 0) {
		_characterVoiceID = kDino;
	} else {
		_characterVoiceID = kDrki;
	}

	setTTSVoice(_characterVoiceID);

	if (!_char.dummy()) {
		_char._head = _disk->loadHead(_char.getBaseName());
		_char._talk = _disk->loadTalk(_char.getBaseName());
		_objects = _disk->loadObjects(_char.getBaseName());
		_objectsNames = _disk->loadTable(_char.getBaseName());

		if (!_intro) {
			// prevent music changes during the introduction
			_soundManI->playCharacterMusic(_char.getBaseName());
		}

		// The original engine used to reload 'common' only on loadgames. We are reloading here since 'common'
		// contains character specific stuff. This causes crashes like bug #3440, because parseLocation tries
		// to reload scripts but the data archive selected is occasionally wrong. This has been solved by having
		// parseLocation only load scripts when they aren't already loaded - which it should have done since the
		// beginning nevertheless.
		if (!(getFeatures() & GF_DEMO))
			parseLocation("common");
	}

	Common::strcpy_s(_characterName1, _char.getFullName());

	debugC(3, kDebugExec, "changeCharacter: switch completed");

	return;
}

void Parallaction_ns::freeCharacter() {
	_gfx->freeCharacterObjects();

	delete _char._talk;
	delete _char._head;
	delete _char._ani->gfxobj;
	delete _objects;
	delete _objectsNames;

	_char._talk = nullptr;
	_char._head = nullptr;
	_char._ani->gfxobj = nullptr;

	_objects = nullptr;
	_objectsNames = nullptr;
}

void Parallaction_ns::freeLocation(bool removeAll) {
	debugC(2, kDebugExec, "freeLocation");

	_soundManI->stopSfx(0);
	_soundManI->stopSfx(1);
	_soundManI->stopSfx(2);
	_soundManI->stopSfx(3);

	_localFlagNames->clear();

	_gfx->freeLocationObjects();

	_location._animations.remove(_char._ani);
	_location.cleanup(removeAll);
	_location._animations.push_front(_char._ani);
}

void Parallaction_ns::cleanupGame() {
	_soundManI->stopMusic();

	_inTestResult = false;
	g_engineFlags &= ~kEngineTransformedDonna;

	_numLocations = 0;
	g_globalFlags = 0;
	memset(_localFlags, 0, sizeof(_localFlags));
	memset(_locationNames, 0, sizeof(_locationNames));

	_location.freeZones(true);

	_score = 0;
	_freeSarcophagusSlotX = INITIAL_FREE_SARCOPHAGUS_SLOT_X;
	_movingSarcophagus = false;
}

void Parallaction_ns::updateWalkers() {
	_walker->walk();
}


void Parallaction_ns::scheduleWalk(int16 x, int16 y, bool fromUser) {
	AnimationPtr a = _char._ani;

	if ((a->_flags & kFlagsRemove) || (a->_flags & kFlagsActive) == 0) {
		return;
	}

	_walker->buildPath(a, x, y);
	g_engineFlags |= kEngineWalking;
}

}// namespace Parallaction
