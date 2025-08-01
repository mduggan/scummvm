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


#include "common/config-manager.h"
#include "common/endian.h"
#include "common/events.h"
#include "common/textconsole.h"
#include "common/translation.h"

#include "graphics/cursorman.h"

#include "gui/saveload.h"

#include "cine/cine.h"
#include "cine/main_loop.h"
#include "cine/object.h"
#include "cine/sound.h"
#include "cine/bg_list.h"
#include "cine/various.h"

namespace Cine {

int16 disableSystemMenu = 0;
bool inMenu;
bool runOnlyUntilFreePartRangeFirst200 = false;

int16 commandVar3[4];
int16 commandVar1;
int16 commandVar2;

//Message messageTable[NUM_MAX_MESSAGE];

uint16 var2;
uint16 var3;
uint16 var4;
uint16 lastType20OverlayBgIdx; // Was var5
uint16 reloadBgPalOnNextFlip;
uint16 forbidBgPalReload;
uint16 gfxFadeOutCompleted;
uint16 gfxFadeInRequested;
uint32 safeControlsLastAccessedMs;
int16 lastSafeControlObjIdx;

int16 buildObjectListCommand(int16 param);
int16 canUseOnObject = 0;

void waitPlayerInput() {
	manageEvents(WAIT_PLAYER_INPUT, UNTIL_MOUSE_BUTTON_DOWN_UP);
}

void setTextWindow(uint16 param1, uint16 param2, uint16 param3, uint16 param4) {
}

uint16 errorVar;
byte menuVar;

uint16 allowPlayerInput;
uint16 checkForPendingDataLoadSwitch;
uint16 isDrawCommandEnabled;
uint16 waitForPlayerClick;
uint16 menuCommandLen;
bool _paletteNeedUpdate;
uint16 _messageLen;

int16 playerCommand;

char currentPrcName[20];
char currentRelName[20];
char currentObjectName[20];
char currentMsgName[20];
char newPrcName[20];
char newRelName[20];
char newObjectName[20];
char newMsgName[20];
char currentCtName[15];
char currentPartName[15];
char currentDatName[30];
uint16 musicIsPlaying;

byte isInPause = 0;

/**
 * Bit masks for mouse buttons' states
 * Bit on = mouse button down
 * Bit off = mouse button up
 */
enum MouseButtonState {
	kLeftMouseButton  = (1 << 0),
	kRightMouseButton = (1 << 1)
};

/** Values used by the xMoveKeyb variable */
enum xMoveKeybEnums {
	kKeybMoveCenterX = 0,
	kKeybMoveRight = 1,
	kKeybMoveLeft = 2
};

/** Values used by the yMoveKeyb variable */
enum yMoveKeybEnums {
	kKeybMoveCenterY = 0,
	kKeybMoveDown = 1,
	kKeybMoveUp = 2
};

uint16 xMoveKeyb = kKeybMoveCenterX;
bool egoMovedWithKeyboard = false;
uint16 yMoveKeyb = kKeybMoveCenterY;

SelectedObjStruct currentSelectedObject;

CommandeType currentSaveName[kMaxSavegames];
static const CommandeType saveChoices[] = {"0-19", "20-39", "40-59", "60-79", "80-99"};

static const int16 choiceResultTable[] = { 1, 1, 1, 2, 1, 1, 1 };
static const int16 subObjectUseTable[] = { 3, 3, 3, 3, 3, 0, 0 };
static const int16 canUseOnItemTable[] = { 1, 0, 0, 1, 1, 0, 0 };

CommandeType objectListCommand[20];
int16 objListTab[20];

/**
 * Move the player character using the keyboard
 * @param x Negative values move left, positive right, zero not at all
 * @param y Negative values move down, positive up, zero not at all
 * NOTE: If both x and y are zero then the character stops
 * FIXME: This seems to only work in Operation Stealth. May need code changes somewhere else...
 */
void moveUsingKeyboard(int x, int y) {
	if (x > 0) {
		xMoveKeyb = kKeybMoveRight;
	} else if (x < 0) {
		xMoveKeyb = kKeybMoveLeft;
	} else {
		xMoveKeyb = kKeybMoveCenterX;
	}

	if (y > 0) {
		yMoveKeyb = kKeybMoveUp;
	} else if (y < 0) {
		yMoveKeyb = kKeybMoveDown;
	} else {
		yMoveKeyb = kKeybMoveCenterY;
	}

	egoMovedWithKeyboard = x || y;
}

void stopMusicAfterFadeOut() {
//	if (g_sfxPlayer->_fadeOutCounter != 0 && g_sfxPlayer->_fadeOutCounter < 100) {
//		g_sfxPlayer->stop();
//	}
}

uint safeControlAccessMinMs() {
	return 250;
}

void runObjectScript(int16 entryIdx) {
	ScriptPtr tmp(g_cine->_scriptInfo->create(*g_cine->_relTable[entryIdx], entryIdx));
	assert(tmp);
	g_cine->_objectScripts.push_back(tmp);
}

/**
 * Add action result message to overlay list
 * @param cmd Message description
 * @todo Why are x, y, width and color left uninitialized?
 */
void addPlayerCommandMessage(int16 cmd) {
	overlay tmp;
	memset(&tmp, 0, sizeof(tmp));
	tmp.objIdx = cmd;
	tmp.type = 3;

	g_cine->_overlayList.push_back(tmp);
}

int16 getRelEntryForObject(uint16 param1, uint16 param2, SelectedObjStruct *pSelectedObject) {
	int16 i;
	int16 found = -1;

	for (i = 0; i < (int16)g_cine->_relTable.size(); i++) {
		if (g_cine->_relTable[i]->_param1 == param1 && g_cine->_relTable[i]->_param2 == pSelectedObject->idx) {
			if (param2 == 1) {
				found = i;
			} else if (param2 == 2) {
				if (g_cine->_relTable[i]->_param3 == pSelectedObject->param) {
					found = i;
				}
			}
		}

		if (found != -1)
			break;
	}

	return found;
}

/**
 * Find index of the object under cursor
 * @param x Mouse cursor coordinate
 * @param y Mouse cursor coordinate
 * @todo Fix displaced type 1 objects
 */
int16 getObjectUnderCursor(uint16 x, uint16 y) {
	Common::List<overlay>::iterator it;

	int16 objX, objY, frame, part, threshold, height, xdif, ydif;
	int width;

	// reverse_iterator would be nice
	for (it = g_cine->_overlayList.reverse_begin(); it != g_cine->_overlayList.end(); --it) {
		if (it->type >= 2 || !g_cine->_objectTable[it->objIdx].name[0]) {
			continue;
		}

		objX = g_cine->_objectTable[it->objIdx].x;
		objY = g_cine->_objectTable[it->objIdx].y;

		frame = ABS((int16)(g_cine->_objectTable[it->objIdx].frame));
		part = g_cine->_objectTable[it->objIdx].part;

		// Additional case for negative frame values in Operation Stealth
		if (g_cine->getGameType() == Cine::GType_OS && g_cine->_objectTable[it->objIdx].frame < 0) {
			if ((it->type == 1) && (x >= objX) && (objX + frame >= x) && (y >= objY) && (objY + part >= y)) {
				return it->objIdx;
			} else {
				continue;
			}
		}

		if (it->type == 0) {
			threshold = g_cine->_animDataTable[frame]._var1;
		} else {
			threshold = g_cine->_animDataTable[frame]._width / 2;
		}

		height = g_cine->_animDataTable[frame]._height;
		width = g_cine->_animDataTable[frame]._realWidth;

		xdif = x - objX;
		ydif = y - objY;

		if ((xdif < 0) || ((threshold << 4) <= xdif) || (ydif <= 0) || (ydif >= height) || !g_cine->_animDataTable[frame].data()) {
			continue;
		}

		if (g_cine->getGameType() == Cine::GType_OS) {
			// This test isn't present in Operation Stealth's PC version's disassembly
			// but removing it makes things crash sometimes (e.g. when selecting a verb
			// and moving the mouse cursor around the floor in the airport's bathroom).
			if (xdif >= width) {
				continue;
			}

			if (it->type == 0 && g_cine->_animDataTable[frame].getColor(xdif, ydif) != (part & 0x0F)) {
				return it->objIdx;
			} else if (it->type == 1 && gfxGetBit(xdif, ydif, g_cine->_animDataTable[frame].data(), g_cine->_animDataTable[frame]._width * 4)) {
				return it->objIdx;
			}
		} else if (it->type == 0) { // use generated mask
			if (gfxGetBit(xdif, ydif, g_cine->_animDataTable[frame].mask(), g_cine->_animDataTable[frame]._width)) {
				return it->objIdx;
			}
		} else if (it->type == 1) { // is mask
			if (gfxGetBit(xdif, ydif, g_cine->_animDataTable[frame].data(), g_cine->_animDataTable[frame]._width * 4)) {
				return it->objIdx;
			}
		}
	}

	return -1;
}

void CineEngine::resetEngine() {
	g_sound->stopMusic();
	freeAnimDataTable();
	g_cine->_overlayList.clear();
	g_cine->_bgIncrustList.clear();
	closePart();

	g_cine->_objectScripts.clear();
	g_cine->_globalScripts.clear();
	g_cine->_relTable.clear();
	g_cine->_scriptTable.clear();
	g_cine->_messageTable.clear();
	resetObjectTable();

	g_cine->_globalVars.reset();

	bgVar0 = 0;
	var2 = var3 = var4 = lastType20OverlayBgIdx = 0;

	newPrcName[0] = '\0';
	newRelName[0] = '\0';
	newObjectName[0] = '\0';
	newMsgName[0] = '\0';
	currentCtName[0] = '\0';

	allowPlayerInput = 0;
	waitForPlayerClick = 0;
	playerCommand = -1;
	isDrawCommandEnabled = 0;

	g_cine->_commandBuffer = "";

	g_cine->_globalVars[VAR_MOUSE_X_POS] = 0;
	g_cine->_globalVars[VAR_MOUSE_Y_POS] = 0;

	renderer->clear();
	currentDisk = 1;

	checkForPendingDataLoadSwitch = 0;

	g_cine->_seqList.clear();
	reloadBgPalOnNextFlip = 0;
	forbidBgPalReload = 0;
	gfxFadeOutCompleted = 0;
	gfxFadeInRequested = 0;
}

int CineEngine::scummVMSaveLoadDialog(bool isSave) {
	GUI::SaveLoadChooser *dialog;
	Common::String desc;
	int slot;

	if (isSave) {
		dialog = new GUI::SaveLoadChooser(_("Save game:"), _("Save"), true);

		slot = dialog->runModalWithCurrentTarget();
		desc = dialog->getResultString();

		if (desc.empty()) {
			// create our own description for the saved game, the user didn't enter it
			desc = dialog->createDefaultSaveDescription(slot);
		}
	}
	else {
		dialog = new GUI::SaveLoadChooser(_("Restore game:"), _("Restore"), false);
		slot = dialog->runModalWithCurrentTarget();
	}

	delete dialog;

	if (slot < 0)
		return true;

	Common::String saveFileName(getSaveStateName(slot));

	if (isSave) {
		Common::String tmp = Common::String::format("%s.dir", _targetName.c_str());

		Common::OutSaveFile *fHandle = _saveFileMan->openForSaving(tmp);
		if (!fHandle) {
			warning("Unable to open file %s for saving", tmp.c_str());
			return false;
		}

		Common::strlcpy(currentSaveName[slot], desc.c_str(), sizeof(CommandeType));

		fHandle->write(currentSaveName, sizeof(currentSaveName));
		delete fHandle;

		makeSave(saveFileName, getTotalPlayTime() / 1000, desc, false);

		return true;
	} else {
		return makeLoad(saveFileName);
	}
}

void CineEngine::makeSystemMenu() {
	int16 numEntry, systemCommand;
	int16 mouseX, mouseY, mouseButton;
	int16 selectedSave;

	g_cine->_previousSaid.clear();

	if (disableSystemMenu != 1) {
		inMenu = true;

		manageEvents(MAKE_SYSTEM_MENU, UNTIL_MOUSE_BUTTON_UP);
		getMouseData(mouseUpdateStatus, (uint16 *)&mouseButton, (uint16 *)&mouseX, (uint16 *)&mouseY);

		numEntry = 6;

		if (!allowPlayerInput) {
			numEntry--;
		}

		renderer->saveBackBuffer(BEFORE_OPENING_MENU);

		systemCommand = makeMenuChoice(systemMenu, numEntry, mouseX, mouseY, 140);

		switch (systemCommand) {
		case 0: { // Pause
			renderer->drawString(otherMessages[2], 0);
			PauseToken pauseToken = g_cine->pauseEngine();
			waitPlayerInput();
			break;
		}
		case 1: { // Restart Game
			getMouseData(mouseUpdateStatus, (uint16 *)&mouseButton, (uint16 *)&mouseX, (uint16 *)&mouseY);
			if (!makeMenuChoice(confirmMenu, 2, mouseX, mouseY + 8, 100)) {
				_restartRequested = true;
			}
			break;
		}
		case 2: { // Quit
			getMouseData(mouseUpdateStatus, (uint16 *)&mouseButton, (uint16 *)&mouseX, (uint16 *)&mouseY);
			if (!makeMenuChoice(confirmMenu, 2, mouseX, mouseY + 8, 100)) {
				quitGame();
			}
			break;
		}
		case 3: { // Select save drive... change ?
			break;
		}
		case 4: { // load game
			if (loadSaveDirectory()) {
				if (!ConfMan.getBool("originalsaveload")) {
					scummVMSaveLoadDialog(false);
					inMenu = false;
					return;
				}

				getMouseData(mouseUpdateStatus, (uint16 *)&mouseButton, (uint16 *)&mouseX, (uint16 *)&mouseY);

				// Choose savegame range first (0-19, 20-39, 40-59, 60-79, 80-99) and then save slot
				int start = makeMenuChoice(saveChoices, ARRAYSIZE(saveChoices), mouseX, mouseY + 8, 9 * 5);
				if (start >= 0) {
					start *= kMaxOrigUiSavegames;
					selectedSave = makeMenuChoice(&currentSaveName[start], kMaxOrigUiSavegames, mouseX, mouseY + 8, 180);
					if (selectedSave >= 0) {
						selectedSave += start;
					}
				} else {
					selectedSave = -1;
				}

				if (selectedSave >= 0) {
					getMouseData(mouseUpdateStatus, (uint16 *)&mouseButton, (uint16 *)&mouseX, (uint16 *)&mouseY);
					if (!makeMenuChoice(confirmMenu, 2, mouseX, mouseY + 8, 100)) {
						char loadString[256];

						Common::sprintf_s(loadString, otherMessages[3], currentSaveName[selectedSave]);
						renderer->drawString(loadString, 0);

						loadGameState(selectedSave);
					} else {
						renderer->drawString(otherMessages[4], 0);
						waitPlayerInput();
						checkDataDisk(-1);
					}
				} else {
					renderer->drawString(otherMessages[4], 0);
					waitPlayerInput();
					checkDataDisk(-1);
				}
			} else {
				renderer->drawString(otherMessages[5], 0);
				waitPlayerInput();
				checkDataDisk(-1);
			}
			break;
		}
		case 5: { // Save game
			loadSaveDirectory();

			if (!ConfMan.getBool("originalsaveload")) {
				scummVMSaveLoadDialog(true);
				inMenu = false;
				return;
			}

			// Choose savegame range first (0-19, 20-39, 40-59, 60-79, 80-99) and then save slot
			int start = makeMenuChoice(saveChoices, ARRAYSIZE(saveChoices), mouseX, mouseY + 8, 9 * 5);
			if (start >= 0) {
				start *= kMaxOrigUiSavegames;
				int minY = ((start == 0) ? g_cine->getAutosaveSlot() + 1 : 0); // Don't allow saving over autosave slot
				selectedSave = makeMenuChoice(&currentSaveName[start], kMaxOrigUiSavegames, mouseX, mouseY + 8, 180, minY);
				if (selectedSave >= 0) {
					selectedSave += start;
				}
			} else {
				selectedSave = -1;
			}

			if (selectedSave >= 0) {
				CommandeType saveName;
				saveName[0] = 0;
				_saveInputMenuOpen = true;

				if (!makeTextEntryMenu(otherMessages[6], saveName, sizeof(CommandeType), 120)) {
					_saveInputMenuOpen = false;
					break;
				}

				Common::strlcpy(currentSaveName[selectedSave], saveName, sizeof(CommandeType));

				Common::String saveFileName = getSaveStateName(selectedSave);

				getMouseData(mouseUpdateStatus, (uint16 *)&mouseButton, (uint16 *)&mouseX, (uint16 *)&mouseY);
				if (!makeMenuChoice(confirmMenu, 2, mouseX, mouseY + 8, 100)) {
					char saveString[256];
					Common::String tmp = Common::String::format("%s.dir", _targetName.c_str());

					Common::OutSaveFile *fHandle = _saveFileMan->openForSaving(tmp);
					if (!fHandle) {
						warning("Unable to open file %s for saving", tmp.c_str());
						break;
					}

					fHandle->write(currentSaveName, sizeof(currentSaveName));
					delete fHandle;

					Common::sprintf_s(saveString, otherMessages[3], currentSaveName[selectedSave]);
					renderer->drawString(saveString, 0);

					makeSave(saveFileName, getTotalPlayTime() / 1000, Common::String((const char *)currentSaveName), false);

					checkDataDisk(-1);
				} else {
					renderer->drawString(otherMessages[4], 0);
					waitPlayerInput();
					checkDataDisk(-1);
				}
			}
			break;
		}
		default:
			break;
		}

		inMenu = false;
	}
}

void drawMessageBox(int16 x, int16 y, int16 width, int16 currentY, int16 offset, int16 color, byte *page) {
	gfxDrawLine(x + offset, y + offset, x + width - offset, y + offset, color, page);   // top
	gfxDrawLine(x + offset, currentY + 4 - offset, x + width - offset, currentY + 4 - offset, color, page); // bottom
	gfxDrawLine(x + offset, y + offset, x + offset, currentY + 4 - offset, color, page);    // left
	gfxDrawLine(x + width - offset, y + offset, x + width - offset, currentY + 4 - offset, color, page);    // right
}

void drawDoubleMessageBox(int16 x, int16 y, int16 width, int16 currentY, int16 color, byte *page) {
	drawMessageBox(x, y, width, currentY, 1, 0, page);
	drawMessageBox(x, y, width, currentY, 0, color, page);
}

void processInventory(int16 x, int16 y) {
	int menuWidth;
	int listSize;
	int commandParam;

	if (g_cine->getGameType() == Cine::GType_FW) {
		menuWidth = 140;
		commandParam = -2;
	} else { // Operation Stealth
		menuWidth = 160;
		commandParam = -3;
	}

	listSize = buildObjectListCommand(commandParam);

	if (!listSize)
		return;

	Common::StringArray list;
	for (int i = 0; i < listSize; ++i) {
		list.push_back(objectListCommand[i]);
		// Some items are duplicated in the inventory, so clear _previousSaid to make sure they are repeated
		g_cine->_previousSaid.clear();
		g_cine->sayText(objectListCommand[i], Common::TextToSpeechManager::QUEUE);
	}
	SelectionMenu *menu = new SelectionMenu(Common::Point(x, y), menuWidth, list);

	inMenu = true;

	renderer->pushMenu(menu);
	renderer->drawFrame();
	renderer->popMenu();
	delete menu;
	menu = nullptr;

	manageEvents(PROCESS_INVENTORY, UNTIL_MOUSE_BUTTON_DOWN_UP);

	inMenu = false;
	g_cine->stopTextToSpeech();
}

int16 buildObjectListCommand(int16 param) {
	int16 i = 0, j = 0;

	for (i = 0; i < 20; i++) {
		objectListCommand[i][0] = 0;
	}

	for (i = 0; i < 255; i++) {
		if (g_cine->_objectTable[i].name[0] && g_cine->_objectTable[i].costume == param) {
			Common::strcpy_s(objectListCommand[j], g_cine->_objectTable[i].name);
			objListTab[j] = i;
			j++;
		}
	}

	return j;
}

int16 selectSubObject(int16 x, int16 y, int16 param) {
	int16 listSize = buildObjectListCommand(param);
	int16 selectedObject = -1;
	bool osExtras = g_cine->getGameType() == Cine::GType_OS;

	if (!listSize) {
		return -2;
	}

	if (disableSystemMenu == 0) {
		inMenu = true;
		selectedObject = makeMenuChoice(objectListCommand, listSize, x, y, 140, 0, osExtras, osExtras);
		inMenu = false;
	}

	if (selectedObject == -1)
		return -1;

	if (osExtras) {
		if (selectedObject >= 8000) {
			return objListTab[selectedObject - 8000] + 8000;
		}
	}

	if (selectedObject >= 20)
		error("Invalid value for selectedObject: %d", selectedObject);
	return objListTab[selectedObject];
}

void makeCommandLine() {
	if (g_cine->getGameType() == Cine::GType_FW)
		makeFWCommandLine();
	else
		makeOSCommandLine();
}

void makeOSCommandLine() {
	uint16 x, y;

	commandVar1 = 0;
	commandVar2 = -10;

	if (playerCommand != -1) {
		g_cine->_commandBuffer = defaultActionCommand[playerCommand];

		// This is not present in disassembly but here to implement
		// prepositions for other than USE command.
		if (choiceResultTable[playerCommand] != 2 && commandPrepositionTable[playerCommand][0]) {
			g_cine->_commandBuffer += " ";
			g_cine->_commandBuffer += commandPrepositionTable[playerCommand];
		}
	} else {
		g_cine->_commandBuffer = "";
	}

	if ((playerCommand != -1) && (choiceResultTable[playerCommand] == 2)) { // need object selection?
		int16 si;

		getMouseData(mouseUpdateStatus, &dummyU16, &x, &y);
		si = selectSubObject(x, y + 8, -subObjectUseTable[playerCommand]);

		if (si < 0) {
			canUseOnObject = 0;
		} else {
			if (si >= 8000) {
				si -= 8000;
				canUseOnObject = canUseOnItemTable[playerCommand];
			} else {
				canUseOnObject = 0;
			}

			commandVar3[0] = si;
			commandVar1 = 1;
			g_cine->_commandBuffer += " ";
			g_cine->_commandBuffer += g_cine->_objectTable[commandVar3[0]].name;
			g_cine->_commandBuffer += " ";
			g_cine->_commandBuffer += commandPrepositionTable[playerCommand];
		}
	}

	if (playerCommand == 2) {
		getMouseData(mouseUpdateStatus, &dummyU16, &x, &y);
		CursorMan.showMouse(false);
		processInventory(x, y + 8);
		playerCommand = -1;
		commandVar1 = 0;
		g_cine->_commandBuffer = "";
		CursorMan.showMouse(true);
	}

	if (playerCommand != 2) {
		if (playerCommand != -1 && canUseOnObject != 0) { // call use on sub object
			int16 si;

			getMouseData(mouseUpdateStatus, &dummyU16, &x, &y);

			si = selectSubObject(x, y + 8, -subObjectUseTable[playerCommand]);

			if (si >= 0) {
				if (si >= 8000) {
					si -= 8000;
				}

				commandVar3[commandVar1] = si;
				commandVar1++;
				g_cine->_commandBuffer += " ";
				g_cine->_commandBuffer += g_cine->_objectTable[si].name;
			}
		}

		isDrawCommandEnabled = 1;

		if (playerCommand != -1 && choiceResultTable[playerCommand] == commandVar1) {
			SelectedObjStruct obj;
			obj.idx = commandVar3[0];
			obj.param = commandVar3[1];
			int16 di = getRelEntryForObject(playerCommand, commandVar1, &obj);

			if (di != -1) {
				runObjectScript(di);
			} else {
				addPlayerCommandMessage(playerCommand);
			}

			playerCommand = -1;
			commandVar1 = 0;
			g_cine->_commandBuffer = "";
		}
	}

	isDrawCommandEnabled = 1;
	renderer->setCommand(g_cine->_commandBuffer);
}

void makeFWCommandLine() {
	uint16 x, y;

	commandVar1 = 0;
	commandVar2 = -10;

	if (playerCommand != -1) {
		g_cine->_commandBuffer = defaultActionCommand[playerCommand];

		// This is not present in disassembly but here to implement
		// prepositions for other than USE command.
		if (choiceResultTable[playerCommand] != 2 && commandPrepositionTable[playerCommand][0]) {
			g_cine->_commandBuffer += " ";
			g_cine->_commandBuffer += commandPrepositionTable[playerCommand];
		}
	} else {
		g_cine->_commandBuffer = "";
	}

	if ((playerCommand != -1) && (choiceResultTable[playerCommand] == 2)) { // need object selection?
		int16 si;

		getMouseData(mouseUpdateStatus, &dummyU16, &x, &y);
		si = selectSubObject(x, y + 8, -2);

		if (si < 0) {
			playerCommand = -1;
			g_cine->_commandBuffer = "";
		} else {
			commandVar3[0] = si;
			commandVar1 = 1;
			g_cine->_commandBuffer += " ";
			g_cine->_commandBuffer += g_cine->_objectTable[commandVar3[0]].name;
			g_cine->_commandBuffer += " ";
			g_cine->_commandBuffer += defaultCommandPreposition;
		}
	}

	if (!(playerCommand != -1 && choiceResultTable[playerCommand] == 2)) {
		if (playerCommand == 2) {
			getMouseData(mouseUpdateStatus, &dummyU16, &x, &y);
			CursorMan.showMouse(false);
			processInventory(x, y + 8);
			playerCommand = -1;
			commandVar1 = 0;
			g_cine->_commandBuffer = "";
			CursorMan.showMouse(true);
		}
	}

	if (!disableSystemMenu) {
		isDrawCommandEnabled = 1;
		g_cine->_previousSaid.clear();
		renderer->setCommand(g_cine->_commandBuffer);
	}
}

uint16 needMouseSave = 0;

uint16 menuVar4 = 0;
uint16 menuVar5 = 0;

int16 makeMenuChoice(const CommandeType commandList[], uint16 height, uint16 X, uint16 Y, uint16 width, int minY, bool recheckValue, bool allowEmpty) {
	int16 paramY;
	uint16 button;
	int16 var_A;
	uint16 j;
	int16 mouseX, mouseY;
	int16 currentSelection, oldSelection;
	int16 var_4;
	SelectionMenu *menu;

	Common::StringArray list;
	for (uint16 i = minY; i < height; ++i)
		list.push_back(commandList[i]);
	height -= minY; // Remove values before minY

	paramY = (height * 9) + 10;

	if (X + width > 319) {
		X = 319 - width;
	}

	if (Y + paramY > 199) {
		Y = 199 - paramY;
	}

	menu = new SelectionMenu(Common::Point(X, Y), width, list);
	renderer->pushMenu(menu);
	renderer->drawFrame();

	manageEvents(MAKE_MENU_CHOICE, UNTIL_MOUSE_BUTTON_UP);
	getMouseData(mouseUpdateStatus, &button, &dummyU16, &dummyU16);

	var_A = 0;

	currentSelection = allowEmpty ? -1 : 0;

	menu->setSelection(currentSelection);
	renderer->drawFrame();

	manageEvents(MAKE_MENU_CHOICE, EMPTY_EVENT_QUEUE);
	getMouseData(mouseUpdateStatus, &button, (uint16 *)&mouseX, (uint16 *)&mouseY);

	menuVar = 0;

	do {
		Common::Rect all(X + 1, Y + 1, X + width, Y + paramY + 1);
		Common::Array<Common::Rect> rects;

		if (currentSelection == -1) {
			rects.push_back(all);
		} else if (allowEmpty) {
			rects.push_back(Common::Rect(0, 0, 320, all.top)); // All above menu (left, top, right)
			rects.push_back(Common::Rect(0, all.bottom, 320, 200)); // All below menu (left, bottom, right)
			rects.push_back(Common::Rect(0, all.top, all.left, all.bottom)); // To the left of menu
			rects.push_back(Common::Rect(all.right, all.top, 320, all.bottom)); // To the right of menu
		}

		if (currentSelection > 0) {
			Common::Rect aboveCurr(X + 1, Y + 1, X + width, currentSelection * 9 + Y + 4);
			rects.push_back(aboveCurr);
		}

		if (currentSelection != -1 && currentSelection < height - 1) {
			Common::Rect belowCurr(X + 1, currentSelection * 9 + 13 + Y, X + width, Y + paramY + 1);
			rects.push_back(belowCurr);
		}

		manageEvents(MAKE_MENU_CHOICE, UNTIL_MOUSE_BUTTON_DOWN_OR_KEY_UP_OR_DOWN_OR_IN_RECTS, false, rects);
		getMouseData(mouseUpdateStatus, &button, (uint16 *)&mouseX, (uint16 *)&mouseY);

		if (button) {
			var_A = 1;
		}

		oldSelection = currentSelection;

		if (needMouseSave) {
			for (j = 0; j < 3; j++) {
				mainLoopSub6();
			}

			if (menuVar4 && currentSelection > 0) { // go up
				currentSelection--;
			}

			if (menuVar5) { // go down
				if (height - 1 > currentSelection) {
					currentSelection++;
				}
			}
		} else {
			int selectionValueDiff = 0;
			while (!g_cine->_actionList.empty()) {
				switch (g_cine->_actionList.back()) {
				case kActionMenuOptionUp:
					selectionValueDiff--;
					break;
				case kActionMenuOptionDown:
					selectionValueDiff++;
					break;
				default:
					break;
				}
				g_cine->_actionList.pop_back();
			}

			if (selectionValueDiff != 0) {
				if (currentSelection == -1) {
					if (selectionValueDiff > 0) {
						// Start going down from the top
						currentSelection = 0;
						selectionValueDiff--;
					} else if (selectionValueDiff < 0) {
						// Start going up from the bottom
						currentSelection = height - 1;
						selectionValueDiff++;
					}
				}

				currentSelection = CLIP<int16>(currentSelection + selectionValueDiff, 0, height - 1);

				if (currentSelection != oldSelection) {
					Common::Point currentSelectionCenter(X + width / 2, (currentSelection * 9) + Y + 8);
					g_system->warpMouse(currentSelectionCenter.x, currentSelectionCenter.y);
				}
			} else if (mouseX > X && mouseX < X + width && mouseY > Y && mouseY < Y + paramY + 1) {
				// Y value range for selection s:
				// (mouseY - (Y + 4)) / 9 >= s
				// mouseY - (Y + 4) >= s * 9 <=>
				// mouseY >= s * 9 + Y + 4
				//
				// (mouseY - (Y + 4)) / 9 < (s + 1) <=>
				// mouseY - (Y + 4) < (s + 1) * 9 <=>
				// mouseY < s * 9 + 13 + Y
				currentSelection = (mouseY - (Y + 4)) / 9;

				if (currentSelection < 0)
					currentSelection = 0;

				if (currentSelection >= height)
					currentSelection = height - 1;
			} else if (allowEmpty) {
				currentSelection = -1;
			}
		}

		if (currentSelection != oldSelection) { // old != new
			if (needMouseSave) {
				hideMouse();
			}

			menu->setSelection(currentSelection);
			renderer->drawFrame();

//			if (needMouseSave) {
//				gfxRedrawMouseCursor();
//			}
		}

	} while (!var_A && !g_cine->shouldQuit());

	assert(!needMouseSave);

	var_4 = button;

	menuVar = 0;

	manageEvents(MAKE_MENU_CHOICE, UNTIL_MOUSE_BUTTON_UP);
	getMouseData(mouseUpdateStatus, &button, &dummyU16, &dummyU16);

	if (currentSelection != -1) {
		currentSelection += minY;
	}

	if (var_4 == 2) { // recheck
		if (!recheckValue)
			return -1;
		else if (currentSelection != -1)
			return currentSelection + 8000;
	}

	return currentSelection;
}

void makeActionMenu() {
	uint16 mouseButton;
	uint16 mouseX;
	uint16 mouseY;

	g_cine->_previousSaid.clear();

	inMenu = true;

	getMouseData(mouseUpdateStatus, &mouseButton, &mouseX, &mouseY);

	if (g_cine->getGameType() == Cine::GType_OS) {
		if (disableSystemMenu == 0) {
			playerCommand = makeMenuChoice(defaultActionCommand, 6, mouseX, mouseY, 70, 0, true, true);
		}

		if (playerCommand >= 8000) {
			playerCommand -= 8000;
			canUseOnObject = canUseOnItemTable[playerCommand];
		}
	} else {
		if (disableSystemMenu == 0) {
			playerCommand = makeMenuChoice(defaultActionCommand, 6, mouseX, mouseY, 70);
		}
	}

	inMenu = false;
}

void mouseLeftRightDown() {
	// Left and right mouse buttons are down
	g_cine->makeSystemMenu();
}

void allowPlayerInputMouseRightDown() {
	// Player input is allowed, left mouse button is up, right mouse button is down
	makeActionMenu();
	makeCommandLine();
}

void playerCommandMouseLeft(uint16 &mouseButton, uint16 &mouseX, uint16 &mouseY) {
	// A player command is given, left mouse button is down, right mouse button is up
	int16 si;
	manageEvents(EXECUTE_PLAYER_INPUT, UNTIL_MOUSE_BUTTON_UP);
	getMouseData(mouseUpdateStatus, &mouseButton, &dummyU16, &dummyU16);

	si = getObjectUnderCursor(mouseX, mouseY);

	if (si != -1) {
		commandVar3[commandVar1] = si;
		commandVar1++;

		g_cine->_commandBuffer += " ";
		g_cine->_commandBuffer += g_cine->_objectTable[si].name;

		isDrawCommandEnabled = 1;

		if (choiceResultTable[playerCommand] == commandVar1) {
			int16 relEntry;

			SelectedObjStruct obj;
			obj.idx = commandVar3[0];
			obj.param = commandVar3[1];

			relEntry = getRelEntryForObject(playerCommand, commandVar1, &obj);

			if (relEntry != -1) {
				runObjectScript(relEntry);
			} else {
				addPlayerCommandMessage(playerCommand);
			}

			playerCommand = -1;

			commandVar1 = 0;
			g_cine->_commandBuffer = "";
		} else if (g_cine->getGameType() == Cine::GType_OS) {
			isDrawCommandEnabled = 1;
			g_cine->_commandBuffer += " ";
			g_cine->_commandBuffer += commandPrepositionTable[playerCommand];
		}

		renderer->setCommand(g_cine->_commandBuffer);
	} else {
		g_cine->_globalVars[VAR_MOUSE_X_POS] = mouseX;
		if (!mouseX) {
			g_cine->_globalVars[VAR_MOUSE_X_POS]++;
		}

		g_cine->_globalVars[VAR_MOUSE_Y_POS] = mouseY;

		if (g_cine->getGameType() == Cine::GType_OS) {
			if (!mouseY) {
				g_cine->_globalVars[VAR_MOUSE_Y_POS]++;
			}
			g_cine->_globalVars[VAR_MOUSE_X_POS_2ND] = g_cine->_globalVars[VAR_MOUSE_X_POS];
			g_cine->_globalVars[VAR_MOUSE_Y_POS_2ND] = g_cine->_globalVars[VAR_MOUSE_Y_POS];
		}
	}
}

void playerCommandMouseLeftRightUp(uint16 mouseX, uint16 mouseY) {
	// A player command is given, left and right mouse buttons are up
	int16 objIdx;

	objIdx = getObjectUnderCursor(mouseX, mouseY);

	// Previously in Operation Stealth the following code was always run but in
	// Future Wars only if commandVar2 != objIdx (Both cases based on disassembly).
	// Trying to update the command line e.g. "EXAMINE" -> "EXAMINE scaffolding"
	// in the manageEvents function to make the user interface responsive made a
	// regression in Future Wars.
	//
	// In Future Wars the command line was not always updated and thus failed sometimes
	// to be up to date (i.e. showing wrong text, e.g. "EXAMINE" only when it should
	// have read "EXAMINE scaffolding" because the mouse cursor was on the scaffolding).
	//
	// Now we just always run this code for both Future Wars and Operation Stealth
	// which seems to fix the command line updating.
	const bool update = true; // Previously: g_cine->getGameType() == Cine::GType_OS || commandVar2 != objIdx
	if (update) {
		if (objIdx != -1) {
			renderer->setCommand(g_cine->_commandBuffer + " " + g_cine->_objectTable[objIdx].name);
		} else {
			isDrawCommandEnabled = 1;
		}
	}

	commandVar2 = objIdx;
}

void noPlayerCommandMouseLeft(uint16 &mouseX, uint16 &mouseY) {
	// No player command is given, left mouse button is down, right mouse button is up
	int16 objIdx;
	int16 relEntry;

	g_cine->_globalVars[VAR_MOUSE_X_POS] = mouseX;
	if (!mouseX) {
		g_cine->_globalVars[VAR_MOUSE_X_POS]++;
	}

	g_cine->_globalVars[VAR_MOUSE_Y_POS] = mouseY;

	if (g_cine->getGameType() == Cine::GType_OS) {
		if (!mouseY) {
			g_cine->_globalVars[VAR_MOUSE_Y_POS]++;
		}
		g_cine->_globalVars[VAR_MOUSE_X_POS_2ND] = g_cine->_globalVars[VAR_MOUSE_X_POS];
		g_cine->_globalVars[VAR_MOUSE_Y_POS_2ND] = g_cine->_globalVars[VAR_MOUSE_Y_POS];
	}

	objIdx = getObjectUnderCursor(mouseX, mouseY);

	if (objIdx != -1) {
		currentSelectedObject.idx = objIdx;
		currentSelectedObject.param = -1;

		relEntry = getRelEntryForObject(6, 1, &currentSelectedObject);

		if (relEntry != -1) {
			bool skipSafeControlAccess = false;

			// HACK: Throttle speed of otherwise overly sensitive safe controls (Bug #11621)
			if (hacksEnabled && g_cine->getGameType() == Cine::GType_OS &&
				scumm_stricmp(renderer->getBgName(), "COFFRE.PI1") == 0 &&
				scumm_stricmp(currentPrcName, "PALAIS1.PRC") == 0) {
				uint32 now = g_system->getMillis();

				// Throttle access to the same safe control repeatedly in succession.
				if (safeControlsLastAccessedMs != 0 &&
					(now - safeControlsLastAccessedMs) < safeControlAccessMinMs() &&
					objIdx == lastSafeControlObjIdx) {
					skipSafeControlAccess = true;
					warning("Skipping safe control access (Time since last called = %d ms < throttling value of %d ms)",
						now - safeControlsLastAccessedMs, safeControlAccessMinMs());
				} else {
					safeControlsLastAccessedMs = now;
				}

				lastSafeControlObjIdx = objIdx;
			}

			if (!skipSafeControlAccess) {
				runObjectScript(relEntry);
			}
		}
	}
}

uint16 executePlayerInput() {
	uint16 var_5E;
	uint16 var_2;
	uint16 mouseX, mouseY, mouseButton;

	canUseOnObject = 0;

	if (isInPause) {
		renderer->drawString(otherMessages[2], 0);
		PauseToken pauseToken = g_cine->pauseEngine();
		waitPlayerInput();
		isInPause = 0;
	}

	if (allowPlayerInput) { // Player input is allowed
		if (isDrawCommandEnabled) {
			renderer->setCommand(g_cine->_commandBuffer);
		}
		isDrawCommandEnabled = 0;

		// DIFFERENCE FROM DISASSEMBLY:
		// See renderer's drawFrame function for comments related to how the waiting
		// period from the original engine has been moved to here. In the original
		// engine a maximum of 200 pollings of the mouse state were done here and
		// the maximum state of the mouse buttons were read. Now we wait for
		// g_cine->getTimerDelay() here while reading the mouse state and updating
		// the command line.
		manageEvents(EXECUTE_PLAYER_INPUT, UNTIL_WAIT_ENDED, true);
	} else {
		manageEvents(EXECUTE_PLAYER_INPUT, UNTIL_MOUSE_BUTTON_UP_AND_WAIT_ENDED, true);
	}

	// Get mouse position and button states
	getMouseData(mouseUpdateStatus, &mouseButton, &mouseX, &mouseY);

	if ((mouseButton & kLeftMouseButton) && (mouseButton & kRightMouseButton)) {
		// Left and right mouse buttons are down
		mouseLeftRightDown();
	} else if (allowPlayerInput) { // Player input is allowed
		if (!(mouseButton & kLeftMouseButton) && (mouseButton & kRightMouseButton)) {
			// Player input is allowed, left mouse button is up, right mouse button is down
			allowPlayerInputMouseRightDown();
		} else if (playerCommand != -1) { // A player command is given
			if (mouseButton & kLeftMouseButton) { // Left mouse button is down
				if (!(mouseButton & kRightMouseButton)) { // Right mouse button is up
					// A player command is given, left mouse button is down, right mouse button is up
					playerCommandMouseLeft(mouseButton, mouseX, mouseY);
				}
			} else if (!(mouseButton & kRightMouseButton)) { // Right mouse button is up
				// A player command is given, left and right mouse buttons are up
				playerCommandMouseLeftRightUp(mouseX, mouseY);
			}
		} else { // No player command is given
			if (!(mouseButton & kRightMouseButton)) { // Right mouse button is up
				if (mouseButton & kLeftMouseButton) { // Left mouse button is down
					// No player command is given, left mouse button is down, right mouse button is up
					noPlayerCommandMouseLeft(mouseX, mouseY);
				}
			}
		}
	}

	var_2 = menuVar & 0x7F;
	var_5E = var_2;

	if (menuVar & 0x80) {
		var_5E = 0;
		var_2 = 0;
	}

	if (g_cine->getGameType() == Cine::GType_OS) { // OS: Move using keyboard
		// Handle possible horizontal movement by keyboard
		if (xMoveKeyb != kKeybMoveCenterX && allowPlayerInput) {
			if (xMoveKeyb == kKeybMoveRight) { // moving right
				const int16 playerFrame = g_cine->_objectTable[1].frame;
				const int16 playerX = g_cine->_objectTable[1].x;
				// TODO: Check if multiplying _width by two here is correct or not
				const int16 newX = g_cine->_animDataTable[playerFrame]._width * 2 + playerX + 8;
				g_cine->_globalVars[VAR_MOUSE_X_POS] = g_cine->_globalVars[VAR_MOUSE_X_POS_2ND] = newX;
			} else { // moving left
				const int16 playerX = g_cine->_objectTable[1].x;
				const int16 newX = playerX - 8;
				g_cine->_globalVars[VAR_MOUSE_X_POS] = g_cine->_globalVars[VAR_MOUSE_X_POS_2ND] = newX;
			}

			// Restrain horizontal position to range 0-319
			if (g_cine->_globalVars[VAR_MOUSE_X_POS] < 0) {
				g_cine->_globalVars[VAR_MOUSE_X_POS] = g_cine->_globalVars[VAR_MOUSE_X_POS_2ND] = 0;
			} else if (g_cine->_globalVars[VAR_MOUSE_X_POS] > 319) {
				g_cine->_globalVars[VAR_MOUSE_X_POS] = g_cine->_globalVars[VAR_MOUSE_X_POS_2ND] = 319;
			}
		}

		// Handle possible vertical movement by keyboard
		if (yMoveKeyb != kKeybMoveCenterY && allowPlayerInput) {
			if (yMoveKeyb == kKeybMoveDown) { // moving down
				const int16 playerFrame = g_cine->_objectTable[1].frame;
				const int16 playerY = g_cine->_objectTable[1].y;
				// TODO: Check if multiplying _height by two here is correct or not
				const int16 newY = g_cine->_animDataTable[playerFrame]._height * 2 + playerY - 1;
				g_cine->_globalVars[VAR_MOUSE_Y_POS] = g_cine->_globalVars[VAR_MOUSE_Y_POS_2ND] = newY;
			} else { // moving up
				const int16 playerY = g_cine->_objectTable[1].y;
				const int16 newY = playerY - 8;
				g_cine->_globalVars[VAR_MOUSE_Y_POS] = g_cine->_globalVars[VAR_MOUSE_Y_POS_2ND] = newY;
			}

			// Restrain vertical position to range 0-199
			if (g_cine->_globalVars[VAR_MOUSE_Y_POS] < 0) {
				g_cine->_globalVars[VAR_MOUSE_Y_POS] = g_cine->_globalVars[VAR_MOUSE_Y_POS_2ND] = 0;
			} else if (g_cine->_globalVars[VAR_MOUSE_Y_POS] > 199) {
				g_cine->_globalVars[VAR_MOUSE_Y_POS] = g_cine->_globalVars[VAR_MOUSE_Y_POS_2ND] = 199;
			}
		}
	} else if (egoMovedWithKeyboard && allowPlayerInput) { // FW: Move using keyboard
		egoMovedWithKeyboard = false;

		switch (g_cine->_globalVars[VAR_MOUSE_X_MODE]) {
		case 1:
			mouseX = g_cine->_objectTable[1].x + 12;
			break;
		case 2:
			mouseX = g_cine->_objectTable[1].x + 7;
			break;
		default:
			mouseX = g_cine->_globalVars[VAR_MOUSE_X_POS];
			break;
		}

		switch (g_cine->_globalVars[VAR_MOUSE_Y_MODE]) {
		case 1:
			mouseY = g_cine->_objectTable[1].y + 34;
			break;
		case 2:
			mouseY = g_cine->_objectTable[1].y + 28;
			break;
		default:
			mouseY = g_cine->_globalVars[VAR_MOUSE_Y_POS];
			break;
		}

		if (var_5E == bgVar0) {
			var_5E = 0;

			g_cine->_globalVars[VAR_MOUSE_X_POS] = mouseX;
			g_cine->_globalVars[VAR_MOUSE_Y_POS] = mouseY;
		} else {
			if (xMoveKeyb) {
				if (xMoveKeyb == kKeybMoveLeft) {
					g_cine->_globalVars[VAR_MOUSE_X_POS] = 1;
				} else {
					g_cine->_globalVars[VAR_MOUSE_X_POS] = 320;
				}
			} else {
				g_cine->_globalVars[VAR_MOUSE_X_POS] = mouseX;
			}

			if (yMoveKeyb) {
				if (yMoveKeyb == kKeybMoveUp) {
					g_cine->_globalVars[VAR_MOUSE_Y_POS] = 1;
				} else {
					g_cine->_globalVars[VAR_MOUSE_Y_POS] = 200;
				}
			} else {
				g_cine->_globalVars[VAR_MOUSE_Y_POS] = mouseY;
			}
		}

		bgVar0 = var_5E;
	}

	if (g_cine->getGameType() == Cine::GType_OS || !(egoMovedWithKeyboard && allowPlayerInput)) {
		getMouseData(mouseUpdateStatus, &mouseButton, &mouseX, &mouseY);

		// TODO: Investigate why some of these buttons don't work (Maybe main_loop.cpp's processEvent has something to do with it?)
		switch (var_2 - 59) {
		case 0: // F1 = EXAMINE
		case 1: // F2 = TAKE
		case 2: // F3 = INVENTORY
		case 3: // F4 = USE
		case 4: // F5 = OPERATE
		case 5: // F6 = SPEAK
			if (allowPlayerInput) {
				playerCommand = var_2 - 59;
				// TODO: Operation Stealth uses shift key here for handling canUseOnObject differently... investigate and implement.
				makeCommandLine();
			}
			break;
		case 6: // F7
		case 7: // F8
		case 8: // F9
		case 23: // Keypad-0/Ins
			// TODO: Restrict this case only to F7 for Operation Stealth
			if (allowPlayerInput) {
				makeActionMenu();
				makeCommandLine();
			}
			break;
		case 9: // F10
		case 24: // Keypad-./Del
			// TODO: Restrict this case only to F10 for Operation Stealth
			g_cine->makeSystemMenu();
			break;
		default:
			break;
		}
	}

	renderer->clearMenuStack();

	return var_5E;
}

void drawSprite(Common::List<overlay>::iterator it, const byte *spritePtr, const byte *maskPtr, uint16 width, uint16 height, byte *page, int16 x, int16 y) {
	byte *msk = nullptr;
	int16 maskX, maskY, maskWidth, maskHeight;
	uint16 maskSpriteIdx;

	msk = (byte *)malloc(width * height);

	if (g_cine->getGameType() == Cine::GType_OS) {
		generateMask(spritePtr, msk, width * height, g_cine->_objectTable[it->objIdx].part);
	} else {
		memcpy(msk, maskPtr, width * height);
	}

	for (++it; it != g_cine->_overlayList.end(); ++it) {
		if (it->type != 5) {
			continue;
		}

		maskX = g_cine->_objectTable[it->objIdx].x;
		maskY = g_cine->_objectTable[it->objIdx].y;

		maskSpriteIdx = ABS((int16)(g_cine->_objectTable[it->objIdx].frame));

		maskWidth = g_cine->_animDataTable[maskSpriteIdx]._realWidth;
		maskHeight = g_cine->_animDataTable[maskSpriteIdx]._height;
		gfxUpdateSpriteMask(msk, x, y, width, height, g_cine->_animDataTable[maskSpriteIdx].data(), maskX, maskY, maskWidth, maskHeight);

#ifdef DEBUG_SPRITE_MASK
		gfxFillSprite(g_cine->_animDataTable[maskSpriteIdx].data(), maskWidth, maskHeight, page, maskX, maskY, 1);
#endif
	}

	gfxDrawMaskedSprite(spritePtr, msk, width, height, page, x, y);
	free(msk);
}

void removeMessages() {
	Common::List<overlay>::iterator it;
	bool remove;

	for (it = g_cine->_overlayList.begin(); it != g_cine->_overlayList.end();) {
		if (g_cine->getGameType() == Cine::GType_OS) {
			// NOTE: These are really removeOverlay calls that have been deferred.
			// In Operation Stealth's disassembly elements are removed from the
			// overlay list right in the drawOverlays function (And actually in
			// some other places too) and that's where incrementing the overlay's
			// last parameter by one if it's negative and testing it for positivity
			// comes from too.
			remove = it->type == 3 || (it->type == 2 && (it->color >= 0 || ++(it->color) >= 0));
		} else { // Future Wars
			remove = it->type == 2 || it->type == 3;
		}

		if (remove) {
			it = g_cine->_overlayList.erase(it);
		} else {
			++it;
		}
	}
}

uint16 processKeyboard(uint16 param) {
	return 0;
}

void mainLoopSub6() {
	// (10923000 ms * 3) / 1193180 ~= 27 ms.
	//g_system->delayMillis(27);
}

void checkForPendingDataLoad() {
	if (newPrcName[0] != 0) {
		bool loadPrcOk = loadPrc(newPrcName);

		Common::strcpy_s(currentPrcName, newPrcName);
		newPrcName[0] = '\0';

		// Check that the loading of the script file was successful before
		// trying to add script 1 from it to the global scripts list. This
		// fixes a crash when failing copy protection in Amiga or Atari ST
		// versions of Future Wars.
		if (loadPrcOk) {
			addScriptToGlobalScripts(1);
		} else if (scumm_stricmp(currentPrcName, COPY_PROT_FAIL_PRC_NAME)) {
			// We only show an error here for other files than the file that
			// is loaded if copy protection fails (i.e. L201.ANI).
			warning("checkForPendingDataLoad: loadPrc(%s) failed", currentPrcName);
		}
	}

	if (newRelName[0] != 0) {
		loadRel(newRelName);

		Common::strcpy_s(currentRelName, newRelName);
		newRelName[0] = '\0';
	}

	if (newObjectName[0] != 0) {
		g_cine->_overlayList.clear();

		loadObject(newObjectName);

		Common::strcpy_s(currentObjectName, newObjectName);
		newObjectName[0] = '\0';
	}

	if (newMsgName[0] != 0) {
		loadMsg(newMsgName);

		Common::strcpy_s(currentMsgName, newMsgName);
		newMsgName[0] = '\0';
	}
}

void hideMouse() {
}

void removeExtension(char *dest, const char *source, size_t sz) {
	Common::strcpy_s(dest, sz, source);

	byte *ptr = (byte *) strchr(dest, '.');

	if (ptr) {
		*ptr = 0;
	}
}

void addMessage(byte param1, int16 param2, int16 param3, int16 param4, int16 param5) {
	overlay tmp;

	tmp.objIdx = param1;
	tmp.type = 2;
	tmp.x = param2;
	tmp.y = param3;
	tmp.width = param4;
	tmp.color = param5;

	g_cine->_overlayList.push_back(tmp);
}

void removeSeq(uint16 param1, uint16 param2, uint16 param3) {
	for (auto &seq : g_cine->_seqList) {
		if (seq.objIdx == param1 && seq.var4 == param2 && seq.varE == param3) {
			seq.var4 = -1;
			break;
		}
	}
}

// Checked against Operation Stealth 16 color DOS disassembly, should be correct.
bool isSeqRunning(uint16 param1, uint16 param2, uint16 param3) {
	for (auto &seq : g_cine->_seqList) {
		if (seq.objIdx == param1 && seq.var4 == param2 && seq.varE == param3) {
			// Just to be on the safe side there's a restriction of the
			// addition's result to 16-bit arithmetic here like in the
			// original. It's possible that it's not strictly needed.
			return ((seq.var14 + seq.var16) & 0xFFFF) == 0;
		}
	}

	return true;
}

void addSeqListElement(uint16 objIdx, int16 param1, int16 param2, int16 frame, int16 param4, int16 param5, int16 param6, int16 param7, int16 param8) {
	Common::List<SeqListElement>::iterator it;
	SeqListElement tmp;

	for (it = g_cine->_seqList.begin(); it != g_cine->_seqList.end() && it->varE < param7; ++it)
		;

	tmp.objIdx = objIdx;
	tmp.var4 = param1;
	tmp.var8 = param2;
	tmp.frame = frame;
	tmp.varC = param4;
	tmp.var14 = 0;
	tmp.var16 = 0;
	tmp.var18 = param5;
	tmp.var1A = param6;
	tmp.varE = param7;
	tmp.var10 = param8;
	tmp.var12 = param8;
	tmp.var1C = 0;
	tmp.var1E = 0;

	g_cine->_seqList.insert(it, tmp);
}

void modifySeqListElement(uint16 objIdx, int16 var4Test, int16 param1, int16 param2, int16 param3, int16 param4) {
	// Find a suitable list element and modify it
	for (auto &seq : g_cine->_seqList) {
		if (seq.objIdx == objIdx && seq.var4 == var4Test) {
			seq.varC  = param1;
			seq.var18 = param2;
			seq.var1A = param3;
			seq.var10 = seq.var12 = param4;
			break;
		}
	}
}

void computeMove1(SeqListElement &element, int16 x, int16 y, int16 param1,
				  int16 param2, int16 x2, int16 y2) {
	element.var16 = 0;
	element.var14 = 0;

	if (y2) {
		if (y - param2 > y2) {
			element.var16 = 2;
		}

		if (y + param2 < y2) {
			element.var16 = 1;
		}
	}

	if (x2) {
		if (x - param1 > x2) {
			element.var14 = 2;
		}

		if (x + param1 < x2) {
			element.var14 = 1;
		}
	}
}

uint16 computeMove2(SeqListElement &element) {
	int16 returnVar = 0;

	if (element.var16 == 1) {
		returnVar = 4;
	} else if (element.var16 == 2) {
		returnVar = 3;
	}

	if (element.var14 == 1) {
		returnVar = 1;
	} else if (element.var14 == 2) {
		returnVar = 2;
	}

	return returnVar;
}

uint16 addAni(uint16 param1, uint16 objIdx, const int8 *ptr, SeqListElement &element, uint16 param3, int16 *param4) {
	const int8 *ptrData;
	const int8 *ptr2;
	int16 di;

	debug(5, "addAni: param1 = %d, objIdx = %d, ptr = %p, element.var8 = %d, element.var14 = %d param3 = %d",
	      param1, objIdx, (const void *)ptr, element.var8, element.var14, param3);

	// In the original an error string is set and 0 is returned if the following doesn't hold
	if (!ptr) {
		return 0;
	}

	// We probably could just use a local variable here instead of the dummyU16 but
	// haven't checked if this has any side-effects so keeping it this way still.
	dummyU16 = READ_BE_UINT16(ptr + param1 * 2 + 8);
	ptrData = ptr + dummyU16;

	// In the original an error string is set and 0 is returned if the following doesn't hold
	if (!*ptrData) {
		return 0;
	}

	di = (g_cine->_objectTable[objIdx].costume + 1) % (*ptrData);
	++ptrData; // Jump over the just read byte
	// Here ptr2 seems to be indexing a table of structs (8 bytes per struct):
	// struct {
	//      int8 x;         // 0 (Used with checkCollision)
	//      int8 y;         // 1 (Used with checkCollision)
	//      int8 numZones;  // 2 (Used with checkCollision)
	//      int8 var3;      // 3 (Not used in this function)
	//      int8 xAdd;      // 4 (Used with an object)
	//      int8 yAdd;      // 5 (Used with an object)
	//      int8 maskAdd;   // 6 (Used with an object)
	//      int8 frameAdd;  // 7 (Used with an object)
	// };
	ptr2 = ptrData + di * 8;

	// We might probably safely discard the AND by 1 here because
	// at least in the original checkCollision returns always 0 or 1.
	if ((checkCollision(objIdx, ptr2[0], ptr2[1], ptr2[2], ptr[0]) & 1)) {
		return 0;
	}

	g_cine->_objectTable[objIdx].x += ptr2[4];
	g_cine->_objectTable[objIdx].y += ptr2[5];
	g_cine->_objectTable[objIdx].mask += ptr2[6];

	if (ptr2[6]) {
		resetGfxEntityEntry(objIdx);
	}

	g_cine->_objectTable[objIdx].frame = ptr2[7] + element.var8;

	if (param3 || !element.var14) {
		g_cine->_objectTable[objIdx].costume = di;
	} else {
		assert(param4);
		*param4 = di;
	}

	return 1;
}

/**
 * Permutates the overlay list into a different order according to some logic.
 * @todo Check this function for correctness (Wasn't very easy to reverse engineer so there may be errors)
 */
void resetGfxEntityEntry(uint16 objIdx) {
	Common::List<overlay>::iterator it, bObjsCutPoint;
	Common::List<overlay> aReverseObjs, bObjs;
	bool foundCutPoint = false;

	// Go through the overlay list and partition the whole list into two categories (Type A and type B objects)
	for (it = g_cine->_overlayList.begin(); it != g_cine->_overlayList.end(); ++it) {
		if (it->objIdx == objIdx && it->type != 2 && it->type != 3) { // Type A object
			aReverseObjs.push_front(*it);
		} else { // Type B object
			bObjs.push_back(*it);
			uint16 objectMask;
			if (it->type == 2 || it->type == 3) {
				objectMask = 10000;
			} else {
				objectMask = g_cine->_objectTable[it->objIdx].mask;
			}

			if (g_cine->_objectTable[objIdx].mask > objectMask) { // Check for B objects' cut point
				bObjsCutPoint = bObjs.reverse_begin();
				foundCutPoint = true;
			}
		}
	}

	// Recreate the overlay list in a different order.
	g_cine->_overlayList.clear();
	if (foundCutPoint) {
		// If a cut point was found the order is:
		// B objects before the cut point, the cut point, A objects in reverse order, B objects after cut point.
		++bObjsCutPoint; // Include the cut point in the first list insertion
		g_cine->_overlayList.insert(g_cine->_overlayList.end(), bObjs.begin(), bObjsCutPoint);
		g_cine->_overlayList.insert(g_cine->_overlayList.end(), aReverseObjs.begin(), aReverseObjs.end());
		g_cine->_overlayList.insert(g_cine->_overlayList.end(), bObjsCutPoint, bObjs.end());
	} else {
		// If no cut point was found the order is:
		// A objects in reverse order, B objects.
		g_cine->_overlayList.insert(g_cine->_overlayList.end(), aReverseObjs.begin(), aReverseObjs.end());
		g_cine->_overlayList.insert(g_cine->_overlayList.end(), bObjs.begin(), bObjs.end());
	}
}

void processSeqListElement(SeqListElement &element) {
	int16 x = g_cine->_objectTable[element.objIdx].x;
	int16 y = g_cine->_objectTable[element.objIdx].y;
	const int8 *ptr1 = (const int8 *) g_cine->_animDataTable[element.frame].data();
	int16 var_10;
	int16 var_4;
	int16 var_2;

	// Initial interpretations for variables addressed through ptr1 (8-bit addressing):
	// These may be inaccurate!
	// 0: ?
	// 1: xRadius
	// 2: yRadius
	// 3: ?
	// 4: xAdd
	// 5: yAdd
	// 6: ?
	// 7: ?
	// After this come (At least at positions 0, 1 and 3 in 16-bit addressing)
	// 16-bit big-endian values used for addressing through ptr1.

	if (element.var12 < element.var10) {
		element.var12++;
		return;
	}

	element.var12 = 0;

	if (ptr1) {
		int16 param1 = ptr1[1];
		int16 param2 = ptr1[2];

		if (element.varC != 255) {
			int16 x2 = element.var18;
			int16 y2 = element.var1A;
			if (element.varC) {
				x2 += g_cine->_objectTable[element.varC].x;
				y2 += g_cine->_objectTable[element.varC].y;
			}
			computeMove1(element, ptr1[4] + x, ptr1[5] + y, param1, param2, x2, y2);
		} else {
			if (xMoveKeyb && allowPlayerInput) {
				int16 adder = param1 + 1;
				if (xMoveKeyb != kKeybMoveRight) {
					adder = -adder;
				}
				g_cine->_globalVars[VAR_MOUSE_X_POS] = g_cine->_globalVars[VAR_MOUSE_X_POS_2ND] = ptr1[4] + x + adder;
			}

			if (yMoveKeyb && allowPlayerInput) {
				int16 adder = param2 + 1;
				if (yMoveKeyb != kKeybMoveDown) {
					adder = -adder;
				}
				g_cine->_globalVars[VAR_MOUSE_Y_POS] = g_cine->_globalVars[VAR_MOUSE_Y_POS_2ND] = ptr1[5] + y + adder;
			}

			if (g_cine->_globalVars[VAR_MOUSE_X_POS] || g_cine->_globalVars[VAR_MOUSE_Y_POS]) {
				computeMove1(element, ptr1[4] + x, ptr1[5] + y, param1, param2, g_cine->_globalVars[VAR_MOUSE_X_POS], g_cine->_globalVars[VAR_MOUSE_Y_POS]);
			} else {
				element.var16 = 0;
				element.var14 = 0;
			}
		}

		var_10 = computeMove2(element);

		if (var_10) {
			element.var1C = var_10;
			element.var1E = var_10;
		}

		var_4 = -1;

		if ((element.var16 == 1
			&& !addAni(3, element.objIdx, ptr1, element, 0, &var_4)) || (element.var16 == 2 && !addAni(2, element.objIdx, ptr1, element, 0, &var_4))) {
			if (element.varC == 255) {
				g_cine->_globalVars[VAR_MOUSE_Y_POS] = 0;
			}
		}

		if ((element.var14 == 1
			&& !addAni(0, element.objIdx, ptr1, element, 1, &var_2))) {
			if (element.varC == 255) {
				g_cine->_globalVars[VAR_MOUSE_X_POS] = 0;

				if (var_4 != -1) {
					g_cine->_objectTable[element.objIdx].costume = var_4;
				}
			}
		}

		if ((element.var14 == 2 && !addAni(1, element.objIdx, ptr1, element, 1, &var_2))) {
			if (element.varC == 255) {
				g_cine->_globalVars[VAR_MOUSE_X_POS] = 0;

				if (var_4 != -1) {
					g_cine->_objectTable[element.objIdx].costume = var_4;
				}
			}
		}

		if (((element.var16 + element.var14) & 0xFFFF) == 0) {
			if (element.var1C) {
				if (element.var1E) {
					g_cine->_objectTable[element.objIdx].costume = 0;
					element.var1E = 0;
				}

				addAni(element.var1C + 3, element.objIdx, ptr1, element, 1, &var_2);

			}
		}

	}
}

void processSeqList() {
	for (auto &seq : g_cine->_seqList) {
		if (seq.var4 == -1) {
			continue;
		}

		processSeqListElement(seq);
	}
}


bool makeTextEntryMenu(const char *messagePtr, char *inputString, int stringMaxLength, int y) {
	int len = strlen(messagePtr);
	int16 width = 6 * len + 20;

	width = CLIP((int)width, 180, 250);

	int16 x = (320 - width) / 2;

	g_cine->_keyInputList.clear();

	int quit = 0;
	bool redraw = true;
	CommandeType tempString;
	int inputLength = strlen(inputString);
	int inputPos = inputLength + 1;

	TextInputMenu *inputBox = new TextInputMenu(Common::Point(x - 16, y), width + 32, messagePtr);
	renderer->pushMenu(inputBox);

	g_cine->sayText(messagePtr, Common::TextToSpeechManager::INTERRUPT);

	while (!quit) {
		if (redraw) {
			inputBox->setInput(inputString, inputPos);
			renderer->drawFrame();
			redraw = false;
		}

		if (g_cine->_keyInputList.empty()) {
			manageEvents(MAKE_TEXT_ENTRY_MENU, UNTIL_MOUSE_BUTTON_DOWN_OR_KEY_INPUT);
		}

		char ch[2];
		memset(tempString, 0, stringMaxLength);
		ch[1] = 0;

		Common::KeyState keyState = Common::KeyState();

		if (!g_cine->_keyInputList.empty()) {
			keyState = g_cine->_keyInputList.back();
			g_cine->_keyInputList.pop_back();
		}

		int keycode = keyState.keycode;
		uint16 ascii = keyState.ascii;
		uint16 mouseButton, mouseX, mouseY;

		getMouseData(0, &mouseButton, &mouseX, &mouseY);

		if ((mouseButton & kRightMouseButton) || g_cine->shouldQuit())
			quit = kRightMouseButton;
		else if (mouseButton & kLeftMouseButton)
			quit = kLeftMouseButton;

		switch (keycode) {
		case Common::KEYCODE_BACKSPACE:
			if (inputPos <= 1) {
				break;
			}
			inputPos--;
			redraw = true;
			// fall through
		case Common::KEYCODE_DELETE:
			if (inputPos <= inputLength) {
				if (inputPos != 1) {
					strncpy(tempString, inputString, inputPos - 1);
				}
				if (inputPos != inputLength) {
					strncat(tempString, &inputString[inputPos], inputLength - inputPos);
				}
				Common::strcpy_s(inputString, stringMaxLength, tempString);
				inputLength = strlen(inputString);
				redraw = true;
			}
			break;
		case Common::KEYCODE_LEFT:
			if (inputPos > 1) {
				inputPos--;
				redraw = true;
			}
			break;
		case Common::KEYCODE_RIGHT:
			if (inputPos <= inputLength) {
				inputPos++;
				redraw = true;
			}
			break;
		default:
			if (((ascii >= 'a') && (ascii <= 'z')) ||
				((ascii >= '0') && (ascii <= '9')) ||
				((ascii >= 'A') && (ascii <= 'Z')) ||
				(ascii == ' ')) {
				if (inputLength < stringMaxLength - 1) {
					ch[0] = ascii;
					if (inputPos != 1) {
						strncpy(tempString, inputString, inputPos - 1);
						Common::strcat_s(tempString, ch);
					}
					if ((inputLength == 0) || (inputPos == 1)) {
						Common::strcpy_s(tempString, ch);
					}
					if ((inputLength != 0) && (inputPos != inputLength)) {
						strncat(tempString, &inputString[inputPos - 1], inputLength - inputPos + 1);
					}

					Common::strcpy_s(inputString, stringMaxLength, tempString);
					inputLength = strlen(inputString);
					inputPos++;
					redraw = true;
				}
			}
			break;
		}
	}

	renderer->popMenu();
	delete inputBox;

	g_cine->sayText(inputString, Common::TextToSpeechManager::QUEUE);

	if (quit == kRightMouseButton)
		return false;

	return true;
}

} // End of namespace Cine
