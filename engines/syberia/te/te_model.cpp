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
#include "syberia/te/te_light.h"
#include "syberia/te/te_model.h"
#include "syberia/te/te_renderer.h"

namespace Syberia {

TeModel::TeModel() {
}

int TeModel::checkFileType(Common::SeekableReadStream &instream) {
	char buf[4];
	instream.seek(0);
	int sz = instream.read(buf, 4);
	instream.seek(0);
	if (sz == 4 && strncmp("TEMD", buf, 4) == 0) {
		return 1;
	} else if (sz == 4 && strncmp("TEAN", buf, 4) == 0) {
		return 2;
	}
	return 0;
}

void TeModel::blendMesh(const Common::String &s1, const Common::String &s2, float amount) {
	_meshBlenders.push_back(new MeshBlender(s1, s2, amount, this));
}

void TeModel::draw() {
	TeRenderer *renderer = g_engine->getRenderer();
	
	if (worldVisible()) {
		TeMatrix4x4 transform = worldTransformationMatrix();
		renderer->sendModelMatrix(transform);
		renderer->pushMatrix();
		renderer->multiplyMatrix(transform);
		if (!_meshes.empty()) {
			for (TeMesh &mesh : _meshes) {
				// TODO: Set some value in the mesh here to this->field_0x158??
				mesh.draw();
			}
		}
		renderer->popMatrix();
		TeLight::disableAll();
	}
}

void TeModel::setColor(const TeColor &col) {
	Te3DObject2::setColor(col);
	for (TeMesh &mesh : _meshes) {
		mesh.setColor(col);
	}
}

void TeModel::update() {
	error("TODO: Implement me.");
}

TeModel::MeshBlender::MeshBlender(const Common::String &name, const Common::String &meshName, float amount, TeModel *model) :
_name(name), _amount(amount) {
	Common::Array<TeMesh> &meshes = model->_meshes;
	uint i = 0;
	for (; i < meshes.size(); i++) {
		if (meshes[i].name().contains(meshName))
			break;
	}
	_meshNo = i;
	_timer.start();
}

/*static*/ void TeModel::loadAlign(Common::SeekableReadStream &stream) {
	int64 pos = stream.pos();
	if (pos % 4) {
		stream.seek(4 - (pos % 4), SEEK_CUR);
	}
}

/*static*/ void TeModel::saveAlign(Common::SeekableWriteStream &stream) {
	int64 pos = stream.pos();
	if (pos % 4) {
		stream.seek(4 - (pos % 4), SEEK_CUR);
	}
}

} // end namespace Syberia
