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

#ifndef SYBERIA_TE_TE_CORE_H
#define SYBERIA_TE_TE_CORE_H

#include "common/array.h"
#include "common/str.h"
#include "common/hashmap.h"
#include "common/hash-str.h"

#include "syberia/te/te_i_loc.h"
#include "syberia/te/te_timer.h"

namespace Syberia {

class TeCore {
public:
	TeCore();

	void addLoc(TeILoc *loc);
	//void args(int argc, char **argv); // Probably not needed
	void create();
	void createVideoCodec(const Common::String &path);
	const Common::String &fileFlagSystemFlag(const Common::String &name) const;
	bool fileFlagSystemFlagsContains(const Common::String &name) const;
	Common::Array<Common::String> fileFlagSystemPossibleFlags();
	bool fileFlagSystemPossibleFlagsContains(const Common::String &name) const;
	void fileFlagSystemSetFlag(const Common::String &name, const Common::String &val);
	void fileFlagSystemSetPossibleFlags(const Common::String &name, const Common::Array<Common::String> &vals);
	
	const Common::String &language() const;
	void language(const Common::String &val);
	
	void onActivityTrackingAlarm();
	void enableActivityTracking(bool enable);
	void setActivityTrackingFolder(const Common::String &path);

	bool _coreNotReady;
private:
	TeILoc *_loc;

	Common::HashMap<Common::String, Common::String, Common::CaseSensitiveString_Hash, Common::CaseSensitiveString_EqualTo> _fileSystemFlags;

	TeTimer _activityTrackingTimer;
};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_CORE_H
