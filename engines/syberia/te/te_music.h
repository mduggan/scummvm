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

#ifndef SYBERIA_TE_TE_MUSIC_H
#define SYBERIA_TE_TE_MUSIC_H

#include "common/str.h"
#include "syberia/te/te_resource.h"

namespace Syberia {

class TeMusic : public TeResource {
public:
	TeMusic();

	void close() {
		stop();
	}
	bool pause();
	bool play();
	bool repeat();
	void repeat(bool val);
	void resume();
	void stop();

	byte currentData();
	void entry();
	const Common::String &filePath() const {
		return _path1;
	}
	bool isPlaying();
	bool load(const Common::String &path);
	bool onSoundManagerVolumeChanged();
	Common::String path();
	void setChannelName(const Common::String &name) {
		_channelName = name;
	}
	void setFilePath(const Common::String &name);
	void update();


private:
	Common::String _path1;
	Common::String _channelName;

	bool _repeat;
};

} // end namespace Syberia

#endif // SYBERIA_TE_TE_MUSIC_H
