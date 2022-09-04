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

#ifndef SYBERIA_TE_TE_RESOURCE_MANAGER_H
#define SYBERIA_TE_TE_RESOURCE_MANAGER_H

#include "common/array.h"
#include "common/path.h"
#include "common/ptr.h"
#include "common/file.h"
#include "common/textconsole.h"

#include "syberia/syberia.h"
#include "syberia/te/te_resource.h"
#include "syberia/te/te_core.h"

namespace Syberia {

class TeResource;

class TeResourceManager {
public:
	TeResourceManager();
	
	void addResource(TeResource *resource);
	bool exists(const Common::Path &path);
	void removeResource(TeResource *resource);
	
	template<class T> Common::SharedPtr<T> getResource(const Common::Path &path) {
		for (uint i = 0; i < this->_resources.size(); i++) {
			if (_resources[i]->getAccessName() == path) {
				return Common::SharedPtr<T>(static_cast<T *>(this->_resources[i]));
			}
		}
		
		Common::SharedPtr<T> retval;
		TeCore *core = g_engine->getCore();
		if (!core->_coreNotReady) {
			if (Common::File::exists(path)) {
				retval.reset(new T());
			} else {
				// Try path with langs
				Common::Path parentPath = path.getParent();
				Common::Path pathWithLang = parentPath.join(core->language()).joinInPlace(path.getLastComponent());
				if (Common::File::exists(pathWithLang)) {
					retval.reset(new T());
				}
				Common::Path pathWithEn = parentPath.join("en").joinInPlace(path.getLastComponent());
				if (Common::File::exists(pathWithEn)) {
					retval.reset(new T());
				}
			}
		} else {
			retval.reset(new T());
		}
		if (retval.get())
			addResource(retval.get());
		return retval;
	}

private:
	Common::Array<TeResource *> _resources;
	// TODO add private members

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_RESOURCE_MANAGER_H
