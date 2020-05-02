/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "ultima/ultima8/misc/pent_include.h"
#include "ultima/ultima8/gumps/difficulty_gump.h"
#include "ultima/ultima8/graphics/render_surface.h"
#include "ultima/ultima8/gumps/desktop_gump.h"
#include "ultima/ultima8/graphics/gump_shape_archive.h"
#include "ultima/ultima8/graphics/palette_manager.h"
#include "ultima/ultima8/ultima8.h"
#include "ultima/ultima8/games/game_data.h"
#include "ultima/ultima8/graphics/shape.h"
#include "ultima/ultima8/graphics/shape_frame.h"
#include "ultima/ultima8/world/get_object.h"

namespace Ultima {
namespace Ultima8 {

DEFINE_RUNTIME_CLASSTYPE_CODE(DifficultyGump, Gump)

DifficultyGump::DifficultyGump()
	: Gump(0, 0, 5, 5), _selected(2) {
}

DifficultyGump::~DifficultyGump() {
}

static const int BUTTON_X = 158;
static const int BUTTON_Y = 120;
static const int BUTTON_SPACE = 17;
static const int BUTTON_HEIGHT = 42;


// gumps: 73: "difficulty level"
//        74-77: levels, 2 frames per level (left/right)

void DifficultyGump::InitGump(Gump *newparent, bool take_focus) {
	Gump::InitGump(newparent, take_focus);

	_dims.x = 0;
	_dims.y = 0;
	_dims.w = 640;
	_dims.h = 480;

    GumpShapeArchive *shapeArchive = GameData::get_instance()->getGumps();

	Shape *difficultyLevel = shapeArchive->getShape(73);

    Shape *levelShape[4];
    for (int s = 74; s < 78; s++) {
        levelShape[s - 74] = shapeArchive->getShape(s);
    }

	if (!difficultyLevel || !levelShape[0] || !levelShape[1] ||
                            !levelShape[2] || !levelShape[3]) {
		error("Couldn't load shapes for difficulty level");
		return;
	}

	PaletteManager *palman = PaletteManager::get_instance();
	assert(palman);
	const Palette *pal = palman->getPalette(PaletteManager::Pal_Diff);
	assert(pal);
	difficultyLevel->setPalette(pal);
    for (int s = 0; s < 4; s++) {
        levelShape[s]->setPalette(pal);
    }

	const ShapeFrame *difficultyFrame = difficultyLevel->getFrame(0);
	if (!difficultyFrame) {
		error("Couldn't load shape frame for difficulty level");
		return;
	}
	Gump *diffGump = new Gump(185, 77, difficultyFrame->_width, difficultyFrame->_height);
	diffGump->SetShape(difficultyLevel, 0);
	diffGump->InitGump(this, false);

    for (int s = 0; s < 4; s++) {
        const int y = BUTTON_Y + (BUTTON_SPACE + BUTTON_HEIGHT) * s;
        const ShapeFrame *leftFrame = levelShape[s]->getFrame(0);
        const ShapeFrame *rightFrame = levelShape[s]->getFrame(1);
        if (!leftFrame || !rightFrame) {
            error("Couldn't load shape frame for difficulty level %d", s);
            return;
        }
        Gump *lGump = new Gump(BUTTON_X, y, leftFrame->_width, leftFrame->_height);
        lGump->SetShape(levelShape[s], 0);
        lGump->InitGump(this, false);
        Gump *rGump = new Gump(BUTTON_X + leftFrame->_width, y, rightFrame->_width, rightFrame->_height);
        rGump->SetShape(levelShape[s], 1);
        rGump->InitGump(this, false);
    }

	// remove focus from children (just in case)
	if (_focusChild) _focusChild->OnFocus(false);
	_focusChild = 0;
}

void DifficultyGump::Close(bool no_del) {
	Gump::Close(no_del);
}

void DifficultyGump::OnFocus(bool gain) {
}

Gump *DifficultyGump::OnMouseDown(int button, int32 mx, int32 my) {
	// TODO: Check if they clicked on a difficulty button.
	return this;
}

void DifficultyGump::OnMouseClick(int button, int32 mx, int32 my) {
	// TODO: Check if they clicked on a difficulty button.
}

void DifficultyGump::ChildNotify(Gump *child, uint32 message) {
}

bool DifficultyGump::OnKeyDown(int key, int mod) {
	if (Gump::OnKeyDown(key, mod)) return true;

    // TODO: Handle arrow key selection of item.

	return false;
}

} // End of namespace Ultima8
} // End of namespace Ultima
