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

/*
 * This file is based on WME Lite.
 * http://dead-code.org/redir.php?target=wmelite
 * Copyright (c) 2011 Jan Nedoma
 */

#include "engines/wintermute/base/base_surface_storage.h"
#include "engines/wintermute/base/gfx/base_surface.h"
#include "engines/wintermute/base/gfx/base_renderer.h"
#include "engines/wintermute/base/base_game.h"
#include "engines/wintermute/base/base_engine.h"
#include "engines/wintermute/base/base_file_manager.h"
#include "engines/wintermute/platform_osystem.h"
#include "common/str.h"

namespace Wintermute {

//IMPLEMENT_PERSISTENT(BaseSurfaceStorage, true);

//////////////////////////////////////////////////////////////////////
BaseSurfaceStorage::BaseSurfaceStorage(BaseGame *inGame) : BaseClass(inGame) {
	_lastCleanupTime = 0;
}


//////////////////////////////////////////////////////////////////////
BaseSurfaceStorage::~BaseSurfaceStorage() {
	cleanup(true);
}


//////////////////////////////////////////////////////////////////////////
bool BaseSurfaceStorage::cleanup(bool warn) {
	for (uint32 i = 0; i < _surfaces.getSize(); i++) {
		if (warn) {
			BaseEngine::LOG(0, "BaseSurfaceStorage warning: purging surface '%s', usage:%d", _surfaces[i]->getFileName(), _surfaces[i]->_referenceCount);
		}
		delete _surfaces[i];
	}
	_surfaces.removeAll();

	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////////
bool BaseSurfaceStorage::initLoop() {
	if (_gameRef->_smartCache && _gameRef->getLiveTimer()->getTime() - _lastCleanupTime >= _gameRef->_surfaceGCCycleTime) {
		_lastCleanupTime = _gameRef->getLiveTimer()->getTime();
		sortSurfaces();
		for (uint32 i = 0; i < _surfaces.getSize(); i++) {
			if (_surfaces[i]->_lifeTime <= 0) {
				break;
			}

			if (_surfaces[i]->_lifeTime > 0 && _surfaces[i]->_valid && (int)(_gameRef->getLiveTimer()->getTime() - _surfaces[i]->_lastUsedTime) >= _surfaces[i]->_lifeTime) {
				//_gameRef->QuickMessageForm("Invalidating: %s", _surfaces[i]->_filename);
				_surfaces[i]->invalidate();
			}
		}
	}
	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////
bool BaseSurfaceStorage::removeSurface(BaseSurface *surface) {
	for (uint32 i = 0; i < _surfaces.getSize(); i++) {
		if (_surfaces[i] == surface) {
			_surfaces[i]->_referenceCount--;
			if (_surfaces[i]->_referenceCount <= 0) {
				delete _surfaces[i];
				_surfaces.removeAt(i);
			}
			break;
		}
	}
	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////
BaseSurface *BaseSurfaceStorage::addSurface(const Common::String &filename, bool defaultCK, byte ckRed, byte ckGreen, byte ckBlue, int lifeTime, bool keepLoaded) {
	for (uint32 i = 0; i < _surfaces.getSize(); i++) {
		if (scumm_stricmp(_surfaces[i]->getFileName(), filename.c_str()) == 0) {
			_surfaces[i]->_referenceCount++;
			return _surfaces[i];
		}
	}

	if (!BaseFileManager::getEngineInstance()->hasFile(filename)) {
		if (filename.size()) {
			BaseEngine::LOG(0, "Missing image: '%s'", filename.c_str());
		}
		if (_gameRef->_debugDebugMode) {
			return addSurface("invalid_debug.bmp", defaultCK, ckRed, ckGreen, ckBlue, lifeTime, keepLoaded);
		} else {
			return addSurface("invalid.bmp", defaultCK, ckRed, ckGreen, ckBlue, lifeTime, keepLoaded);
		}
	}

	BaseSurface *surface;
	surface = BaseEngine::getRenderer()->createSurface();

	if (!surface) {
		return nullptr;
	}

	if (DID_FAIL(surface->create(filename, defaultCK, ckRed, ckGreen, ckBlue, lifeTime, keepLoaded))) {
		delete surface;
		return nullptr;
	} else {
		surface->_referenceCount = 1;
		_surfaces.add(surface);
		return surface;
	}
}


//////////////////////////////////////////////////////////////////////
bool BaseSurfaceStorage::restoreAll() {
	bool ret;
	for (uint32 i = 0; i < _surfaces.getSize(); i++) {
		ret = _surfaces[i]->restore();
		if (ret != STATUS_OK) {
			BaseEngine::LOG(0, "BaseSurfaceStorage::RestoreAll failed");
			return ret;
		}
	}
	return STATUS_OK;
}


/*
//////////////////////////////////////////////////////////////////////////
bool BaseSurfaceStorage::persist(BasePersistenceManager *persistMgr)
{

	if (!persistMgr->getIsSaving()) cleanup(false);

	persistMgr->transfer(TMEMBER(_gameRef));

	//_surfaces.persist(persistMgr);

	return STATUS_OK;
}
*/


//////////////////////////////////////////////////////////////////////////
bool BaseSurfaceStorage::sortSurfaces() {
	qsort(_surfaces.getData(), _surfaces.getSize(), sizeof(BaseSurface *), surfaceSortCB);
	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////////
int BaseSurfaceStorage::surfaceSortCB(const void *arg1, const void *arg2) {
	void *o1 = const_cast<void *>(arg1);
	void *o2 = const_cast<void *>(arg2);
	BaseSurface *s1 = *((BaseSurface **)o1);
	BaseSurface *s2 = *((BaseSurface **)o2);

	// sort by life time
	if (s1->_lifeTime <= 0 && s2->_lifeTime > 0)
		return 1;
	else if (s1->_lifeTime > 0 && s2->_lifeTime <= 0)
		return -1;


	// sort by validity
	if (s1->_valid && !s2->_valid)
		return -1;
	else if (!s1->_valid && s2->_valid)
		return 1;

	// sort by time
	else if (s1->_lastUsedTime > s2->_lastUsedTime)
		return 1;
	else if (s1->_lastUsedTime < s2->_lastUsedTime)
		return -1;
	else
		return 0;
}

} // End of namespace Wintermute
