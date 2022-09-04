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

#include "syberia/game/character.h"

namespace Syberia {

Character::Character() {
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

/*static*/ TeModelAnimation *Character::animCacheLoad(const Common::String &path) {
	// todo: return should be TeIntrusivePtr
	error("TODO: Implement me.");
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
	error("TODO: Implement me.");
}

long Character::getCurrentWalkFiles() {
	error("TODO: Implement me.");
}

bool Character::isFramePassed(uint frameno) {
	error("TODO: Implement me.");
}

bool Character::isWalkEnd() {
	error("TODO: Implement me.");
	return false;
}

bool Character::leftStepFrame(enum Character::WalkPart walkpart) {
	error("TODO: Implement me.");
	return false;
}

bool Character::rightStepFrame(enum Character::WalkPart walkpart) {
	error("TODO: Implement me.");
	return false;
}

bool Character::loadModel(const Common::String &name, bool param_2) {
	error("TODO: Implement me.");
	return false;
}

/*static*/ bool Character::loadSettings(const Common::String &path) {
	error("TODO: Implement me.");
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
	error("TODO: Implement me.");
}

void Character::placeOnCurve(const TeBezierCurve &curve) {
	error("TODO: Implement me.");
}

void Character::removeAnim() {
	error("TODO: Implement me.");
}

void Character::removeFromCurve() {
	error("TODO: Implement me.");
}

bool Character::setAnimation(const Common::String &param_1, bool param_2, bool param_3, bool param_4, int param_5, int param_6)  {
	error("TODO: Implement me.");
}

void Character::setAnimationSound(const Common::String &name, uint param_2)  {
	error("TODO: Implement me.");
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

Character::WalkPart Character::walkAnim(uint offset)  {
	error("TODO: Implement me.");
	return WalkPart0;
}

void Character::walkMode(const Common::String &mode) {
	error("TODO: Implement me.");
}
	
void Character::walkTo(float param_1, bool param_2) {
	error("TODO: Implement me.");
}

} // end namespace Syberia
