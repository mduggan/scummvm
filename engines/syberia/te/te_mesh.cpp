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

#include "graphics/opengl/glad.h"

#include "syberia/syberia.h"
#include "syberia/te/te_renderer.h"
#include "syberia/te/te_light.h"
#include "syberia/te/te_mesh.h"
#include "syberia/te/te_material.h"

namespace Syberia {

TeMesh::TeMesh() : _matrixForced(false), _mode(MeshMode0),
_hasAlpha(false), _glTexEnvMode(GL_MODULATE), _initialMaterialIndexCount(0),
_drawWires(false), _shouldDrawMaybe(true) {
}


void TeMesh::defaultMaterial(const TeIntrusivePtr<Te3DTexture> &texture) {
	TeMaterial::Mode mode = TeMaterial::Mode1;
	if (texture && !texture->hasAlpha())
		mode = TeMaterial::Mode0;

	_materials.resize(1);
	_materials[0] = TeMaterial(texture, mode);
}

const TeMaterial *TeMesh::material(uint index) const {
	if (index < _materials.size()) {
		return &_materials[index];
	} else {
		return &_materials[0];
	}
}


void TeMesh::destroy() {
	_hasAlpha = false;
	_updatedVerticies.clear();
	_updatedNormals.clear();
	_verticies.clear();
	_normals.clear();
	_uvs.clear();
	_colors.clear();
	_indexes.clear();
	_materialIndexes.clear();
	_faceCounts.clear();
	_matricies.clear();
}

void TeMesh::draw() {
	TeRenderer *renderer = g_engine->getRenderer();
	renderer->pushMatrix();
	if (_matrixForced)
		renderer->multiplyMatrix(_forceMatrix);
	else
		renderer->multiplyMatrix(transformationMatrix());
	
	Common::Array<TeVector3f32> &normals = (_updatedVerticies.empty() ? _normals : _updatedNormals);
	Common::Array<TeVector3f32> &verticies = (_updatedVerticies.empty() ? _verticies : _updatedVerticies);
	if (renderer->shadowMode() != TeRenderer::ShadowMode1) {
		if (_faceCounts.empty()) {
			if (hasAlpha(0) && _shouldDrawMaybe) {
				renderer->addTransparentMesh(*this, 0, 0, 0);
				renderer->popMatrix();
				return;
			}
		} else if (!_materials.empty()) {
			for (unsigned int i = 0; i < _faceCounts.size(); i++) {
				int totalFaceCount = 0;
				if (_faceCounts[i]) {
					if (hasAlpha(i)) {
						renderer->addTransparentMesh(*this, totalFaceCount, _faceCounts[i], i);
					}
					totalFaceCount += _faceCounts[i];
				}
			}
		}
	}

	renderer->setMatrixMode(TeRenderer::MM_GL_MODELVIEW);
	renderer->pushMatrix();
	renderer->loadMatrixToGL(renderer->currentMatrix());
	glEnableClientState(GL_VERTEX_ARRAY);
	if (!normals.empty()) {
		glEnableClientState(GL_NORMAL_ARRAY);
	}
	if (!_colors.empty()) {
		glEnableClientState(GL_COLOR_ARRAY);
	}
	glVertexPointer(3, GL_FLOAT, 12, verticies.data());
	if (!normals.empty()) {
		glNormalPointer(GL_FLOAT, 12, normals.data());
	}
	if (!_uvs.empty() && renderer->shadowMode() != TeRenderer::ShadowMode2) {
		glTexCoordPointer(2, GL_FLOAT, 8, _uvs.data());
	}
	if (!_colors.empty()) {
		glColorPointer(4, GL_UNSIGNED_BYTE,4, _colors.data());
	}
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, _glTexEnvMode);
	if (renderer->scissorEnabled()) {
		glEnable(GL_SCISSOR_TEST);
		uint scissorx = renderer->scissorX();
		uint scissory = renderer->scissorY();
		uint scissorwidth = renderer->scissorWidth();
		uint scissorheight = renderer->scissorHeight();
		glScissor(scissorx, scissory, scissorwidth, scissorheight);
	}

	if (_faceCounts.empty()) {
		if (!_materials.empty()) {
			_materials[0].apply();
		}
		glDrawElements(_mode, _indexes.size(), GL_UNSIGNED_SHORT, _indexes.data());
		if (!_materials.empty()) {
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			renderer->disableTexture();
		}
	} else {
		int totalfacecount = 0;
		for (unsigned int i = 0; i < _materials.size(); i++) {
			if (_faceCounts[i]) {
			if (!hasAlpha(i) || renderer->shadowMode() == TeRenderer::ShadowMode1 || !_shouldDrawMaybe) {
				_materials[i].apply();
				glDrawElements(_mode, _faceCounts[i] * 3, GL_UNSIGNED_SHORT, _indexes.data() + totalfacecount * 3);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				renderer->disableTexture();
			}
			totalfacecount += _faceCounts[i];
		  }
		}
	}

	if (!renderer->scissorEnabled()) {
		glDisable(GL_SCISSOR_TEST);
	}
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	//renderer->setCurrentColor(renderer->currentColor()); // pointless?

	if (_drawWires && !normals.empty()) {
		TeLight::disableAll();
		glBegin(GL_LINES);
		renderer->setCurrentColor(TeColor(255, 255, 255, 255));
		if (!_verticies.empty()) {
			error("TODO: Implement wire drawing here in TeMesh::draw..");
			/*
			offset1 = 1;
			offset2 = 2;
			do {
			  i = (ulong)offset2;
			  uVar5 = (ulong)(offset2 - 1);
			  totalfacecount = (ulong)(offset2 - 2);
			  glVertex3f
						((&verticiesbuf->f1)[totalfacecount],(&verticiesbuf->f1)[uVar5],
						 (&verticiesbuf->f1)[i]);
			  glVertex3f
						((&verticiesbuf->f1)[totalfacecount] + (&normalsbuf->f1)[totalfacecount],
						 (&verticiesbuf->f1)[uVar5] + (&normalsbuf->f1)[uVar5],
						 (&verticiesbuf->f1)[i] + (&normalsbuf->f1)[i]);
			  offset2 = offset2 + 3;
			  i = (ulong)offset1;
			  offset1 = offset1 + 1;
			} while (i < (this->verticiesArray).len);
			 */
		}
		glEnd();
	}
	
	renderer->setMatrixMode(TeRenderer::MM_GL_MODELVIEW);
	renderer->popMatrix();
	renderer->popMatrix();
}

TeMesh::Mode TeMesh::getMode() const {
	// Do the reverse translation of setMode... why? I dunno.. the game does that..
	switch(_mode) {
	case MeshMode0:
		return MeshMode1;
	case MeshMode1:
		return MeshMode2;
	case MeshMode2:
		return MeshMode3;
	case MeshMode3:
		return MeshMode4;
	case MeshMode4:
		return MeshMode5;
	case MeshMode5:
		return MeshMode6;
	case MeshMode6:
		return MeshMode7;
	default:
		return MeshMode0;
	}
}

bool TeMesh::hasAlpha(uint idx) {
	// TODO: this logic is a bit sketchy.  Check it again.
	bool retval = _hasAlpha && !_colors.empty();
	for (const TeMaterial &material : _materials) {
		/*if (!material._someFlagDefault0 == 0)*/
		retval = true;
		if (material._mode != TeMaterial::Mode1 && material._ambientColor.a() == 255)
			retval = (material._diffuseColor.a() != 255);
	}
	return retval;
}

TeVector3f32 TeMesh::normal(uint idx) const {
	if (!_updatedNormals.empty())
		return _updatedNormals[idx];
	else
		return _normals[idx];
}

void TeMesh::resizeUpdatedTables(unsigned long newSize) {
	_updatedVerticies.resize(newSize);
	_updatedNormals.resize(newSize);
}

void TeMesh::setColor(const TeColor &col) {
	Te3DObject2::setColor(col);
	
	if (!_verticies.empty()) {
		TeColor colnow = Te3DObject2::color();
		_colors.resize(_verticies.size());
		if (colnow.a() != 255)
			_hasAlpha = true;

		for (unsigned int i = 0; i < _verticies.size(); i++) {
			_colors[i] = colnow;
		}
	}
}

void TeMesh::setColor(uint idx, const TeColor &col) {
	if (col.a() != 255) {
		_hasAlpha = true;
	}
	_colors.resize(_verticies.size());
	_colors[idx] = col;
}

void TeMesh::setConf(unsigned long vertexCount, unsigned long indexCount, enum Mode mode, unsigned int materialCount, unsigned int materialIndexCount) {
	destroy();
	_initialMaterialIndexCount = materialIndexCount;
	_verticies.resize(vertexCount);
	_indexes.resize(indexCount);
	_materials.resize(materialCount);
	_matricies.resize(vertexCount);
	// TODO: This can't be right.. what's going on with this translation??
	switch(mode) {
	case MeshMode0:
	case MeshMode1:
	  _mode = MeshMode0;
	  break;
	case MeshMode2:
	  _mode = MeshMode1;
	  break;
	case MeshMode3:
	  _mode = MeshMode2;
	  break;
	case MeshMode4:
	  _mode = MeshMode3;
	  break;
	case MeshMode5:
	  _mode = MeshMode4;
	  break;
	case MeshMode6:
	  _mode = MeshMode5;
	  break;
	case MeshMode7:
	  _mode = MeshMode6;
	}
}

void TeMesh::setIndex(unsigned int index, unsigned int val) {
	_indexes[index] = val;
}

void TeMesh::setNormal(unsigned int index, const TeVector3f32 &val) {
	_normals.resize(_verticies.size());
	_normals[index] = val;
}

void TeMesh::setTextureUV(unsigned int index, const TeVector2f32 &val) {
	_uvs.resize(_verticies.size());
	_uvs[index] = val;
}

void TeMesh::setVertex(unsigned int index, const TeVector3f32 &val) {
	_verticies[index] = val;
}

TeVector3f32 TeMesh::vertex(uint idx) const {
	if (!_updatedVerticies.empty())
		return _updatedVerticies[idx];
	else
		return _verticies[idx];
}

// TODO: Add more functions here.

} // end namespace Syberia
