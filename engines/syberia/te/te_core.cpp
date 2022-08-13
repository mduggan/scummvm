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

#include "syberia/te/te_core.h"

namespace Syberia {

TeCore::TeCore() : _loc(nullptr) {
}

void TeCore::addLoc(TeILoc *loc) {
	if (_loc) {
		warning("TeCore::addLoc: There is already a loc");
	}
	_loc = loc;
}

const Common::String &TeCore::fileFlagSystemFlag(const Common::String &name) const {
	return _fileSystemFlags.find(name)->_value;
}

void TeCore::fileFlagSystemSetFlag(const Common::String &name, const Common::String &val) {
	// TODO: Impmenent this fully to check possible values
	_fileSystemFlags.setVal(name, val);
}

bool TeCore::fileFlagSystemFlagsContains(const Common::String &name) const {
	// TODO: Implement me
	return false;
}

Common::Array<Common::String> TeCore::fileFlagSystemPossibleFlags() {
	// TODO: Implement me
	return Common::Array<Common::String>();
}

bool TeCore::fileFlagSystemPossibleFlagsContains(const Common::String &name) const {
	// TODO: Implement me
	return false;
}

const Common::String &TeCore::language() const {
	return fileFlagSystemFlag("language");
}

void TeCore::language(const Common::String &val) {
	return fileFlagSystemSetFlag("language", val);
}

} // end namespace Syberia
