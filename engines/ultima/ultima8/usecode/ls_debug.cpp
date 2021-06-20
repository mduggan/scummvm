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
#include "ultima/ultima8/usecode/ls_debug.h"
#include "ultima/ultima8/world/loop_script.h"

namespace Ultima {
namespace Ultima8 {
namespace LoopScript {

const char *opcodeStr(uint8 opcode) {
	switch (opcode) {
		case 0:
			return "FALSE";
		case 1:
			return "TRUE";
		case LS_TOKEN_AND:
			return "AND";
		case LS_TOKEN_OR:
			return "OR";
		case LS_TOKEN_NOT:
			return "NOT";
		case LS_TOKEN_EQUAL:
			return "EQUAL";
		case LS_TOKEN_GREATER:
			return "GREATER";
		case LS_TOKEN_LESS:
			return "LESS";
		case LS_TOKEN_GEQUAL:
			return "GEQUAL";
		case LS_TOKEN_LEQUAL:
			return "LEQUAL";
		case LS_TOKEN_INT:
			return "INT";
		case LS_TOKEN_NPCNUM:
			return "NPCNUM";
		case LS_TOKEN_STATUS:
			return "STATUS";
		case LS_TOKEN_Q:
			return "Q";
		case LS_TOKEN_FAMILY:
			return "FAMILY";
		case LS_TOKEN_SHAPE:
			return "SHAPE";
		case LS_TOKEN_SHAPE+1:
			return "SHAPE_1";
		case LS_TOKEN_SHAPE+2:
			return "SHAPE_2";
		case LS_TOKEN_SHAPE+3:
			return "SHAPE_3";
		case LS_TOKEN_SHAPE+4:
			return "SHAPE_4";
		case LS_TOKEN_FRAME:
			return "FRAME";
		case LS_TOKEN_FRAME+1:
			return "FRAME_1";
		case LS_TOKEN_FRAME+2:
			return "FRAME_2";
		case LS_TOKEN_FRAME+3:
			return "FRAME_3";
		case LS_TOKEN_FRAME+4:
			return "FRAME_4";
		case LS_TOKEN_END:
			return "END";
		default:
			break;
	}
	return "**UNKNOWN**";
}

const char *searchTypeStr(uint8 type) {
	switch (type) {
		case 2:
			return "area";
		case 3:
			return "recursive area";
		case 4:
			return "container";
		case 5:
			return "recursive container";
		case 6:
			return "surface";
		default:
			break;
	}
	return "**invalid search**";
}

static const uint8 *_formatUint16(const uint8 *script, Common::String &formatted) {
	char buf[8];
	uint16 val = script[0] + (script[1] << 8);
	sprintf(buf, "%04X", val);
	formatted += buf;
	return script + 2;
}

static const uint8 *_formatUint16List(const uint8 *script, Common::String &formatted, int n) {
	formatted += '[';
	for (int i = 0; i < n; i++) {
		script = _formatUint16(script, formatted);
		if (i < n - 1)
			formatted += ',';
	}
	formatted += ']';
	return script;
}

Common::String formatScript(const uint8 *script) {
	uint8 opcode = *script;
	if (opcode == LS_TOKEN_END)
		return "<EMPTY>";

	Common::Array<Common::String> stack;
	while (opcode != LS_TOKEN_END) {
		Common::String formatted;
		switch (opcode) {
			// zero operands
			case 0:
			case 1:
			case LS_TOKEN_SHAPE:
			case LS_TOKEN_FRAME:
			case LS_TOKEN_NPCNUM:
			case LS_TOKEN_STATUS:
			case LS_TOKEN_Q:
			case LS_TOKEN_FAMILY:
				formatted = opcodeStr(opcode);
				script++;
				break;

			// Single-operand function
			case LS_TOKEN_NOT: {
				Common::String arg = stack.back();
				stack.pop_back();
				formatted = Common::String::format("%s(%s)", opcodeStr(opcode), arg.c_str());
				script++;
				break;
			}

			// 2-operand functions
			case LS_TOKEN_AND:
			case LS_TOKEN_OR:
			case LS_TOKEN_EQUAL:
			case LS_TOKEN_GREATER:
			case LS_TOKEN_LESS:
			case LS_TOKEN_GEQUAL:
			case LS_TOKEN_LEQUAL: {
				Common::String arg1 = stack.back();
				stack.pop_back();
				Common::String arg2 = stack.back();
				stack.pop_back();
				formatted = Common::String::format("%s(%s, %s)",
						opcodeStr(opcode), arg1.c_str(), arg2.c_str()
				);
				script++;
				break;
			}

			// Operators that take immediates.
			case LS_TOKEN_INT:
				// Just print the immediate to make the expression cleaner.
				script++;
				script = _formatUint16(script, formatted);
				break;

			case LS_TOKEN_SHAPE+1:
			case LS_TOKEN_FRAME+1:
				formatted = opcodeStr(opcode);
				formatted += " ";
				script++;
				script = _formatUint16(script, formatted);
				break;

			case LS_TOKEN_SHAPE+2:
			case LS_TOKEN_FRAME+2:
				formatted = opcodeStr(opcode);
				formatted += " ";
				script++;
				script = _formatUint16List(script, formatted, 2);
				break;

			case LS_TOKEN_SHAPE+3:
			case LS_TOKEN_FRAME+3:
				formatted = opcodeStr(opcode);
				formatted += " ";
				script++;
				script = _formatUint16List(script, formatted, 3);
				break;

			case LS_TOKEN_SHAPE+4:
			case LS_TOKEN_FRAME+4:
				formatted = opcodeStr(opcode);
				formatted += " ";
				script++;
				script = _formatUint16List(script, formatted, 4);
				break;

			default:
				// ?? ignore it?
				script++;
				break;
		}
		stack.push_back(formatted);
		opcode = *script;
	}
	
	return stack.back();
}

} // End of namespace LoopScript
} // End of namespace Ultima8
} // End of namespace Ultima
