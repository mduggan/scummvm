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
#include "syberia/te/te_intrusive_ptr.h"

namespace Syberia {

class TeResource;

class TeResourceManager {
public:
	TeResourceManager();

	~TeResourceManager();

	void addResource(const TeIntrusivePtr<TeResource> &resource);
	void addResource(TeResource *resource);
	bool exists(const Common::Path &path);
	void removeResource(const TeIntrusivePtr<TeResource> &resource);
	void removeResource(const TeResource *resource);
	
	template<class T> TeIntrusivePtr<T> getResource(const Common::Path &path) {
		for (TeIntrusivePtr<TeResource> &resource : this->_resources) {
			if (resource->getAccessName() == path) {
				return TeIntrusivePtr<T>(dynamic_cast<T *>(resource.get()));
			}
		}
		
		TeIntrusivePtr<T> retval;
		TeCore *core = g_engine->getCore();
		if (!core->_coreNotReady) {
			if (Common::File::exists(path)) {
				retval = new T();
			} else {
				// Try path with langs
				Common::Path parentPath = path.getParent();
				Common::Path pathWithLang = parentPath.join(core->language()).joinInPlace(path.getLastComponent());
				if (Common::File::exists(pathWithLang)) {
					retval = new T();
				}
				Common::Path pathWithEn = parentPath.join("en").joinInPlace(path.getLastComponent());
				if (Common::File::exists(pathWithEn)) {
					retval = new T();
				}
			}
		} else {
			retval = new T();
		}
		if (retval.get())
			addResource(retval.get());
		return retval;
	}

private:
	Common::Array<TeIntrusivePtr<TeResource>> _resources;

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_RESOURCE_MANAGER_H
