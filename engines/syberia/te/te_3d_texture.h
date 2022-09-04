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

#ifndef SYBERIA_TE_TE_3D_TEXTURE_H
#define SYBERIA_TE_TE_3D_TEXTURE_H

#include "common/path.h"
#include "common/ptr.h"
#include "common/str.h"
#include "syberia/te/te_resource.h"

namespace Syberia {

class Te3DTexture : public TeResource {
public:
	Te3DTexture();

	static Common::SharedPtr<Te3DTexture> load2(const Common::Path &path, uint size);
	
	void load(const Common::Path &path);
	// TODO add public members

private:
	// TODO add private members

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_3D_TEXTURE_H
