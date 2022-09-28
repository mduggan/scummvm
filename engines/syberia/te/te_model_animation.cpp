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

#include "common/util.h"
#include "common/file.h"
#include "common/stream.h"
#include "common/substream.h"
#include "common/zlib.h"

#include "syberia/te/te_model_animation.h"

namespace Syberia {

TeModelAnimation::TeModelAnimation() : _firstFrame(-1), _lastFrame(SHRT_MAX),
_curFrame(0), _curFrameValFresh(false), _repeatNum(0), _finishedSignalPending(false),
_curFrame2(0), _useNMOArrays(0), _speed(0.0f) {
}

//void *TeModelAnimation::calcCurrentFrame(double proportion);

void TeModelAnimation::cont() {
	_repeatNum = 0;
	TeAnimation::cont();
}

void TeModelAnimation::destroy() {
	_loadedPath = Common::Path();
	_curFrame2 = 0;
	_repeatNum = 0;
	_boneNames.clear();
	_nmoTransArrays.clear();
	_nmoRotArrays.clear();
	_nmoScaleArrays.clear();
	_fbxArrays.clear();
}

int TeModelAnimation::findBone(const Common::String &name) {
	for (unsigned int i = 0; i < _boneNames.size(); i++) {
		if (_boneNames[i] == name)
			return i;
	}
	return -1;
}

int TeModelAnimation::firstFrame() {
	if (_firstFrame < 0)
		return 0;
	return _firstFrame;
}

//TeMatrix4x4 TeModelAnimation::getMatrix(const Common::String &name, unsigned long frame, bool param_5);

//TeQuaternion TeModelAnimation::getNMORotation(unsigned long param_3, float param_4);

//TeVector3f32 TeModelAnimation::getNMOTranslation(unsigned long param_3, float param_4);

//TeTRS TeModelAnimation::getTRS(const Common::String &boneName, unsigned long frame, bool param_5);

//TeTRS TeModelAnimation::getTRS(unsigned long boneNo, unsigned long frame, bool param_5);

int TeModelAnimation::lastFrame() {
	int result;
	if (!_useNMOArrays) {
		if (_fbxArrays.empty())
			result = 0;
		result = _fbxArrays[0].size();
	} else {
		result = _numNMOFrames;
	}
	return MIN(_lastFrame, result);
}

bool TeModelAnimation::load(const Common::Path &path) {
	Common::File modelFile;
	if (!modelFile.open(path)) {
		warning("[TeModel::load] Can't open file : %s.", path.toString().c_str());
		return false;
	}
	bool retval;
	if (TeModel::loadAndCheckString(modelFile, "TEZ0")) {
		Common::SeekableReadStream *zlibStream = TeModel::tryLoadZlibStream(modelFile);
		if (!zlibStream)
			return false;
		retval = load(*zlibStream);
		delete zlibStream;
	} else {
		modelFile.seek(0);
		retval = load(modelFile);
	}
	_loadedPath = path;
	return retval;
}

bool TeModelAnimation::load(Common::SeekableReadStream &stream) {
	if (!TeModel::loadAndCheckString(stream, "TEAN")) {
		warning("[TeModelAnimation::load] Unknown format.");
		return false;
	}
	uint32 version = stream.readUint32LE();
	if (version != 3) {
		warning("[TeModelAnimation::load] Unsupported version %d (expect 3)", version);
		return false;
	}
	
	_useNMOArrays = stream.readUint32LE();
	_numNMOFrames = stream.readUint32LE();
	uint32 numBones = stream.readUint32LE();
	
	if (_useNMOArrays == 0) {
		_fbxArrays.resize(numBones);
	} else {
		_nmoTransArrays.resize(numBones);
		_nmoRotArrays.resize(numBones);
		_nmoScaleArrays.resize(numBones);
	}
	
	_speed = stream.readFloatLE();
	for (unsigned int i = 0; i < numBones; i++) {
		if (!TeModel::loadAndCheckString(stream, "BONE"))
			return false;
		const Common::String boneName = Te3DObject2::deserializeString(stream);
		setBoneName(i, boneName);
		if (!TeModel::loadAndCheckString(stream, "BTRA"))
			return false;
		uint32 numTrans = stream.readUint32LE();
		for (unsigned int j = 0; j < numTrans; j++) {
			float f = stream.readFloatLE();
			TeVector3f32 trans;
			TeVector3f32::deserialize(stream, trans);
			setTranslation(j, f, trans);
		}
		if (!TeModel::loadAndCheckString(stream, "BROT"))
			return false;
		uint32 numRots = stream.readUint32LE();
		for (unsigned int j = 0; j < numRots; j++) {
			float f = stream.readFloatLE();
			TeQuaternion rot;
			TeQuaternion::deserialize(stream, rot);
			setRotation(j, f, rot);
		}
	}
	return true;
}

int TeModelAnimation::nbFrames() {
	if (!_useNMOArrays) {
		if (_fbxArrays.empty())
			return 0;
		return _fbxArrays[0].size();
	} else {
		return _numNMOFrames;
	}
}

void TeModelAnimation::reset() {
	TeAnimation::reset();
	_finishedSignalPending = false;
	_curFrame = 0;
	_curFrame2 = 0;
	_firstFrame = -1;
	_lastFrame = SHRT_MAX;
}

void TeModelAnimation::resizeFBXArrays(unsigned long len) {
	_fbxArrays.resize(len);
}

void TeModelAnimation::resizeNMOArrays(unsigned long len) {
	_nmoTransArrays.resize(len);
	_nmoRotArrays.resize(len);
	_nmoScaleArrays.resize(len);
}

//void TeModelAnimation::save(Common::SeekableWriteStream &stream) {

//void TeModelAnimation::saveBone(Common::SeekableWriteStream &stream, uint param_2);

void TeModelAnimation::setBoneName(uint boneNo, const Common::String &name) {
	if (_boneNames.size() < boneNo + 1) {
		_boneNames.resize(boneNo + 1);
	}
	_boneNames[boneNo] = name;
}

void TeModelAnimation::setRotation(unsigned long num, float amount, const TeQuaternion &rot) {
	if (!_useNMOArrays) {
		_fbxArrays[num][(int)amount].setRotation(rot);
	} else {
		NMORotation nmorot;
		nmorot._rot = rot;
		nmorot._f = amount;
		_nmoRotArrays[num].push_back(nmorot);
	}
}

void TeModelAnimation::setScale(unsigned long num, float amount, const TeVector3f32 &scale) {
	// NOTE: This isn't actually implemented in the original.
	error("TeModelAnimation::setScale not implemented in original game.");
}

void TeModelAnimation::setTranslation(unsigned long num, float amount, const TeVector3f32 &trans) {
	if (!_useNMOArrays) {
		_fbxArrays[num][(int)amount].setTranslation(trans);
	} else {
		NMOTranslation nmotrans;
		nmotrans._trans = trans;
		nmotrans._f = amount;
		_nmoTransArrays[num].push_back(nmotrans);
	}
}

void TeModelAnimation::unbind() {
	_model.release();
}

void TeModelAnimation::update(double proportion) {
	error("TODO: Implement TeModelAnimation::update");
}

} // end namespace Syberia
