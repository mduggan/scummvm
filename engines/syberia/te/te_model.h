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

#ifndef SYBERIA_TE_TE_MODEL_H
#define SYBERIA_TE_TE_MODEL_H

#include "common/array.h"
#include "common/ptr.h"
#include "common/stream.h"
#include "syberia/te/te_timer.h"
#include "syberia/te/te_mesh.h"
#include "syberia/te/te_model_animation.h"

namespace Syberia {

class TeModelAnimation;

class TeModel : public Te3DObject2 {
public:
	TeModel();

	class BonesBlender {
	public:
		BonesBlender(TeModelAnimation *modelanim, float blend, TeModel *model);
		float coef();

		TeTimer _timer;
	};

	class MeshBlender {
	public:
		MeshBlender(const Common::String &s1, const Common::String &s2, float amount, TeModel *model);
	private:
		Common::String _name;
		uint _meshNo;
		float _amount;
		TeTimer _timer;
	};

	void addMesh(const TeMesh &mesh) {
		_meshes.push_back(mesh);
	}
	long anim(); // TODO: Probably wrong type here.
	void blendAnim(Common::SharedPtr<TeModelAnimation>& anim, float amount, bool flag);
	void blendMesh(const Common::String &s1, const Common::String &s2, float amount);

	int checkFileType(Common::SeekableReadStream &instream);

	/* Align the stream to the nearest 4 byte boudary*/
	static void loadAlign(Common::SeekableReadStream &stream);
	static void saveAlign(Common::SeekableWriteStream &stream);

	void update();
	bool visible();
	void setVisible(bool vis);

private:
	Common::Array<TeMesh> _meshes;
	Common::Array<MeshBlender *> _meshBlenders;
	// TODO add private members

};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_MODEL_H
