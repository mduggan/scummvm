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

#include "syberia/te/te_resource.h"
#include "syberia/te/te_resource_manager.h"

namespace Syberia {

TeResourceManager::TeResourceManager() {
}

void TeResourceManager::addResource(TeResource *resource) {
	_resources.insert_at(0, resource);
}

bool TeResourceManager::exists(const Common::Path &path) {
	Common::String pathstr = path.toString();
	for (auto &resource : _resources) {
		if (resource->getAccessName() == pathstr)
			return true;
	}
	return false;
}

void TeResourceManager::removeResource(TeResource *resource) {
	for (uint i = 0; i < _resources.size(); i++) {
		if (_resources[i] == resource) {
			_resources.remove_at(i);
			break;
		}
	}
}

// TODO: Add more functions here.

} // end namespace Syberia
