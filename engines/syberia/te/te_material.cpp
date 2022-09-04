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

#include "common/textconsole.h"
#include "syberia/te/te_material.h"
#include "syberia/te/te_model.h"

namespace Syberia {

TeMaterial::TeMaterial() {
	defaultValues();
	_mode = Mode1;
}

TeMaterial::TeMaterial(Common::SharedPtr<Te3DTexture> texture, Mode mode) {
	defaultValues();
	_texture = texture;
	_mode = mode;
}

void TeMaterial::apply() {
	error("TODO: implement me.");
}

void TeMaterial::defaultValues() {
	_ambientColor = TeColor(0, 0, 0, 255);
	_diffuseColor = TeColor(255, 255, 255, 255);
	_specularColor = TeColor(0, 0, 0, 255);
	_emissionColor = TeColor(0, 0, 0, 255);
	_shininess = 0.0;
	_enableLights = false;
}

/*static*/ void TeMaterial::deserialize(Common::SeekableReadStream &stream, TeMaterial &material, const Common::Path &texPath) {
	uint32 nameLen = stream.readUint32LE();
	char *buf = (char *)malloc(nameLen + 1);
	stream.read(buf, nameLen);
	Common::String nameStr(buf, nameLen);
	delete buf;

	TeModel::loadAlign(stream);
	material._mode = static_cast<TeMaterial::Mode>(stream.readUint32LE());

	if (nameStr.size()) {
		Common::Path fullTexPath = texPath.join(nameStr);
		material._texture = Te3DTexture::load2(fullTexPath, 0x500);
	}

	material._ambientColor.deserialize(stream);
	material._diffuseColor.deserialize(stream);
	material._specularColor.deserialize(stream);
	material._emissionColor.deserialize(stream);
	material._shininess = stream.readFloatLE();
}

/*static*/ void TeMaterial::serialize(Common::SeekableWriteStream &stream, TeMaterial &material) {
	Te3DTexture *tex = material._texture.get();
	Common::String texName;
	if (tex) {
		texName = tex->getAccessName().toString();
		// "Remove extension" twice for some reason..
		size_t offset = texName.rfind('.');
		if (offset != Common::String::npos) {
			texName.substr(0, offset);
		}
		offset = texName.rfind('.');
		if (offset != Common::String::npos) {
			texName.substr(0, offset);
		}
	}
	stream.writeUint32LE(texName.size());
	stream.write(texName.c_str(), texName.size());
	TeModel::saveAlign(stream);
	stream.writeUint32LE(static_cast<uint32>(material._mode));
	material._ambientColor.serialize(stream);
	material._diffuseColor.serialize(stream);
	material._specularColor.serialize(stream);
	material._emissionColor.serialize(stream);
	stream.writeFloatLE(material._shininess);
}

} // end namespace Syberia
