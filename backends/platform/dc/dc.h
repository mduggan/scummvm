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

#include "backends/base-backend.h"
#include <graphics/surface.h>
#include <graphics/paletteman.h>
#include <ronin/soundcommon.h>
#include "backends/timer/default/default-timer.h"
#include "backends/audiocd/default/default-audiocd.h"
#include "backends/fs/fs-factory.h"
#include "audio/mixer_intern.h"
#include "common/language.h"
#include "common/platform.h"
#ifdef DYNAMIC_MODULES
#include "backends/plugins/dynamic-plugin.h"
#endif

#define NUM_BUFFERS 4
#define SOUND_BUFFER_SHIFT 3

class Interactive
{
 public:
  virtual int key(int k, byte &shiftFlags) = 0;
  virtual void mouse(int x, int y) = 0;
  virtual ~Interactive() = 0;
};
inline Interactive::~Interactive() { }

#include "softkbd.h"

class DCHardware {
 private:
  static void dc_init_hardware();
 protected:
  DCHardware() { dc_init_hardware(); }
};

class DCCDManager : public DefaultAudioCDManager {
public:
	// Poll cdrom status
	// Returns true if cd audio is playing
	bool isPlaying() const override;

	// Play cdrom audio track
	bool play(int track, int numLoops, int startFrame, int duration, bool onlyEmulate = false,
		Audio::Mixer::SoundType soundType = Audio::Mixer::kMusicSoundType) override;

	// Stop cdrom audio track
	void stop() override;
};

class OSystem_Dreamcast : private DCHardware, public EventsBaseBackend, public PaletteManager, public FilesystemFactory
#ifdef DYNAMIC_MODULES
  , public FilePluginProvider
#endif
 {

 public:
  OSystem_Dreamcast();

  virtual void initBackend();

  // Determine whether the backend supports the specified feature.
  bool hasFeature(Feature f);

  // En-/disable the specified feature.
  void setFeatureState(Feature f, bool enable);

  // Query the state of the specified feature.
  bool getFeatureState(Feature f);

  // Set colors of the palette
  PaletteManager *getPaletteManager() { return this; }
protected:
	// PaletteManager API
  void setPalette(const byte *colors, uint start, uint num);
  void grabPalette(byte *colors, uint start, uint num) const;

public:

  // Determine the pixel format currently in use for screen rendering.
  Graphics::PixelFormat getScreenFormat() const;

  // Returns a list of all pixel formats supported by the backend.
  Common::List<Graphics::PixelFormat> getSupportedFormats() const;

  // Set the size of the video bitmap.
  // Typically, 320x200
  void initSize(uint w, uint h, const Graphics::PixelFormat *format);
  int16 getHeight() { return _screen_h; }
  int16 getWidth() { return _screen_w; }

  // Draw a bitmap to screen.
  // The screen will not be updated to reflect the new bitmap
  void copyRectToScreen(const void *buf, int pitch, int x, int y, int w, int h);

	virtual Graphics::Surface *lockScreen();
	virtual void unlockScreen();

  // Update the dirty areas of the screen
  void updateScreen();

  // Either show or hide the mouse cursor
  bool showMouse(bool visible);

  // Move ("warp") the mouse cursor to the specified position.
  void warpMouse(int x, int y);

  // Set the bitmap that's used when drawing the cursor.
  void setMouseCursor(const void *buf, uint w, uint h, int hotspot_x, int hotspot_y, uint32 keycolor, bool dontScale, const Graphics::PixelFormat *format, const byte *mask);

  // Replace the specified range of cursor the palette with new colors.
  void setCursorPalette(const byte *colors, uint start, uint num);

  // Shaking is used in SCUMM. Set current shake position.
  void setShakePos(int shake_x_pos, int shake_y_pos);

  // Get the number of milliseconds since the program was started.
  uint32 getMillis(bool skipRecord = false);

  // Delay for a specified amount of milliseconds
  void delayMillis(uint msecs);

  // Get the current time and date. Correspond to time()+localtime().
  void getTimeAndDate(TimeDate &td, bool skipRecord = false) const;

  // Get the next event.
  // Returns true if an event was retrieved.
  bool pollEvent(Common::Event &event);

  // Quit
  void quit();

  // Overlay
  int16 getOverlayHeight() const;
  int16 getOverlayWidth() const;
  bool isOverlayVisible() const { return _overlay_visible; }
  void showOverlay(bool inGUI);
  void hideOverlay();
  void clearOverlay();
  void grabOverlay(Graphics::Surface &surface);
  void copyRectToOverlay(const void *buf, int pitch, int x, int y, int w, int h);
  virtual Graphics::PixelFormat getOverlayFormat() const { return Graphics::PixelFormat(2, 4, 4, 4, 4, 8, 4, 0, 12); }

  // Mutex handling
  Common::MutexInternal *createMutex();

  // Set a window caption or any other comparable status display to the
  // given value.
  void setWindowCaption(const Common::U32String &caption);

  // Modulatized backend
  Audio::Mixer *getMixer() { return _mixer; }

  // Extra SoftKbd support
  void mouseToSoftKbd(int x, int y, int &rx, int &ry) const;

  // Filesystem
  AbstractFSNode *makeRootFileNode() const override;
  AbstractFSNode *makeCurrentDirectoryFileNode() const override;
  AbstractFSNode *makeFileNodePath(const Common::String &path) const override;

 private:

  Audio::MixerImpl *_mixer;
  SoftKeyboard _softkbd;

  int _ms_cur_x, _ms_cur_y, _ms_cur_w, _ms_cur_h, _ms_old_x, _ms_old_y;
  int _ms_hotspot_x, _ms_hotspot_y, _ms_visible, _devpoll, _last_screen_refresh;
  int _current_shake_x_pos, _current_shake_y_pos, _screen_w, _screen_h;
  int _overlay_x, _overlay_y;
  unsigned char *_ms_buf;
  uint32 _ms_keycolor;
  bool _overlay_visible, _overlay_dirty, _screen_dirty;
  int _screen_buffer, _overlay_buffer, _mouse_buffer;
  bool _aspect_stretch, _softkbd_on, _enable_cursor_palette;
  bool _overlay_in_gui;
  float _overlay_fade, _xscale, _yscale, _top_offset;
  int _softkbd_motion;

  unsigned char *screen;
  unsigned short *mouse;
  unsigned short *overlay;
  void *screen_tx[NUM_BUFFERS];
  void *mouse_tx[NUM_BUFFERS];
  void *ovl_tx[NUM_BUFFERS];
  unsigned short palette[256], cursor_palette[256];

  Graphics::Surface _framebuffer;
  int _screenFormat, _mouseFormat;

  int temp_sound_buffer[RING_BUFFER_SAMPLES>>SOUND_BUFFER_SHIFT];

  uint initSound();
  void checkSound();

  void updateScreenTextures(void);
  void updateScreenPolygons(void);
  void maybeRefreshScreen(void);
  void drawMouse(int xdraw, int ydraw, int w, int h,
		 unsigned char *buf, bool visible);

  void setScaling();


  Common::SaveFileManager *createSavefileManager();

  Common::SeekableReadStream *createConfigReadStream();
  Common::WriteStream *createConfigWriteStream();

  void logMessage(LogMessageType::Type type, const char *message);
  Common::String getSystemLanguage() const;

#ifdef DYNAMIC_MODULES
  class DCPlugin;

 protected:
  Plugin* createPlugin(const Common::FSNode &node) const;
  bool isPluginFilename(const Common::FSNode &node) const;
  void addCustomDirectories(Common::FSList &dirs) const;
 public:
  PluginList getPlugins();
 private:
  const char *pluginCustomDirectory;
#endif
};


extern int handleInput(struct mapledev *pad,
		       int &mouse_x, int &mouse_y,
		       byte &shiftFlags, Interactive *inter = NULL);
extern bool selectGame(char *&, char *&, char *&, Common::Language &, Common::Platform &, class Icon &);
#ifdef DYNAMIC_MODULES
extern bool selectPluginDir(Common::String &selection, const Common::FSNode &base);
#endif
