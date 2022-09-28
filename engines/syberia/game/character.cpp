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

#include "common/hashmap.h"
#include "common/path.h"
#include "common/file.h"
#include "common/debug.h"

#include "syberia/game/character.h"
#include "syberia/game/character_settings_xml_parser.h"
#include "syberia/te/te_model_animation.h"

namespace Syberia {

/*static*/ Common::HashMap<Common::String, Character::CharacterSettings> *Character::_globalCharacterSettings = nullptr;

void Character::CharacterSettings::clear() {
	_name.clear();
	_modelFileName.clear();
	_defaultScale = TeVector3f32();
	_walkFileName.clear();
	_walkSettings.clear();
	_walkSpeed = 0.0f;
	_cutSceneCurveDemiPosition = TeVector3f32();
	_defaultEyes.clear();
	_defaultMouth.clear();
	_defaultBody.clear();
}

void Character::WalkSettings::clear() {
	for (int i = 0; i < 4; i++) {
		_walkParts[i] = AnimSettings();
	}
}

Character::Character() : _curveOffset(0) {
}

void Character::addCallback(const Common::String &s1, const Common::String &s2, float f1, float f2) {
	error("TODO: Implement me.");
}

/*static*/ void Character::animCacheFreeAll() {
	error("TODO: Implement me.");
}

/*static*/ void Character::animCacheFreeOldest() {
	error("TODO: Implement me.");
}

/*static*/ TeIntrusivePtr<TeModelAnimation> Character::animCacheLoad(const Common::Path &path) {
	static Common::HashMap<Common::String, TeIntrusivePtr<TeModelAnimation>> _cache;
	
	const Common::String pathStr = path.toString();
	if (_cache.contains(pathStr))
		return _cache.getVal(pathStr);
	
	TeIntrusivePtr<TeModelAnimation> modelAnim = new TeModelAnimation();
	modelAnim->load(path);
	
	_cache.setVal(pathStr, modelAnim);
	return modelAnim;
}

float Character::animLength(TeModelAnimation *modelanim, long bone, long lastframe) {
	error("TODO: Implement me.");
}

float Character::animLengthFromFile(const Common::String &animname, uint *param_2, uint lastframe) {
	error("TODO: Implement me.");
}

bool Character::blendAnimation(const Common::String &animname, float param_2, bool param_3, bool param_4) {
	error("TODO: Implement me.");
}

TeVector3f32 Character::correctPosition(const TeVector3f32 &pos) {
	error("TODO: Implement me.");
}

float Character::curveOffset() {
	return _curveOffset;
}

void Character::deleteAllCallback() {
	error("TODO: Implement me.");
}

void Character::deleteAnim() {
	error("TODO: Implement me.");
}

void Character::deleteCallback(const Common::String &str1, const Common::String &str2, float f) {
	error("TODO: Implement me.");
}

//static bool deserialize(TiXmlElement *param_1, Walk *param_2);
void Character::endMove() {
	error("TODO: Implement Character::endMove.");
}

const Character::WalkSettings *Character::getCurrentWalkFiles() {
	for (const auto & walkSettings : _characterSettings._walkSettings) {
		if (walkSettings._key == _walkModeStr)
			return &walkSettings._value;
	}
	return nullptr;
}

bool Character::isFramePassed(uint frameno) {
	error("TODO: Implement Character::isFramePassed.");
}

bool Character::isWalkEnd() {
	error("TODO: Implement Character::isWalkEnd.");
	return false;
}

bool Character::leftStepFrame(enum Character::WalkPart walkpart) {
	error("TODO: Implement Character::leftStepFrame.");
	return false;
}

bool Character::rightStepFrame(enum Character::WalkPart walkpart) {
	error("TODO: Implement Character::rightStepFrame.");
	return false;
}

bool Character::loadModel(const Common::String &name, bool param_2) {
	assert(_globalCharacterSettings);
	if (_model) {
		//_model->bonesUpdateSignal().remove(this, &Character::onBonesUpdate);
	}
	_model = new TeModel();
	//_model->bonesUpdateSignal().add(this, &Character::onBonesUpdate);

	if (!_globalCharacterSettings->contains(name))
		return false;

	_characterSettings = _globalCharacterSettings->getVal(name);
	_model->_texturePath = Common::Path("models/Textures");
	_model->_enableLights = true;
	Common::Path modelPath("models");
	modelPath.joinInPlace(_characterSettings._modelFileName);
	if (!_model->load(modelPath))
		return false;

	_model->setName(name);
	_model->setScale(_characterSettings._defaultScale);

	for (unsigned int i = 0; i < _model->_meshes.size(); i++)
		_model->_meshes[i].setVisible(true);

	_model->setVisibleByName("_B_", false);
	_model->setVisibleByName("_Y_", false);

	_model->setVisibleByName(_characterSettings._defaultEyes, true);
	_model->setVisibleByName(_characterSettings._defaultMouth, true);

	setAnimation(_characterSettings._walkFileName, true, false, false, -1, 9999);

	_walkPart0AnimLen = animLengthFromFile(walkAnim(WalkPart_Start), &_walkPart0AnimFrameCount, 9999);
	_walkPart3AnimLen = animLengthFromFile(walkAnim(WalkPart_EndG), &_walkPart3AnimFrameCount, 9999);
	_walkPart1AnimLen = animLengthFromFile(walkAnim(WalkPart_Loop), &_walkPart1AnimFrameCount, 9999);

	TeIntrusivePtr<Te3DTexture> shadow = new Te3DTexture();
	shadow->load("models/Textures/simple_shadow_alpha.tga");

	for (int i = 0; i < 2; i++) {
		TeModel *pmodel = new TeModel();
		_shadowModel[i] = pmodel;
		pmodel->setName("Shadow");
		Common::Array<TeVector3f32> arr;
		arr.resize(4);
		arr[0] = TeVector3f32(-60.0, 0.0, -60.0);
		arr[1] = TeVector3f32(-60.0, 0.0, 60.0);
		arr[2] = TeVector3f32(60.0, 0.0, -60.0);
		arr[3] = TeVector3f32(60.0, 0.0, 60.0);
		pmodel->setQuad(shadow, arr, TeColor(0xff,0xff,0xff,0x50));
	}
	return true;
}

/*static*/ bool Character::loadSettings(const Common::String &path) {
	CharacterSettingsXmlParser parser;
	parser.setAllowText();
	if (_globalCharacterSettings)
		delete _globalCharacterSettings;
	_globalCharacterSettings = new Common::HashMap<Common::String, CharacterSettings>();
	parser.setCharacterSettings(_globalCharacterSettings);

	// WORKAROUND: This file contains invalid comments
	// eg, <!--------- and a comment-inside-a-comment.
	// patch them before parsing.
	Common::File xmlFile;
	if (!xmlFile.open(path))
		error("Character::loadSettings: Can't open %s", path.c_str());
	const uint32 bufsize = xmlFile.size();
	char *buf = new char[bufsize+1];
	buf[bufsize] = '\0';
	xmlFile.read(buf, bufsize);
	Common::String fixedbuf(buf);
	uint32 offset = fixedbuf.find("------------");
	while (offset != Common::String::npos) {
		fixedbuf.replace(offset, 12, "--");
		offset = fixedbuf.find("------------");
	}

	// Big HACK: Remove the embedded comment in this config.
	offset = fixedbuf.find("<!--<walk>");
	if (offset != Common::String::npos) {
		uint32 endOffset = fixedbuf.find(" -->", offset);
		if (endOffset != Common::String::npos) {
			uint32 realEndOffset = fixedbuf.find("walk>-->", endOffset);
			if (realEndOffset  != Common::String::npos && realEndOffset > endOffset) {
				fixedbuf.replace(offset, endOffset - offset, "<!-- ");
			}
		}
	}

	if (!parser.loadBuffer((unsigned char *)fixedbuf.c_str(), bufsize))
		error("Character::loadSettings: Can't open %s", path.c_str());

	if (!parser.parse())
		error("Character::loadSettings: Can't parse %s", path.c_str());

	return false;
}

bool Character::onBonesUpdate(const Common::String &param_1, const TeMatrix4x4 *param_2) {
	error("TODO: Implement me.");
	return false;
}

bool Character::onModelAnimationFinished() {
	error("TODO: Implement me.");
	return false;
}

void Character::permanentUpdate() {
	error("TODO: Implement Character::permanentUpdate.");
}

void Character::placeOnCurve(const TeBezierCurve &curve) {
	error("TODO: Implement Character::placeOnCurve.");
}

void Character::removeAnim() {
	error("TODO: Implement Character::removeAnim.");
}

void Character::removeFromCurve() {
	error("TODO: Implement Character::removeFromCurve.");
}

bool Character::setAnimation(const Common::String &name, bool repeat, bool param_3, bool param_4, int startFrame, int endFrame)  {
	if (name.empty())
		return false;
	
	Common::Path animPath("models/Anims");
	animPath.joinInPlace(name);
	bool validAnim = (name.contains(_characterSettings._walkFileName) ||
					  name.contains(walkAnim(WalkPart_Start)) ||
					  name.contains(walkAnim(WalkPart_Loop)) ||
					  name.contains(walkAnim(WalkPart_EndD)) ||
					  name.contains(walkAnim(WalkPart_EndG)));

	
	if (_lastModelAnim) {
		_lastModelAnim->onFinished().remove(this, &Character::onModelAnimationFinished);
		_lastModelAnim->unbind();
	}
	
	_lastModelAnim = animCacheLoad(animPath);
	_lastModelAnim->bind(_model);
	_lastModelAnim->setFrameLimits(startFrame, endFrame);
	_model->setAnim(_lastModelAnim, repeat);
	error("TODO: Finish Character::setAnimation.");
}

void Character::setAnimationSound(const Common::String &name, uint param_2)  {
	error("TODO: Implement Character::setAnimationSound.");
}

void Character::setCurveOffset(float offset) {
	_curveOffset = offset;
	updatePosition(offset);
}

void Character::setFreeMoveZone(const Common::SharedPtr<TeFreeMoveZone> &zone) {
	_freeMoveZone = zone;
}

void Character::setStepSound(const Common::String &stepSound1, const Common::String &stepSound2) {
	_stepSound1 = stepSound1;
	_stepSound2	= stepSound2;
}

bool Character::setShadowVisible(bool visible)  {
	error("TODO: Implement me.");
	return false;
}

float Character::speedFromAnim(double movepercent)  {
	error("TODO: Implement me.");
	return 0;
}

float Character::translationFromAnim(const TeModelAnimation &anim, long bone, long param_3) {
	return translationVectorFromAnim(anim, bone, param_3).z();
}

TeVector3f32 Character::translationVectorFromAnim(const TeModelAnimation &anim, long bone, long frame)  {
	error("TODO: Implement me.");
	return TeVector3f32();
}

TeTRS Character::trsFromAnim(const TeModelAnimation &anim, long bone, long frame)  {
	error("TODO: Implement me.");
	return TeTRS();
}

void Character::update(double percentval)  {
	error("TODO: Implement me.");
}

void Character::updateAnimFrame()  {
	error("TODO: Implement me.");
}

void Character::updatePosition(float curveOffset) {
	error("TODO: Implement me.");
}

Common::String Character::walkAnim(Character::WalkPart part)  {
	Common::String result;
	const Character::WalkSettings *settings = getCurrentWalkFiles();
	if (settings) {
		return settings->_walkParts[(int)part]._file;
	}
	return result;
}

void Character::walkMode(const Common::String &mode) {
	error("TODO: Implement me.");
}

void Character::walkTo(float param_1, bool param_2) {
	error("TODO: Implement me.");
}

} // end namespace Syberia
