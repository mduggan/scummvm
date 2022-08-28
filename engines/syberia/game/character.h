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

#ifndef SYBERIA_GAME_CHARACTER_H
#define SYBERIA_GAME_CHARACTER_H

#include "common/array.h"
#include "common/str.h"
#include "common/types.h"
#include "syberia/te/te_animation.h"
#include "syberia/te/te_model_animation.h"
#include "syberia/te/te_vector3f32.h"
#include "syberia/te/te_matrix4x4.h"
#include "syberia/te/te_model.h"
#include "syberia/te/te_bezier_curve.h"
#include "syberia/te/te_free_move_zone.h"
#include "syberia/te/te_trs.h"

namespace Syberia {

class Character : public TeAnimation {
public:
	Character();

	class CharacterSettings {};
	class AnimCacheElement {};
	enum WalkPart {
		WalkPart0,
		WalkPart1,
		WalkPart2,
		WalkPart3
	};
	
	void addCallback(const Common::String &s1, const Common::String &s2, float f1, float f2);

	static void animCacheFreeAll();
	static void animCacheFreeOldest();
	static TeModelAnimation *animCacheLoad(const Common::String &path); // todo: should be TeIntrusivePtr
	
	float animLength(TeModelAnimation *modelanim, long bone, long lastframe);
	float animLengthFromFile(const Common::String &animname, uint *param_2, uint lastframe);
	bool blendAnimation(const Common::String &animname, float param_2, bool param_3, bool param_4);
	TeVector3f32 correctPosition(const TeVector3f32 &pos);
	float curveOffset();
	void deleteAllCallback();
	void deleteAnim();
	void deleteCallback(const Common::String &str1, const Common::String &str2, float f);
	//static bool deserialize(TiXmlElement *param_1, Walk *param_2);
	void endMove();
	
	long getCurrentWalkFiles();
	bool isFramePassed(uint frameno);
	bool isWalkEnd();
	bool leftStepFrame(enum WalkPart walkpart);
	bool rightStepFrame(enum WalkPart walkpart);
	bool loadModel(const Common::String &name, bool param_2);
	static bool loadSettings(const Common::String &path);
	
	bool onBonesUpdate(const Common::String &param_1, const TeMatrix4x4 *param_2);
	bool onModelAnimationFinished();
	void permanentUpdate();
	void placeOnCurve(const TeBezierCurve &curve);
	//void play() // just called TeAnimation::play();
	void removeAnim();
	void removeFromCurve();
	static Common::String rootBone() { return "Pere"; }
	
	bool setAnimation(const Common::String &param_1, bool param_2, bool param_3, bool param_4, int param_5, int param_6);
	void setAnimationSound(const Common::String &name, uint param_2);
	void setCurveOffset(float offset);
	void setFreeMoveZone(const TeFreeMoveZone &zone);
	bool setShadowVisible(bool visible);
	void setStepSound(const Common::String &stepSound1, const Common::String &stepSound2);
	float speedFromAnim(double movepercent);
	//void stop(); // just maps to TeAnimation::stop();
	float translationFromAnim(const TeModelAnimation &anim, long bone, long frame);
	TeVector3f32 translationVectorFromAnim(const TeModelAnimation &anim, long bone, long frame);
	TeTRS trsFromAnim(const TeModelAnimation &anim, long bone, long frame);
	void update(double percentval);
	void updateAnimFrame();
	void updatePosition(float curveOffset);
	WalkPart walkAnim(uint offset);
	void walkMode(const Common::String &mode);
	void walkTo(float param_1, bool param_2);

	TeModel *_model; //TODO: should be a smart pointer?
	
private:
	float _curveOffset;
	TeBezierCurve _curve;
	TeFreeMoveZone _freeMoveZone;
	Common::String _stepSound1;
	Common::String _stepSound2;
	Common::String _walkModeStr; // Walk or Jog

	static Common::Array<AnimCacheElement> _animCache;
	static uint _animCacheSize;
	static Common::Array<CharacterSettings> _characterSettings;
};

} // end namespace Syberia

#endif // SYBERIA_GAME_CHARACTER_H
