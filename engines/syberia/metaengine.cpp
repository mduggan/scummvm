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

#include "syberia/metaengine.h"
#include "syberia/detection.h"
#include "syberia/syberia.h"

const char *SyberiaMetaEngine::getName() const {
	return "syberia";
}

Common::Error SyberiaMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	*engine = new Syberia::SyberiaEngine(syst, desc);
	return Common::kNoError;
}

bool SyberiaMetaEngine::hasFeature(MetaEngineFeature f) const {
	return
		(f == kSavesUseExtendedFormat) ||
		(f == kSimpleSavesNames) ||
	    (f == kSupportsListSaves) ||
	    (f == kSupportsDeleteSave) ||
	    (f == kSavesSupportMetaInfo) ||
	    (f == kSavesSupportThumbnail) ||
	    (f == kSupportsLoadingDuringStartup);
}

#if PLUGIN_ENABLED_DYNAMIC(SYBERIA)
REGISTER_PLUGIN_DYNAMIC(SYBERIA, PLUGIN_TYPE_ENGINE, SyberiaMetaEngine);
#else
REGISTER_PLUGIN_STATIC(SYBERIA, PLUGIN_TYPE_ENGINE, SyberiaMetaEngine);
#endif
