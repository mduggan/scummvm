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

#include "common/file.h"
#include "common/zlib.h"
#include "common/substream.h"

#include "syberia/syberia.h"
#include "syberia/te/te_light.h"
#include "syberia/te/te_model.h"
#include "syberia/te/te_renderer.h"
#include "syberia/te/te_trs.h"

namespace Syberia {

TeModel::TeModel() : _enableLights(false), _skipBoneMatricies(false) {
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
		const TeMatrix4x4 transform = transformationMatrix();
		renderer->sendModelMatrix(transform);
		renderer->pushMatrix();
		renderer->multiplyMatrix(transform);
		for (TeMesh &mesh : _meshes) {
			// TODO: Set some value in the mesh here to this->field_0x158??
			mesh.draw();
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

bool TeModel::load(Common::SeekableReadStream &stream) {
	char buf[5];
	buf[4] = '\0';
	stream.read(buf, 4);
	if (strncmp(buf, "TEMD", 4)) {
		error("[TeModel::load] Unknown format.");
	}

	uint version = stream.readUint32LE();
	if (!(version == 11) || (version == 13)) {
		error("[TeModel::load] Unsupported version %d", version);
	}

	_meshes.resize(stream.readUint32LE());
	_weightElements.resize(stream.readUint32LE());
	uint32 bonecount = stream.readUint32LE();
	_bones.resize(bonecount);
	_boneMatrices.resize(bonecount);

	if (version == 13) {
		_skipBoneMatricies = stream.readUint32LE();
	}

	stream.read(buf, 4);
	if (strncmp(buf, "SKEL", 4)) {
		error("[TeModel::load] Unable to find skeleton.");
	}

	for (unsigned int i = 0; i < _bones.size(); i++) {
		_bones[i]._name = Te3DObject2::deserializeString(stream);
		loadAlign(stream);
		_bones[i]._x = stream.readUint32LE();
		TeTRS::deserialize(stream, _bones[i]._trs);
		if (!_skipBoneMatricies) {
			_boneMatrices[i].deserialize(stream);
		}
	}

	for (unsigned int m = 0; m < _meshes.size(); m++) {
		// FIXME: Translation clearly wrong here.. what's the return condition?
		if (!loadMesh(stream, _meshes[m])) {
			error("[TeModel::load] Error on meshes loading.");
		}
	}

	stream.read(buf, 4);
	if (strncmp(buf, "WEIG", 4)) {
		error("[TeModel::load] Unable to load weight.");
	}
	for (unsigned int w = 0; w < _weightElements.size(); w++) {
		for (unsigned int i = 0; i < _weightElements.size(); i++) {
			loadWeights(stream, _weightElements[i]);
		}
	}
	//if (*(long *)(*(long *)&(_bones).field_0x8 + 0x68) != 0) {
	//	return true;
	//}
	_bones.resize(1);
	return true;
}

bool TeModel::load(const Common::Path &path) {
	Common::File modelFile;
	if (!modelFile.open(path)) {
		warning("[TeModel::load] Can't open file : %s.", path.toString().c_str());
		return false;
	}
	char buf[5];
	buf[4] = '\0';
	modelFile.read(buf, 4);
	if (!strncmp(buf, "TEZ0", 4)) {
		byte version = modelFile.readByte();
		if (version != 1) {
			warning("[TeModel::load] invalid version number %d (expect 1)", version);
			return false;
		}
		uint32 compressedSize = modelFile.readUint32LE();
		if (compressedSize > modelFile.size()) {
			warning("[TeModel::load] invalid size %d (file size %d)", compressedSize, (int)modelFile.size());
			return false;
		}
		uint32 uncompressedSize = modelFile.readUint32LE();
		Common::SeekableSubReadStream substream(&modelFile, modelFile.pos(), modelFile.size());
		Common::SeekableReadStream *zlibStream = Common::wrapCompressedReadStream(&substream, uncompressedSize);
		if (!zlibStream)
			return false;
		load(*zlibStream);
		delete zlibStream;
	} else {
		modelFile.seek(0);
		load(modelFile);
	}
	return true;
}

bool TeModel::loadWeights(Common::ReadStream &stream, Common::Array<weightElement> weights) {
	uint32 nweights = stream.readUint32LE();
	weights.resize(nweights);
	for (unsigned int i = 0; i < nweights; i++) {
		weights[i]._w = stream.readFloatLE();
		weights[i]._x = stream.readUint16LE();
		stream.readUint16LE();
	}
	return true;
}

bool TeModel::loadMesh(Common::SeekableReadStream &stream, TeMesh &mesh) {
	char buf[5];
	buf[4] = '\0';
	stream.read(buf, 4);
	if (strncmp(buf, "MESH", 4))
		return false;
	
	uint32 vertcount = stream.readUint32LE();
	uint32 idxcount = stream.readUint32LE();
	uint32 matcount = stream.readUint32LE();
	uint32 matidxcount = stream.readUint32LE();
	mesh.setConf(vertcount, idxcount, TeMesh::MeshMode_TriangleFan, matcount, matidxcount);
	
	uint32 flags = stream.readUint32LE();
	if (flags & 1)
		mesh.setColor(0, TeColor(0xff, 0xff, 0xff, 0xff));
	if (flags & 2)
		mesh.setTextureUV(0, TeVector2f32(0.0f, 0.0f));

	mesh.setName(Te3DObject2::deserializeString(stream));
	loadAlign(stream);

	stream.read(buf, 4);
	if (strncmp(buf, "MTRL", 4))
		return false;
	
	for (unsigned int i = 0; i < mesh.materials().size(); i++) {
		TeMaterial mat;
		TeMaterial::deserialize(stream, mat, _texturePath);
		if (_enableLights)
			mat._enableLights = true;
		mesh.attachMaterial(i, mat);
	}

	stream.read(buf, 4);
	if (strncmp(buf, "VERT", 4))
		return false;

	for (unsigned int i = 0; i < mesh.numVerticies(); i++) {
		TeVector3f32 v;
		TeVector3f32::deserialize(stream, v);
		mesh.setVertex(i, v);
	}
	if (mesh.hasUvs()) {
		for (unsigned int i = 0; i < mesh.numVerticies(); i++) {
			TeVector2f32 v;
			TeVector2f32::deserialize(stream, v);
			mesh.setTextureUV(i, v);
		}
	}

	stream.read(buf, 4);
	if (strncmp(buf, "NORM", 4))
		return false;

	for (unsigned int i = 0; i < mesh.numVerticies(); i++) {
		TeVector3f32 v;
		TeVector3f32::deserialize(stream, v);
		mesh.setNormal(i, v);
	}

	stream.read(buf, 4);
	if (strncmp(buf, "COLS", 4))
		return false;

	for (unsigned int i = 0; i < mesh.numVerticies(); i++) {
		TeColor c;
		c.deserialize(stream);
		mesh.setColor(i, c);
	}

	stream.read(buf, 4);
	if (strncmp(buf, "FCPM", 4))
		return false;

	for (unsigned int i = 0; i < mesh.materials().size(); i++) {
		mesh.facesPerMaterial(i, stream.readUint16LE());
	}

	stream.read(buf, 4);
	if (strncmp(buf, "MTXI", 4))
		return false;

	for (unsigned int i = 0; i < mesh.numVerticies(); i++) {
		mesh.matrixIndex(i, stream.readUint16LE());
	}

	stream.read(buf, 4);
	if (strncmp(buf, "IDXS", 4))
		return false;

	for (unsigned int i = 0; i < mesh.numIndexes(); i++) {
		mesh.setIndex(i, stream.readUint16LE());
	}
	
	loadAlign(stream);
	return true;
}

} // end namespace Syberia
