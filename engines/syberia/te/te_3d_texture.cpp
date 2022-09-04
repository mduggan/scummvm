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

#include "syberia/syberia.h"
#include "syberia/te/te_3d_texture.h"
#include "syberia/te/te_resource_manager.h"

namespace Syberia {

Te3DTexture::Te3DTexture() {
}

/*static*/ Common::SharedPtr<Te3DTexture> Te3DTexture::load2(const Common::Path &path, uint size) {
	Common::Path fullPath = path.append(".3dtex");
	
	TeResourceManager *resMgr = g_engine->getResourceManager();
	if (!resMgr->exists(fullPath)) {
		Common::SharedPtr<Te3DTexture> retval(new Te3DTexture());
		retval->load(fullPath);
		retval->setAccessName(fullPath);
		resMgr->addResource(retval.get());
		return retval;
	} else {
		return resMgr->getResource<Te3DTexture>(fullPath);
	}
}

void Te3DTexture::load(const Common::Path &path) {
	error("TODO: Implement me");
}


// TODO: Add more functions here.

} // end namespace Syberia
