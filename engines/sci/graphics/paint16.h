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

#ifndef SCI_GRAPHICS_PAINT16_H
#define SCI_GRAPHICS_PAINT16_H

namespace Sci {

class GfxPorts;
class GfxScreen;
class GfxPalette;
class Font;
class GfxView;

/**
 * Paint16 class, handles painting/drawing for SCI16 (SCI0-SCI1.1) games
 */
class GfxPaint16 {
public:
	GfxPaint16(ResourceManager *resMan, SegManager *segMan, GfxCache *cache, GfxPorts *ports, GfxCoordAdjuster16 *coordAdjuster, GfxScreen *screen, GfxPalette *palette, GfxTransitions *transitions, AudioPlayer *audio);
	~GfxPaint16();

	void init(GfxAnimate *animate, GfxText16 *text16);

	void debugSetEGAdrawingVisualize(bool state);

	void drawPicture(GuiResourceId pictureId, bool mirroredFlag, bool addToFlag, GuiResourceId paletteId);
	void drawCelAndShow(GuiResourceId viewId, int16 loopNo, int16 celNo, int16 tweenNo, uint16 leftPos, uint16 topPos, byte priority, uint16 paletteNo, uint16 scaleX = 128, uint16 scaleY = 128, uint16 scaleSignal = 0);
	void drawCel(Graphics::Surface *viewpng, const byte *viewenh, int pixelsLength, bool viewEnhanced, bool enhancedIs256, GuiResourceId viewId, int16 loopNo, int16 celNo, int16 tweenNo, const Common::Rect &celRect, byte priority, uint16 paletteNo, uint16 scaleX = 128, uint16 scaleY = 128, uint16 scaleSignal = 0);
	void drawCel(Graphics::Surface *viewpng, const byte *viewenh, int pixelsLength, bool viewEnhanced, bool enhancedIs256, GfxView *view, int16 loopNo, int16 celNo, int16 tweenNo, const Common::Rect &celRect, byte priority, uint16 paletteNo, uint16 scaleX = 128, uint16 scaleY = 128, uint16 scaleSignal = 0);
	void drawHiresCelAndShow(GuiResourceId viewId, int16 loopNo, int16 celNo, int16 tweenNo, uint16 leftPos, uint16 topPos, byte priority, uint16 paletteNo, reg_t upscaledHiresHandle, uint16 scaleX = 128, uint16 scaleY = 128);

	void clearScreen(byte color = 255);
	void invertRect(const Common::Rect &rect);
	void invertRectViaXOR(const Common::Rect &rect);
	void eraseRect(const Common::Rect &rect);
	void paintRect(const Common::Rect &rect);
	void fillRect(const Common::Rect &rect, int16 drawFlags, byte color, byte priority = 0, byte control = 0);
	void frameRect(const Common::Rect &rect);

	void bitsShow(const Common::Rect &r);
	void bitsShowHires(const Common::Rect &rect);
	reg_t bitsSave(const Common::Rect &rect, byte screenFlags);
	void bitsGetRect(reg_t memoryHandle, Common::Rect *destRect);
	void bitsRestore(reg_t memoryHandle);
	void bitsFree(reg_t memoryHandle);

	void kernelDrawPicture(GuiResourceId pictureId, int16 animationNr, bool animationBlackoutFlag, bool mirroredFlag, bool addToFlag, int16 EGApaletteNo);
	void kernelDrawCel(GuiResourceId viewId, int16 loopNo, int16 celNo, int16 tweenNo, uint16 leftPos, uint16 topPos, int16 priority, uint16 paletteNo, uint16 scaleX, uint16 scaleY, bool hiresMode, reg_t upscaledHiresHandle);

	void kernelGraphFillBoxForeground(const Common::Rect &rect);
	void kernelGraphFillBoxBackground(const Common::Rect &rect);
	void kernelGraphFillBox(const Common::Rect &rect, uint16 colorMask, int16 color, int16 priority, int16 control);
	void kernelGraphFrameBox(const Common::Rect &rect, int16 color);
	void kernelGraphDrawLine(Common::Point startPoint, Common::Point endPoint, int16 color, int16 priority, int16 control);
	reg_t kernelGraphSaveBox(const Common::Rect &rect, uint16 flags);
	reg_t kernelGraphSaveUpscaledHiresBox(const Common::Rect &rect);
	void kernelGraphRestoreBox(reg_t handle);
	void kernelGraphUpdateBox(const Common::Rect &rect, bool hiresMode);
	void kernelGraphRedrawBox(Common::Rect rect);

	reg_t kernelDisplay(const char *text, uint16 languageSplitter, int argc, reg_t *argv);

	reg_t kernelPortraitLoad(const Common::String &resourceName);
	void kernelPortraitShow(const Common::String &resourceName, Common::Point position, uint16 resourceNum, uint16 noun, uint16 verb, uint16 cond, uint16 seq);
	void kernelPortraitUnload(uint16 portraitId);
	GfxPorts *_ports;
	

private:
	ResourceManager *_resMan;
	SegManager *_segMan;
	AudioPlayer *_audio;
	GfxAnimate *_animate;
	GfxCache *_cache;	
	GfxCoordAdjuster16 *_coordAdjuster;
	GfxScreen *_screen;
	GfxPalette *_palette;
	GfxText16 *_text16;
	GfxTransitions *_transitions;

	// true means make EGA picture drawing visible
	bool _EGAdrawingVisualize;
};

} // End of namespace Sci

#endif
