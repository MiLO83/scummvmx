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

#include "common/util.h"
#include "common/stack.h"
#include "graphics/primitives.h"

#include "sci/console.h"
#include "sci/sci.h"
#include "sci/event.h"
#include "sci/engine/kernel.h"
#include "sci/engine/script_patches.h"
#include "sci/engine/state.h"
#include "sci/engine/selector.h"
#include "sci/engine/vm.h"
#include "sci/graphics/cache.h"
#include "sci/graphics/compare.h"
#include "sci/graphics/cursor.h"
#include "sci/graphics/ports.h"
#include "sci/graphics/paint16.h"
#include "sci/graphics/palette.h"
#include "sci/graphics/view.h"
#include "sci/graphics/screen.h"
#include "sci/graphics/transitions.h"
#include "sci/graphics/animate.h"
#include <common/config-manager.h>
#include <image/png.h>
#include <cctype>

namespace Sci {

GfxAnimate::GfxAnimate(EngineState *state, ScriptPatcher *scriptPatcher, GfxCache *cache, GfxPorts *ports, GfxPaint16 *paint16, GfxScreen *screen, GfxPalette *palette, GfxCursor *cursor, GfxTransitions *transitions)
	: _s(state), _scriptPatcher(scriptPatcher), _cache(cache), _ports(ports), _paint16(paint16), _screen(screen), _palette(palette), _cursor(cursor), _transitions(transitions) {
	init();
}

GfxAnimate::~GfxAnimate() {
}

void GfxAnimate::init() {
	_lastCastData.clear();

	_fastCastEnabled = false;
	if (getSciVersion() == SCI_VERSION_1_1) {
		// Seems to have been available for all SCI1.1 games
		_fastCastEnabled = true;
	} else if (getSciVersion() >= SCI_VERSION_1_EARLY) {
		// fastCast only exists for some games between SCI1 early and SCI1 late
		// Try to detect it by code signature
		// It's extremely important, that we only enable it for games that actually need it
		if (detectFastCast()) {
			_fastCastEnabled = true;
		}
	}
}

// Signature for fastCast detection
static const uint16 fastCastSignature[] = {
	SIG_MAGICDWORD,
	0x35, 0x00,                      // ldi 00
	0xa1, 84,                        // sag global[84d]
	SIG_END
};

// Fast cast in games:

// SCI1 Early:
// KQ5 - no fastcast, LSL1 (demo) - no fastcast, Mixed Up Fairy Tales - *has fastcast*, XMas Card 1990 - no fastcast,
// SQ4Floppy - no fastcast, Mixed Up Mother Goose - no fastcast
//
// SCI1 Middle:
// LSL5 demo - no fastfast, Conquest of the Longbow demo - no fastcast, LSL1 - no fastcast,
// Astro Chicken II - no fastcast
//
// SCI1 Late:
// Castle of Dr. Brain demo - has fastcast, Castle of Dr. Brain - has fastcast,
// Conquests of the Longbow - has fastcast, Space Quest 1 EGA - has fastcast,
// King's Quest 5 multilingual - *NO* fastcast, Police Quest 3 demo - *NO* fastcast,
// LSL5 multilingual - has fastcast, Police Quest 3 - has fastcast,
// EcoQuest 1 - has fastcast, Mixed Up Fairy Tales demo - has fastcast,
// Space Quest 4 multilingual - *NO* fastcast
//
// SCI1.1
// Quest for Glory 3 demo - has fastcast, Police Quest 1 - hast fastcast, Quest for Glory 1 - has fastcast
// Laura Bow 2 Floppy - has fastcast, Mixed Up Mother Goose - has fastcast, Quest for Glory 3 - has fastcast
// Island of Dr. Brain - has fastcast, King's Quest 6 - has fastcast, Space Quest 5 - has fastcast
// Hoyle 4 - has fastcast, Laura Bow 2 CD - has fastcast, Freddy Pharkas CD - has fastcast
bool GfxAnimate::detectFastCast() {
	SegManager *segMan = _s->_segMan;
	const reg_t gameVMObject = g_sci->getGameObject();
	reg_t gameSuperVMObject = segMan->getObject(gameVMObject)->getSuperClassSelector();
	uint32 magicDWord = 0; // platform-specific BE/LE for performance
	int    magicDWordOffset = 0;

	if (gameSuperVMObject.isNull()) {
		gameSuperVMObject = gameVMObject; // Just in case. According to sci.cpp this may happen in KQ5CD, when loading saved games before r54510
	}

	Script *objectScript = segMan->getScript(gameSuperVMObject.getSegment());
	byte *scriptData = const_cast<byte *>(objectScript->getBuf(0));
	uint32 scriptSize = objectScript->getBufSize();

	_scriptPatcher->calculateMagicDWordAndVerify("fast cast detection", fastCastSignature, true, magicDWord, magicDWordOffset);

	// Signature is found for multilingual King's Quest 5 too, but it looks as if the fast cast global is never set
	// within that game. Which means even though we detect it as having the capability, it's never actually used.
	// The original multilingual KQ5 interpreter did have this feature disabled.
	// Sierra probably used latest system scripts and that's why we detect it.
	if (_scriptPatcher->findSignature(magicDWord, magicDWordOffset, fastCastSignature, "fast cast detection", SciSpan<const byte>(scriptData, scriptSize)) >= 0) {
		// Signature found, game seems to use fast cast for kAnimate
		return true;
	}
	return false;
}

void GfxAnimate::disposeLastCast() {
	_lastCastData.clear();
}

bool GfxAnimate::invoke(List *list, int argc, reg_t *argv) {
	reg_t curAddress = list->first;
	Node *curNode = _s->_segMan->lookupNode(curAddress);
	reg_t curObject;
	uint16 signal;

	while (curNode) {
		curObject = curNode->value;

		if (_fastCastEnabled) {
			// Check if the game has a fastCast object set
			//  if we don't abort kAnimate processing, at least in kq5 there will be animation cels drawn into speech boxes.
			if (!_s->variables[VAR_GLOBAL][kGlobalVarFastCast].isNull()) {
				// This normally points to an object called "fastCast",
				// but for example in Eco Quest 1 it may also point to an object called "EventHandler" (see bug #5170)
				// Original SCI only checked, if this global was not 0.
				return false;
			}
		}

		signal = readSelectorValue(_s->_segMan, curObject, SELECTOR(signal));
		if (!(signal & kSignalFrozen)) {
			// Call .doit method of that object
			invokeSelector(_s, curObject, SELECTOR(doit), argc, argv, 0);

			// If a game is being loaded, stop processing
			if (_s->abortScriptProcessing != kAbortNone)
				return true; // Stop processing

			// Lookup node again, since the nodetable it was in may have been reallocated.
			// The node might have been deallocated at this point (e.g. LSL2, room 42),
			// in which case the node reference will be null and the loop will stop below.
			// If the node is deleted from kDeleteKey, it won't have a successor node, thus
			// list processing will stop here (which is what SSCI does).
			curNode = _s->_segMan->lookupNode(curAddress, false);
		}

		if (curNode) {
			curAddress = curNode->succ;
			curNode = _s->_segMan->lookupNode(curAddress);
		}
	}
	return true;
}

bool sortHelper(const AnimateEntry &entry1, const AnimateEntry &entry2) {
	if (entry1.y == entry2.y) {
		// if both y and z are the same, use the order we were given originally
		//  this is needed for special cases like iceman room 35
		if (entry1.z == entry2.z)
			return entry1.givenOrderNo < entry2.givenOrderNo;
		else
			return entry1.z < entry2.z;
	}
	return entry1.y < entry2.y;
}

Graphics::Surface *loadCelPNG(Common::SeekableReadStream *s) {
	Image::PNGDecoder d;

	if (!s)
		return nullptr;
	d.loadStream(*s);
	delete s;

	Graphics::Surface *srf = d.getSurface()->convertTo(Graphics::PixelFormat(4, 8, 8, 8, 8, 0, 8, 16, 24));
	return srf;
}
Graphics::Surface *loadCelPNGCLUT(Common::SeekableReadStream *s) {
	Image::PNGDecoder d;

	if (!s)
		return nullptr;
	d.loadStream(*s);
	delete s;
	Graphics::Surface *srf = d.getSurface()->convertTo(Graphics::PixelFormat::createFormatCLUT8());
	return srf;
}

Graphics::Surface *loadCelPNGCLUTOverride(Common::SeekableReadStream *s, GfxScreen *_tehScreen) {
	Image::PNGDecoder d;

	if (!s)
		return nullptr;
	d.loadStream(*s);
	delete s;
	Graphics::Surface *srf = d.getSurface()->convertTo(Graphics::PixelFormat::createFormatCLUT8(), d.getPalette());

	for (int16 i = 0; i < 256; i++) {
		g_sci->_gfxPalette16->_paletteOverride.colors[i].r = d.getPalette()[i * 3];
		g_sci->_gfxPalette16->_paletteOverride.colors[i].g = d.getPalette()[(i * 3) + 1];
		g_sci->_gfxPalette16->_paletteOverride.colors[i].b = d.getPalette()[(i * 3) + 2];
	}
	g_sci->_gfxPalette16->_sysPalette = g_sci->_gfxPalette16->_paletteOverride;
	//memcpy((void *)g_sci->_gfxPalette16->_paletteOverride, d.getPalette(), sizeof(d.getPalette()));
	//_tehScreen->setPalette(d.getPalette(), 0, 256, true);
	return srf;
}

void GfxAnimate::makeSortedList(List *list) {
	reg_t curAddress = list->first;
	Node *curNode = _s->_segMan->lookupNode(curAddress);
	int16 listNr;
	
	// Fill the list
	for (listNr = 0; curNode != 0; listNr++) {
		AnimateEntry listEntry;
		const reg_t curObject = curNode->value;
		listEntry.object = curObject;
		listEntry.castHandle = NULL_REG;

		// Get data from current object
		listEntry.givenOrderNo = listNr;
		listEntry.viewId = readSelectorValue(_s->_segMan, curObject, SELECTOR(view));
		listEntry.loopNo = readSelectorValue(_s->_segMan, curObject, SELECTOR(loop));
		listEntry.celNo = readSelectorValue(_s->_segMan, curObject, SELECTOR(cel));
		listEntry.paletteNo = readSelectorValue(_s->_segMan, curObject, SELECTOR(palette));
		listEntry.x = readSelectorValue(_s->_segMan, curObject, SELECTOR(x));
		listEntry.y = readSelectorValue(_s->_segMan, curObject, SELECTOR(y));
		listEntry.z = readSelectorValue(_s->_segMan, curObject, SELECTOR(z));
		listEntry.priority = readSelectorValue(_s->_segMan, curObject, SELECTOR(priority));
		listEntry.processed = false;
		listEntry.signal = readSelectorValue(_s->_segMan, curObject, SELECTOR(signal));
		listEntry.pixelsLength = 0;
		
		if (getSciVersion() >= SCI_VERSION_1_1) {
			// Cel scaling
			listEntry.scaleSignal = readSelectorValue(_s->_segMan, curObject, SELECTOR(scaleSignal));
			if (listEntry.scaleSignal & kScaleSignalDoScaling) {
				listEntry.scaleX = readSelectorValue(_s->_segMan, curObject, SELECTOR(scaleX));
				listEntry.scaleY = readSelectorValue(_s->_segMan, curObject, SELECTOR(scaleY));
			} else {
				listEntry.scaleX = 128;
				listEntry.scaleY = 128;
			}
		} else {
			listEntry.scaleSignal = 0;
			listEntry.scaleX = 128;
			listEntry.scaleY = 128;
		}
		// listEntry.celRect is filled in AnimateFill()
		listEntry.showBitsFlag = false;
		foundInList = false;
		if (_list.size() > 0) {
			for (it = _list.begin(); it != _list.end(); ++it) {
				bool found = false;
				if (found == false && it->viewId == listEntry.viewId && it->loopNo == listEntry.loopNo && it->celNo == listEntry.celNo && it->processed == false && listEntry.processed == false) {
					it->tweenNo++;
					if (it->tweenNo > 3) {
						it->tweenNo = 3;
					}
					listEntry.tweenNo = it->tweenNo;
					it->processed = true;
					listEntry.processed = true;
					_newList.push_back(listEntry);
					foundInList = true;
					found = true;
				}
			}
		}
		if (foundInList == false)
		{
			listEntry.tweenNo = 0;
			listEntry.processed = true;
			listEntry.viewEnhanced = false;
			listEntry.enhancedIs256 = false;
			_newList.push_back(listEntry);
		}

		curAddress = curNode->succ;
		curNode = _s->_segMan->lookupNode(curAddress);
	}
	// Clear lists
	_list.clear();
	_lastCastData.clear();
	for (itb = _newList.begin(); itb != _newList.end(); ++itb) {
		AnimateEntry listEntry;
		listEntry.object = itb->object;
		listEntry.castHandle = itb->castHandle;

		// Get data from current object
		listEntry.givenOrderNo = itb->givenOrderNo;
		listEntry.viewId = itb->viewId;
		listEntry.loopNo = itb->loopNo;
		listEntry.celNo = itb->celNo;
		listEntry.tweenNo = itb->tweenNo;
		listEntry.processed = false;
		listEntry.paletteNo = itb->paletteNo;
		listEntry.x = itb->x;
		listEntry.y = itb->y;
		listEntry.z = itb->z;
		listEntry.priority = itb->priority;
		listEntry.signal = itb->signal;
		listEntry.viewpng = itb->viewpng;
		listEntry.viewenh = itb->viewenh;
		listEntry.pixelsLength = itb->pixelsLength;
		listEntry.viewEnhanced = itb->viewEnhanced;
		listEntry.enhancedIs256 = itb->enhancedIs256;
		if (getSciVersion() >= SCI_VERSION_1_1) {
			// Cel scaling
			listEntry.scaleSignal = itb->scaleSignal;
			if (listEntry.scaleSignal & kScaleSignalDoScaling) {
				listEntry.scaleX = itb->scaleX;
				listEntry.scaleY = itb->scaleY;
			} else {
				listEntry.scaleX = 128;
				listEntry.scaleY = 128;
			}
		} else {
			listEntry.scaleSignal = 0;
			listEntry.scaleX = 128;
			listEntry.scaleY = 128;
		}
		// listEntry.celRect is filled in AnimateFill()
		listEntry.showBitsFlag = false;
		Common::String fn = "view.";
		char viewNoStr[5];
		sprintf(viewNoStr, "%u", listEntry.viewId);
		for (int n = 0; n < 5; n++) {
			if (viewNoStr[n] >= '0' && viewNoStr[n] <= '9') {
				fn += viewNoStr[n];
			}
		}
		fn += ".";
		char loopNoStr[5];
		sprintf(loopNoStr, "%u", listEntry.loopNo);
		for (int n = 0; n < 5; n++) {
			if (loopNoStr[n] >= '0' && loopNoStr[n] <= '9') {
				fn += loopNoStr[n];
			}
		}
		fn += ".";
		char celNoStr[5];
		sprintf(celNoStr, "%u", listEntry.celNo);
		for (int n = 0; n < 5; n++) {
			if (celNoStr[n] >= '0' && celNoStr[n] <= '9') {
				fn += celNoStr[n];
			}
		}
		char tweenNoStr[5];
		Common::String twn = "";
		sprintf(tweenNoStr, "%u", listEntry.tweenNo);
		for (int n = 0; n < 5; n++) {
			if (tweenNoStr[n] >= '0' && tweenNoStr[n] <= '9') {
				twn += tweenNoStr[n];
			}
		}
		debug(fn.c_str());
		Common::FSNode folder;
		if (ConfMan.hasKey("extrapath")) {

			if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".png").exists()) {
				Common::String fileName = folder.getChild(fn + ".png").getName();
				Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
				if (!file) {
					debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
				} else {
					debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
					listEntry.viewpng = loadCelPNG(file);
					if (listEntry.viewpng) {
						listEntry.viewenh = (const byte *)listEntry.viewpng->getPixels();
						if (listEntry.viewenh) {
							listEntry.pixelsLength = listEntry.viewpng->w * listEntry.viewpng->h;
							listEntry.viewEnhanced = true;
							listEntry.enhancedIs256 = false;
						}
					}
				}
			} else if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + "_256.png").exists()) {
				if (!listEntry.viewEnhanced) {
					Common::String fileName = folder.getChild(fn + "_256.png").getName();
					Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
					if (!file) {
						debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
					} else {
						debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
						listEntry.viewpng = loadCelPNGCLUT(file);
						if (listEntry.viewpng) {
							listEntry.viewenh = (const byte *)listEntry.viewpng->getPixels();
							if (listEntry.viewenh) {
								listEntry.pixelsLength = listEntry.viewpng->w * listEntry.viewpng->h;
								listEntry.viewEnhanced = true;
								listEntry.enhancedIs256 = true;
							}
						}
					}
				}
			} else if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + "_256RP.png").exists()) {
				if (!listEntry.viewEnhanced) {
					Common::String fileName = folder.getChild(fn + "_256RP.png").getName();
					Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
					if (!file) {
						debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
					} else {
						debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
						listEntry.viewpng = loadCelPNGCLUTOverride(file, _screen);
						if (listEntry.viewpng) {
							listEntry.viewenh = (const byte *)listEntry.viewpng->getPixels();
							if (listEntry.viewenh) {
								listEntry.pixelsLength = listEntry.viewpng->w * listEntry.viewpng->h;
								listEntry.viewEnhanced = true;
								listEntry.enhancedIs256 = true;
							}
						}
					}
				}
			}

			if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".t." + twn + ".png").exists()) {
				{
					Common::String fileName = folder.getChild(fn + ".t." + twn + ".png").getName();
					Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
					if (!file) {
						debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
					} else {
						debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
						listEntry.viewpng = loadCelPNG(file);
						if (listEntry.viewpng) {
							listEntry.viewenh = (const byte *)listEntry.viewpng->getPixels();
							if (listEntry.viewenh) {
								listEntry.pixelsLength = listEntry.viewpng->w * listEntry.viewpng->h;
								listEntry.viewEnhanced = true;
								listEntry.enhancedIs256 = false;
							}
						}
					}
				}
			} else if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".t." + twn + "_256.png").exists()) {
		 {
					Common::String fileName = folder.getChild(fn + ".t." + twn + "_256.png").getName();
					Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
					if (!file) {
						debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
					} else {
						debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());

						listEntry.viewpng = loadCelPNGCLUT(file);
						if (listEntry.viewpng) {
							listEntry.viewenh = (const byte *)listEntry.viewpng->getPixels();
							if (listEntry.viewenh) {
								listEntry.pixelsLength = listEntry.viewpng->w * listEntry.viewpng->h;
								listEntry.viewEnhanced = true;
								listEntry.enhancedIs256 = true;
							}
						}
					}
				}
			} else if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".t." + twn + "_256RP.png").exists()) {
		 {
					Common::String fileName = folder.getChild(fn + ".t." + twn + "_256RP.png").getName();
					Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
					if (!file) {
						debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
					} else {
						debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
						listEntry.viewpng = loadCelPNGCLUTOverride(file, _screen);
						if (listEntry.viewpng) {
							listEntry.viewenh = (const byte *)listEntry.viewpng->getPixels();
							if (listEntry.viewenh) {
								listEntry.pixelsLength = listEntry.viewpng->w * listEntry.viewpng->h;
								listEntry.viewEnhanced = true;
								listEntry.enhancedIs256 = true;
							}
						}
					}
				}
			}
		}
		_list.push_back(listEntry);
	}
	
	
	// Possible TODO: As noted in the comment in sortHelper we actually
	// require a stable sorting algorithm here. Since Common::sort is not stable
	// at the time of writing this comment, we work around that in our ordering
	// comparator. If that changes in the future or we want to use some
	// stable sorting algorithm here, we should change that.
	// In that case we should test such changes intensively. A good place to test stable sort
	// is iceman, cupboard within the submarine. If sort isn't stable, the cupboard will be
	// half-open, half-closed. Of course that's just one of many special cases.

	// Now sort the list according y and z (descending)
	Common::sort(_list.begin(), _list.end(), sortHelper);
	_newList.clear();
}

void GfxAnimate::fill(byte &old_picNotValid) {
	GfxView *view = NULL;
	AnimateList::iterator it;
	const AnimateList::iterator end = _list.end();

	for (it = _list.begin(); it != end; ++it) {
		// Get the corresponding view
		view = _cache->getView(it->viewId);

		adjustInvalidCels(view, it);
		processViewScaling(view, it);
		setNsRect(view, it);

		//warning("%s view %d, loop %d, cel %d, signal %x", _s->_segMan->getObjectName(curObject), it->viewId, it->loopNo, it->celNo, it->signal);

		// Calculate current priority according to y-coordinate
		if (!(it->signal & kSignalFixedPriority)) {
			it->priority = _ports->kernelCoordinateToPriority(it->y);
			writeSelectorValue(_s->_segMan, it->object, SELECTOR(priority), it->priority);
		}

		if (it->signal & kSignalNoUpdate) {
			if ((it->signal & (kSignalForceUpdate | kSignalViewUpdated))
				||   (it->signal & kSignalHidden  && !(it->signal & kSignalRemoveView))
				|| (!(it->signal & kSignalHidden) &&   it->signal & kSignalRemoveView)
				||   (it->signal & kSignalAlwaysUpdate))
				old_picNotValid++;
			it->signal &= ~kSignalStopUpdate;
		} else {
			if ((it->signal & kSignalStopUpdate) || (it->signal & kSignalAlwaysUpdate))
				old_picNotValid++;
			it->signal &= ~kSignalForceUpdate;
		}
	}
}

void GfxAnimate::adjustInvalidCels(GfxView *view, AnimateList::iterator it) {
	// adjust loop and cel, if any of those is invalid
	//  this seems to be completely crazy code
	//  sierra sci checked signed int16 to be above or equal the counts and reseted to 0 in those cases
	//  later during view processing those are compared unsigned again and then set to maximum count - 1
	//  Games rely on this behavior. For example laura bow 1 has a knight standing around in room 37
	//   which has cel set to 3. This cel does not exist and the actual knight is 0
	//   In kq5 on the other hand during the intro, when the trunk is opened, cel is set to some real
	//   high number, which is negative when considered signed. This actually requires to get fixed to
	//   maximum cel, otherwise the trunk would be closed.
	int16 viewLoopCount = view->getLoopCount();
	if (it->loopNo >= viewLoopCount) {
		it->loopNo = 0;
		writeSelectorValue(_s->_segMan, it->object, SELECTOR(loop), it->loopNo);
	} else if (it->loopNo < 0) {
		it->loopNo = viewLoopCount - 1;
		// not setting selector is right, sierra sci didn't do it during view processing as well
	}
	int16 viewCelCount = view->getCelCount(it->loopNo);
	if (it->celNo >= viewCelCount) {
		it->celNo = 0;
		writeSelectorValue(_s->_segMan, it->object, SELECTOR(cel), it->celNo);
	} else if (it->celNo < 0) {
		it->celNo = viewCelCount - 1;
	}
}

void GfxAnimate::processViewScaling(GfxView *view, AnimateList::iterator it) {
	if (!view->isScaleable()) {
		// Laura Bow 2 (especially floppy) depends on this, some views are not supposed to be scaleable
		//  this "feature" was removed in later versions of SCI1.1
		it->scaleSignal = 0;
		it->scaleY = it->scaleX = 128;
	} else {
		// Process global scaling, if needed
		if (it->scaleSignal & kScaleSignalDoScaling) {
			if (it->scaleSignal & kScaleSignalGlobalScaling) {
				applyGlobalScaling(it, view);
			}
		}
	}
}

void GfxAnimate::applyGlobalScaling(AnimateList::iterator entry, GfxView *view) {
	// Global scaling uses global var 2 and some other stuff to calculate scaleX/scaleY
	int16 maxScale = readSelectorValue(_s->_segMan, entry->object, SELECTOR(maxScale));
	int16 celHeight = view->getHeight(entry->loopNo, entry->celNo);
	int16 maxCelHeight = (maxScale * celHeight) >> 7;
	reg_t globalVar2 = _s->variables[VAR_GLOBAL][kGlobalVarCurrentRoom]; // current room object
	int16 vanishingY = readSelectorValue(_s->_segMan, globalVar2, SELECTOR(vanishingY));

	int16 fixedPortY = _ports->getPort()->rect.bottom - vanishingY;
	int16 fixedEntryY = entry->y - vanishingY;
	if (!fixedEntryY)
		fixedEntryY = 1;

	if ((celHeight == 0) || (fixedPortY == 0))
		error("global scaling panic");

	entry->scaleY = ( maxCelHeight * fixedEntryY ) / fixedPortY;
	entry->scaleY = (entry->scaleY * 128) / celHeight;

	entry->scaleX = entry->scaleY;

	// and set objects scale selectors
	writeSelectorValue(_s->_segMan, entry->object, SELECTOR(scaleX), entry->scaleX);
	writeSelectorValue(_s->_segMan, entry->object, SELECTOR(scaleY), entry->scaleY);
}

void GfxAnimate::setNsRect(GfxView *view, AnimateList::iterator it) {
	bool shouldSetNsRect = true;

	// Create rect according to coordinates and given cel
	if (it->scaleSignal & kScaleSignalDoScaling) {
		if (!it->viewEnhanced) {
			view->getCelScaledRect(it->loopNo, it->celNo, it->x, it->y, it->z, it->scaleX, it->scaleY, it->celRect);
		} else {
			view->getCelScaledRectEnhanced(it->viewpng, it->viewEnhanced, it->loopNo, it->celNo, it->x, it->y, it->z, it->scaleX, it->scaleY, it->celRect);
		}
		// when being scaled, only set nsRect, if object will get drawn
		if ((it->signal & kSignalHidden) && !(it->signal & kSignalAlwaysUpdate))
			shouldSetNsRect = false;
	} else {
		//  This special handling is not included in the other SCI1.1 interpreters and MUST NOT be
		//  checked in those cases, otherwise we will break games (e.g. EcoQuest 2, room 200)
		if ((g_sci->getGameId() == GID_HOYLE4) && (it->scaleSignal & kScaleSignalHoyle4SpecialHandling)) {
			it->celRect = g_sci->_gfxCompare->getNSRect(it->object);
			view->getCelSpecialHoyle4Rect(it->loopNo, it->celNo, it->x, it->y, it->z, it->celRect);
			shouldSetNsRect = false;
		} else {
			if (!it->viewEnhanced) {
				view->getCelRect(it->loopNo, it->celNo, it->x, it->y, it->z, it->celRect);
			} else {
				view->getCelRectEnhanced(it->viewpng, it->viewEnhanced, it->loopNo, it->celNo, it->x, it->y, it->z, it->celRect);
			}
		}
	}

	if (shouldSetNsRect) {
		g_sci->_gfxCompare->setNSRect(it->object, it->celRect);
	}
}

void GfxAnimate::update() {
	reg_t bitsHandle;
	Common::Rect rect;
	AnimateList::iterator it;
	const AnimateList::iterator end = _list.end();

	// Remove all no-update cels, if requested
	for (it = _list.reverse_begin(); it != end; --it) {
		if (it->signal & kSignalNoUpdate) {
			if (!(it->signal & kSignalRemoveView)) {
				bitsHandle = readSelector(_s->_segMan, it->object, SELECTOR(underBits));
				if (_screen->_picNotValid != 1) {
					_paint16->bitsRestore(bitsHandle);
					it->showBitsFlag = true;
				} else	{
					_paint16->bitsFree(bitsHandle);
				}
				writeSelectorValue(_s->_segMan, it->object, SELECTOR(underBits), 0);
			}
			it->signal &= ~kSignalForceUpdate;
			if (it->signal & kSignalViewUpdated)
				it->signal &= ~(kSignalViewUpdated | kSignalNoUpdate);
		} else if (it->signal & kSignalStopUpdate) {
			it->signal &= ~kSignalStopUpdate;
			it->signal |= kSignalNoUpdate;
		}
	}

	// Draw always-update cels
	for (it = _list.begin(); it != end; ++it) {
		if (it->signal & kSignalAlwaysUpdate) {
			// draw corresponding cel
			_paint16->drawCel(it->viewpng, it->viewenh, it->pixelsLength, it->viewEnhanced, it->enhancedIs256, it->viewId, it->loopNo, it->celNo, it->tweenNo, it->celRect, it->priority, it->paletteNo, it->scaleX, it->scaleY);
			it->showBitsFlag = true;

			it->signal &= ~(kSignalStopUpdate | kSignalViewUpdated | kSignalNoUpdate | kSignalForceUpdate);
			if (!(it->signal & kSignalIgnoreActor)) {
				rect = it->celRect;
				rect.top = CLIP<int16>(_ports->kernelPriorityToCoordinate(it->priority) - 1, rect.top, rect.bottom - 1);
				_paint16->fillRect(rect, GFX_SCREEN_MASK_CONTROL, 0, 0, 15);
			}
		}
	}

	// Saving background for all NoUpdate-cels
	for (it = _list.begin(); it != end; ++it) {
		if (it->signal & kSignalNoUpdate) {
			if (it->signal & kSignalHidden) {
				it->signal |= kSignalRemoveView;
			} else {
				it->signal &= ~kSignalRemoveView;
				if (it->signal & kSignalIgnoreActor)
					bitsHandle = _paint16->bitsSave(it->celRect, GFX_SCREEN_MASK_VISUAL|GFX_SCREEN_MASK_PRIORITY);
				else
					bitsHandle = _paint16->bitsSave(it->celRect, GFX_SCREEN_MASK_ALL);
				writeSelector(_s->_segMan, it->object, SELECTOR(underBits), bitsHandle);
			}
		}
	}

	// Draw NoUpdate cels
	for (it = _list.begin(); it != end; ++it) {
		if (it->signal & kSignalNoUpdate && !(it->signal & kSignalHidden)) {
			// draw corresponding cel
			
			_paint16->drawCel(it->viewpng, it->viewenh, it->pixelsLength, it->viewEnhanced, it->enhancedIs256, it->viewId, it->loopNo, it->celNo, it->tweenNo, it->celRect, it->priority, it->paletteNo, it->scaleX, it->scaleY);
			it->showBitsFlag = true;

			if (!(it->signal & kSignalIgnoreActor)) {
				rect = it->celRect;
				rect.top = CLIP<int16>(_ports->kernelPriorityToCoordinate(it->priority) - 1, rect.top, rect.bottom - 1);
				_paint16->fillRect(rect, GFX_SCREEN_MASK_CONTROL, 0, 0, 15);
			}
		}
	}
}

void GfxAnimate::drawCels() {
	reg_t bitsHandle;
	AnimateList::iterator it;
	const AnimateList::iterator end = _list.end();
	_lastCastData.clear();

	for (it = _list.begin(); it != end; ++it) {
		if (!(it->signal & (kSignalNoUpdate | kSignalHidden | kSignalAlwaysUpdate))) {
			// Save background
			bitsHandle = _paint16->bitsSave(it->celRect, GFX_SCREEN_MASK_ALL);
			writeSelector(_s->_segMan, it->object, SELECTOR(underBits), bitsHandle);
			// draw corresponding cel
			

				Common::FSNode folder;
			if (ConfMan.hasKey("extrapath")) {
				Common::String fn = "view.";
				char viewNoStr[5];
				sprintf(viewNoStr, "%u", it->viewId);
				for (int n = 0; n < 5; n++) {
					if (viewNoStr[n] >= '0' && viewNoStr[n] <= '9') {
						fn += viewNoStr[n];
					}
				}
				fn += ".";
				char loopNoStr[5];
				sprintf(loopNoStr, "%u", it->loopNo);
				for (int n = 0; n < 5; n++) {
					if (loopNoStr[n] >= '0' && loopNoStr[n] <= '9') {
						fn += loopNoStr[n];
					}
				}
				fn += ".";
				char celNoStr[5];
				sprintf(celNoStr, "%u", it->celNo);
				for (int n = 0; n < 5; n++) {
					if (celNoStr[n] >= '0' && celNoStr[n] <= '9') {
						fn += celNoStr[n];
					}
				}
				char tweenNoStr[5];
				Common::String twn = "";
				sprintf(tweenNoStr, "%u", it->tweenNo);
				for (int n = 0; n < 5; n++) {
					if (tweenNoStr[n] >= '0' && tweenNoStr[n] <= '9') {
						twn += tweenNoStr[n];
					}
				}
				debug(fn.c_str());
				if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".png").exists()) {
					if (!it->viewEnhanced) {
						Common::String fileName = folder.getChild(fn + ".png").getName();
						Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
						if (!file) {
							debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
						} else {
							debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
							it->viewpng = loadCelPNG(file);
							if (it->viewpng) {
								it->viewenh = (const byte *)it->viewpng->getPixels();
								if (it->viewenh) {
									it->pixelsLength = it->viewpng->w * it->viewpng->h;
									it->viewEnhanced = true;
									it->enhancedIs256 = false;
								}
							}
						}
					}
				}
				if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + "_256.png").exists()) {
					if (!it->viewEnhanced) {
						Common::String fileName = folder.getChild(fn + "_256.png").getName();
						Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
						if (!file) {
							debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
						} else {
							debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
							it->viewpng = loadCelPNGCLUT(file);
							if (it->viewpng) {
								it->viewenh = (const byte *)it->viewpng->getPixels();
								if (it->viewenh) {
									it->pixelsLength = it->viewpng->w * it->viewpng->h;
									it->viewEnhanced = true;
									it->enhancedIs256 = true;
								}
							}
						}
					}
				}
				if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + "_256RP.png").exists()) {
					if (!it->viewEnhanced) {
						Common::String fileName = folder.getChild(fn + "_256RP.png").getName();
						Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
						if (!file) {
							debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
						} else {
							debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
							it->viewpng = loadCelPNGCLUTOverride(file, _screen);
							if (it->viewpng) {
								it->viewenh = (const byte *)it->viewpng->getPixels();
								if (it->viewenh) {
									it->pixelsLength = it->viewpng->w * it->viewpng->h;
									it->viewEnhanced = true;
									it->enhancedIs256 = true;
								}
							}
						}
					}
				}
				if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".t." + twn + ".png").exists()) {
					if (!it->viewEnhanced) {
						Common::String fileName = folder.getChild(fn + ".t." + twn + ".png").getName();
						Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
						if (!file) {
							debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
						} else {
							debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
							it->viewpng = loadCelPNG(file);
							if (it->viewpng) {
								it->viewenh = (const byte *)it->viewpng->getPixels();
								if (it->viewenh) {
									it->pixelsLength = it->viewpng->w * it->viewpng->h;
									it->viewEnhanced = true;
									it->enhancedIs256 = false;
								}
							}
						}
					}
				}
				if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".t." + twn + "_256.png").exists()) {
					if (!it->viewEnhanced) {
						Common::String fileName = folder.getChild(fn + ".t." + twn + "_256.png").getName();
						Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
						if (!file) {
							debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
						} else {
							debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());

							it->viewpng = loadCelPNGCLUT(file);
							if (it->viewpng) {
								it->viewenh = (const byte *)it->viewpng->getPixels();
								if (it->viewenh) {
									it->pixelsLength = it->viewpng->w * it->viewpng->h;
									it->viewEnhanced = true;
									it->enhancedIs256 = true;
								}
							}
						}
					}
				}
				if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".t." + twn + "_256RP.png").exists()) {
					if (!it->viewEnhanced) {
						Common::String fileName = folder.getChild(fn + ".t." + twn + "_256RP.png").getName();
						Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
						if (!file) {
							debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
						} else {
							debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
							it->viewpng = loadCelPNGCLUTOverride(file, _screen);
							if (it->viewpng) {
								it->viewenh = (const byte *)it->viewpng->getPixels();
								if (it->viewenh) {
									it->pixelsLength = it->viewpng->w * it->viewpng->h;
									it->viewEnhanced = true;
									it->enhancedIs256 = true;
								}
							}
						}
					}
				}
			}
			_paint16->drawCel(it->viewpng, it->viewenh, it->pixelsLength, it->viewEnhanced, it->enhancedIs256, it->viewId, it->loopNo, it->celNo, it->tweenNo, it->celRect, it->priority, it->paletteNo, it->scaleX, it->scaleY, it->scaleSignal);
			it->showBitsFlag = true;

			if (it->signal & kSignalRemoveView)
				it->signal &= ~kSignalRemoveView;

			// Remember that entry in lastCast
			_lastCastData.push_back(*it);
		}
	}
}

void GfxAnimate::updateScreen(byte oldPicNotValid) {
	AnimateList::iterator it;
	const AnimateList::iterator end = _list.end();
	Common::Rect lsRect;
	Common::Rect workerRect;

	for (it = _list.begin(); it != end; ++it) {
		if (it->showBitsFlag || !(it->signal & (kSignalRemoveView | kSignalNoUpdate) ||
										(!(it->signal & kSignalRemoveView) && (it->signal & kSignalNoUpdate) && oldPicNotValid))) {
			lsRect.left = readSelectorValue(_s->_segMan, it->object, SELECTOR(lsLeft));
			lsRect.top = readSelectorValue(_s->_segMan, it->object, SELECTOR(lsTop));
			lsRect.right = readSelectorValue(_s->_segMan, it->object, SELECTOR(lsRight));
			lsRect.bottom = readSelectorValue(_s->_segMan, it->object, SELECTOR(lsBottom));

			workerRect = lsRect;
			workerRect.clip(it->celRect);

			if (!workerRect.isEmpty()) {
				workerRect = lsRect;
				workerRect.extend(it->celRect);
			} else {
				_paint16->bitsShow(lsRect);
				workerRect = it->celRect;
			}
			writeSelectorValue(_s->_segMan, it->object, SELECTOR(lsLeft), it->celRect.left);
			writeSelectorValue(_s->_segMan, it->object, SELECTOR(lsTop), it->celRect.top);
			writeSelectorValue(_s->_segMan, it->object, SELECTOR(lsRight), it->celRect.right);
			writeSelectorValue(_s->_segMan, it->object, SELECTOR(lsBottom), it->celRect.bottom);
			// may get used for debugging
			//_paint16->frameRect(workerRect);
			_paint16->bitsShow(workerRect);

			if (it->signal & kSignalHidden)
				it->signal |= kSignalRemoveView;
		}
	}
	// use this for debug purposes
	// _screen->copyToScreen();
}

void GfxAnimate::restoreAndDelete(int argc, reg_t *argv) {
	AnimateList::iterator it;
	const AnimateList::iterator end = _list.end();

	// This has to be done in a separate loop. At least in sq1 some .dispose
	// modifies FIXEDLOOP flag in signal for another object. In that case we
	// would overwrite the new signal with our version of the old signal.
	for (it = _list.begin(); it != end; ++it) {
		// Finally update signal
		writeSelectorValue(_s->_segMan, it->object, SELECTOR(signal), it->signal);
	}

	for (it = _list.reverse_begin(); it != end; --it) {
		// We read out signal here again, this is not by accident but to ensure
		// that we got an up-to-date signal
		it->signal = readSelectorValue(_s->_segMan, it->object, SELECTOR(signal));

		if ((it->signal & (kSignalNoUpdate | kSignalRemoveView)) == 0) {
			_paint16->bitsRestore(readSelector(_s->_segMan, it->object, SELECTOR(underBits)));
			writeSelectorValue(_s->_segMan, it->object, SELECTOR(underBits), 0);
		}

		if (it->signal & kSignalDisposeMe) {
			// Call .delete_ method of that object
			invokeSelector(_s, it->object, SELECTOR(delete_), argc, argv, 0);
		}
	}
}

void GfxAnimate::reAnimate(Common::Rect rect) {
	if (!_lastCastData.empty()) {
		AnimateArray::iterator it;
		AnimateArray::iterator end = _lastCastData.end();
		for (it = _lastCastData.begin(); it != end; ++it) {
			it->castHandle = _paint16->bitsSave(it->celRect, GFX_SCREEN_MASK_VISUAL|GFX_SCREEN_MASK_PRIORITY);
			

				Common::FSNode folder;
			if (ConfMan.hasKey("extrapath")) {
				Common::String fn = "view.";
				char viewNoStr[5];
				sprintf(viewNoStr, "%u", it->viewId);
				for (int n = 0; n < 5; n++) {
					if (viewNoStr[n] >= '0' && viewNoStr[n] <= '9') {
						fn += viewNoStr[n];
					}
				}
				fn += ".";
				char loopNoStr[5];
				sprintf(loopNoStr, "%u", it->loopNo);
				for (int n = 0; n < 5; n++) {
					if (loopNoStr[n] >= '0' && loopNoStr[n] <= '9') {
						fn += loopNoStr[n];
					}
				}
				fn += ".";
				char celNoStr[5];
				sprintf(celNoStr, "%u", it->celNo);
				for (int n = 0; n < 5; n++) {
					if (celNoStr[n] >= '0' && celNoStr[n] <= '9') {
						fn += celNoStr[n];
					}
				}
				char tweenNoStr[5];
				Common::String twn = "";
				sprintf(tweenNoStr, "%u", it->tweenNo);
				for (int n = 0; n < 5; n++) {
					if (tweenNoStr[n] >= '0' && tweenNoStr[n] <= '9') {
						twn += tweenNoStr[n];
					}
				}
				debug(fn.c_str());

				if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".png").exists()) {
					if (!it->viewEnhanced) {
						Common::String fileName = folder.getChild(fn + ".png").getName();
						Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
						if (!file) {
							debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
						} else {
							debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
							it->viewpng = loadCelPNG(file);
							if (it->viewpng) {
								it->viewenh = (const byte *)it->viewpng->getPixels();
								if (it->viewenh) {
									it->pixelsLength = it->viewpng->w * it->viewpng->h;
									it->viewEnhanced = true;
									it->enhancedIs256 = false;
								}
							}
						}
					}
				}
				if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + "_256.png").exists()) {
					if (!it->viewEnhanced) {
						Common::String fileName = folder.getChild(fn + "_256.png").getName();
						Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
						if (!file) {
							debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
						} else {
							debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
							it->viewpng = loadCelPNGCLUT(file);
							if (it->viewpng) {
								it->viewenh = (const byte *)it->viewpng->getPixels();
								if (it->viewenh) {
									it->pixelsLength = it->viewpng->w * it->viewpng->h;
									it->viewEnhanced = true;
									it->enhancedIs256 = true;
								}
							}
						}
					}
				}
				if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + "_256RP.png").exists()) {
					if (!it->viewEnhanced) {
						Common::String fileName = folder.getChild(fn + "_256RP.png").getName();
						Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
						if (!file) {
							debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
						} else {
							debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
							it->viewpng = loadCelPNGCLUTOverride(file, _screen);
							if (it->viewpng) {
								it->viewenh = (const byte *)it->viewpng->getPixels();
								if (it->viewenh) {
									it->pixelsLength = it->viewpng->w * it->viewpng->h;
									it->viewEnhanced = true;
									it->enhancedIs256 = true;
								}
							}
						}
					}
				}
				if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".t." + twn + ".png").exists()) {
					if (!it->viewEnhanced) {
						Common::String fileName = folder.getChild(fn + ".t." + twn + ".png").getName();
						Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
						if (!file) {
							debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
						} else {
							debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
							it->viewpng = loadCelPNG(file);
							if (it->viewpng) {
								it->viewenh = (const byte *)it->viewpng->getPixels();
								if (it->viewenh) {
									it->pixelsLength = it->viewpng->w * it->viewpng->h;
									it->viewEnhanced = true;
									it->enhancedIs256 = false;
								}
							}
						}
					}
				}
				if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".t." + twn + "_256.png").exists()) {
					if (!it->viewEnhanced) {
						Common::String fileName = folder.getChild(fn + ".t." + twn + "_256.png").getName();
						Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
						if (!file) {
							debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
						} else {
							debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());

							it->viewpng = loadCelPNGCLUT(file);
							if (it->viewpng) {
								it->viewenh = (const byte *)it->viewpng->getPixels();
								if (it->viewenh) {
									it->pixelsLength = it->viewpng->w * it->viewpng->h;
									it->viewEnhanced = true;
									it->enhancedIs256 = true;
								}
							}
						}
					}
				}
				if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".t." + twn + "_256RP.png").exists()) {
					if (!it->viewEnhanced) {
						Common::String fileName = folder.getChild(fn + ".t." + twn + "_256RP.png").getName();
						Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
						if (!file) {
							debug("Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
						} else {
							debug("Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
							it->viewpng = loadCelPNGCLUTOverride(file, _screen);
							if (it->viewpng) {
								it->viewenh = (const byte *)it->viewpng->getPixels();
								if (it->viewenh) {
									it->pixelsLength = it->viewpng->w * it->viewpng->h;
									it->viewEnhanced = true;
									it->enhancedIs256 = true;
								}
							}
						}
					}
				}
			}
			
			_paint16->drawCel(it->viewpng, it->viewenh, it->pixelsLength, it->viewEnhanced, it->enhancedIs256, it->viewId, it->loopNo, it->celNo, it->tweenNo, it->celRect, it->priority, it->paletteNo, it->scaleX, it->scaleY);
		}
		_paint16->bitsShow(rect);
		// restoring
		while (it != _lastCastData.begin()) {		// FIXME: HACK, this iterator use is not very safe
			it--;
			_paint16->bitsRestore(it->castHandle);
		}
	} else {
		_paint16->bitsShow(rect);
	}
}

void GfxAnimate::addToPicDrawCels() {
	reg_t curObject;
	GfxView *view = NULL;
	AnimateList::iterator it;
	const AnimateList::iterator end = _list.end();

	for (it = _list.begin(); it != end; ++it) {
		curObject = it->object;

		// Get the corresponding view
		view = _cache->getView(it->viewId);

		// kAddToPic does not do loop/cel-number fixups

		if (it->priority == -1)
			it->priority = _ports->kernelCoordinateToPriority(it->y);

		if (!view->isScaleable()) {
			// Laura Bow 2 specific - Check fill() below
			it->scaleSignal = 0;
			it->scaleY = it->scaleX = 128;
		}

		// Create rect according to coordinates and given cel
		if (it->scaleSignal & kScaleSignalDoScaling) {
			if (it->scaleSignal & kScaleSignalGlobalScaling) {
				applyGlobalScaling(it, view);
			}
			if (!it->viewEnhanced) {
				view->getCelScaledRect(it->loopNo, it->celNo, it->x, it->y, it->z, it->scaleX, it->scaleY, it->celRect);
			} else {
				view->getCelScaledRectEnhanced(it->viewpng, it->viewEnhanced, it->loopNo, it->celNo, it->x, it->y, it->z, it->scaleX, it->scaleY, it->celRect);
			}
			g_sci->_gfxCompare->setNSRect(curObject, it->celRect);
		} else {
			if (!it->viewEnhanced) {
				view->getCelRect(it->loopNo, it->celNo, it->x, it->y, it->z, it->celRect);
			} else {
				view->getCelRectEnhanced(it->viewpng, it->viewEnhanced, it->loopNo, it->celNo, it->x, it->y, it->z, it->celRect);
			}
		}

		// draw corresponding cel
		_paint16->drawCel(it->viewpng, it->viewenh, it->pixelsLength, it->viewEnhanced, it->enhancedIs256, view, it->loopNo, it->celNo, 0, it->celRect, it->priority, it->paletteNo, it->scaleX, it->scaleY);
		if (!(it->signal & kSignalIgnoreActor)) {
			it->celRect.top = CLIP<int16>(_ports->kernelPriorityToCoordinate(it->priority) - 1, it->celRect.top, it->celRect.bottom - 1);
			_paint16->fillRect(it->celRect, GFX_SCREEN_MASK_CONTROL, 0, 0, 15);
		}
	}
}

void GfxAnimate::addToPicDrawView(GuiResourceId viewId, int16 viewNo, int16 loopNo, int16 celNo, int16 x, int16 y, int16 priority, int16 control) {
	GfxView *view = _cache->getView(viewId);
	Common::Rect celRect;

	if (priority == -1)
		priority = _ports->kernelCoordinateToPriority(y);

	AnimateEntry listEntry;

		Common::FSNode folder;
		if (ConfMan.hasKey("extrapath")) {
			Common::String fn = "view.";
			char viewNoStr[5];
			sprintf(viewNoStr, "%u", viewId);
			for (int n = 0; n < 5; n++) {
				if (viewNoStr[n] >= '0' && viewNoStr[n] <= '9') {
					fn += viewNoStr[n];
				}
			}
			fn += ".";
			char loopNoStr[5];
			sprintf(loopNoStr, "%u", loopNo);
			for (int n = 0; n < 5; n++) {
				if (loopNoStr[n] >= '0' && loopNoStr[n] <= '9') {
					fn += loopNoStr[n];
				}
			}
			fn += ".";
			char celNoStr[5];
			sprintf(celNoStr, "%u", celNo);
			for (int n = 0; n < 5; n++) {
				if (celNoStr[n] >= '0' && celNoStr[n] <= '9') {
					fn += celNoStr[n];
				}
			}
			char tweenNoStr[5];
			Common::String twn = "";
			sprintf(tweenNoStr, "%u", 0);
			for (int n = 0; n < 5; n++) {
				if (tweenNoStr[n] >= '0' && tweenNoStr[n] <= '9') {
					twn += tweenNoStr[n];
				}
			}
			debug(fn.c_str());
			if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".png").exists()) {
				Common::String fileName = folder.getChild(fn + ".png").getName();
				Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
				if (!file) {
					debug(10, "Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
				} else {
					debug(10, "Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
					listEntry.viewpng = loadCelPNG(file);
					if (listEntry.viewpng) {
						listEntry.viewenh = (const byte *)listEntry.viewpng->getPixels();
						if (listEntry.viewenh) {
							listEntry.pixelsLength = listEntry.viewpng->w * listEntry.viewpng->h;
							listEntry.viewEnhanced = true;
							listEntry.enhancedIs256 = false;
						}
					}
				}
			}
			if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + "_256.png").exists()) {
				Common::String fileName = folder.getChild(fn + "_256.png").getName();
				Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
				if (!file) {
					debug(10, "Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
				} else {
					debug(10, "Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
					listEntry.viewpng = loadCelPNGCLUT(file);
					if (listEntry.viewpng) {
						listEntry.viewenh = (const byte *)listEntry.viewpng->getPixels();
						if (listEntry.viewenh) {
							listEntry.pixelsLength = listEntry.viewpng->w * listEntry.viewpng->h;
							listEntry.viewEnhanced = true;
							listEntry.enhancedIs256 = true;
						}
					}
				}
			}
			if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + "_256RP.png").exists()) {
				Common::String fileName = folder.getChild(fn + "_256RP.png").getName();
				Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
				if (!file) {
					debug(10, "Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
				} else {
					debug(10, "Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
					listEntry.viewpng = loadCelPNGCLUTOverride(file, _screen);
					if (listEntry.viewpng) {
						listEntry.viewenh = (const byte *)listEntry.viewpng->getPixels();
						if (listEntry.viewenh) {
							listEntry.pixelsLength = listEntry.viewpng->w * listEntry.viewpng->h;
							listEntry.viewEnhanced = true;
							listEntry.enhancedIs256 = true;
						}
					}
				}
			}
			if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".t." + twn + ".png").exists()) {
				Common::String fileName = folder.getChild(fn + ".t." + twn + ".png").getName();
				Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
				if (!file) {
					debug(10, "Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
				} else {
					debug(10, "Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
					listEntry.viewpng = loadCelPNG(file);
					if (listEntry.viewpng) {
						listEntry.viewenh = (const byte *)listEntry.viewpng->getPixels();
						if (listEntry.viewenh) {
							listEntry.pixelsLength = listEntry.viewpng->w * listEntry.viewpng->h;
							listEntry.viewEnhanced = true;
							listEntry.enhancedIs256 = false;
						}
					}
				}
			}
			if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".t." + twn + "_256.png").exists()) {
				Common::String fileName = folder.getChild(fn + ".t." + twn + "_256.png").getName();
				Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
				if (!file) {
					debug(10, "Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
				} else {
					debug(10, "Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());

					listEntry.viewpng = loadCelPNGCLUT(file);
					if (listEntry.viewpng) {
						listEntry.viewenh = (const byte *)listEntry.viewpng->getPixels();
						if (listEntry.viewenh) {
							listEntry.pixelsLength = listEntry.viewpng->w * listEntry.viewpng->h;
							listEntry.viewEnhanced = true;
							listEntry.enhancedIs256 = true;
						}
					}
				}
			}
			if ((folder = Common::FSNode(ConfMan.get("extrapath"))).exists() && folder.getChild(fn + ".t." + twn + "_256RP.png").exists()) {
				Common::String fileName = folder.getChild(fn + ".t." + twn + "_256RP.png").getName();
				Common::SeekableReadStream *file = SearchMan.createReadStreamForMember(fileName);
				if (!file) {
					debug(10, "Enhanced Bitmap %s DOES NOT EXIST, yet would have been loaded.. 2", fileName.c_str());
				} else {
					debug(10, "Enhanced Bitmap %s EXISTS, and has been loaded..", fileName.c_str());
					listEntry.viewpng = loadCelPNGCLUTOverride(file, _screen);
					if (listEntry.viewpng) {
						listEntry.viewenh = (const byte *)listEntry.viewpng->getPixels();
						if (listEntry.viewenh) {
							listEntry.pixelsLength = listEntry.viewpng->w * listEntry.viewpng->h;
							listEntry.viewEnhanced = true;
							listEntry.enhancedIs256 = true;
						}
					}
				}
			}
		}
	    // Create rect according to coordinates and given cel
	    view->getCelRectEnhanced(listEntry.viewpng, listEntry.viewEnhanced,loopNo, celNo, x, y, 0, celRect);
	_paint16->drawCel(listEntry.viewpng, listEntry.viewenh, listEntry.pixelsLength, listEntry.viewEnhanced, listEntry.enhancedIs256, view, loopNo, celNo, 0, celRect, priority, 0);

	if (control != -1) {
		celRect.top = CLIP<int16>(_ports->kernelPriorityToCoordinate(priority) - 1, celRect.top, celRect.bottom - 1);
		_paint16->fillRect(celRect, GFX_SCREEN_MASK_CONTROL, 0, 0, control);
	}
}


void GfxAnimate::animateShowPic() {
	Port *picPort = _ports->_picWind;
	Common::Rect picRect = picPort->rect;
	bool previousCursorState = _cursor->isVisible();

	if (previousCursorState)
		_cursor->kernelHide();
	// Adjust picRect to become relative to screen
	picRect.translate(picPort->left, picPort->top);
	_transitions->doit(picRect);
	if (previousCursorState)
		_cursor->kernelShow();
}

void GfxAnimate::kernelAnimate(reg_t listReference, bool cycle, int argc, reg_t *argv) {
	// If necessary, delay this kAnimate for a running PalVary.
	// See delayForPalVaryWorkaround() for details.
	if (_screen->_picNotValid)
		_palette->delayForPalVaryWorkaround();

	byte old_picNotValid = _screen->_picNotValid;

	if (getSciVersion() >= SCI_VERSION_1_1)
		_palette->palVaryUpdate();

	if (listReference.isNull()) {
		disposeLastCast();
		if (_screen->_picNotValid)
			animateShowPic();
		return;
	}

	List *list = _s->_segMan->lookupList(listReference);
	if (!list)
		error("kAnimate called with non-list as parameter");

	if (cycle) {
		if (!invoke(list, argc, argv))
			return;

		// Look up the list again, as it may have been modified
		list = _s->_segMan->lookupList(listReference);
	}

	Port *oldPort = _ports->setPort((Port *)_ports->_picWind);
	disposeLastCast();

	makeSortedList(list);
	fill(old_picNotValid);

	if (old_picNotValid) {
		// beginUpdate()/endUpdate() were introduced SCI1.
		// Calling those for SCI0 will work most of the time but breaks minor
		// stuff like percentage bar of qfg1ega at the character skill screen.
		if (getSciVersion() >= SCI_VERSION_1_EGA_ONLY)
			_ports->beginUpdate(_ports->_picWind);
		update();
		if (getSciVersion() >= SCI_VERSION_1_EGA_ONLY)
			_ports->endUpdate(_ports->_picWind);
	}

	drawCels();

	if (_screen->_picNotValid)
		animateShowPic();

	updateScreen(old_picNotValid);
	restoreAndDelete(argc, argv);

	// We update the screen here as well, some scenes like EQ1 credits run w/o calling kGetEvent thus we wouldn't update
	//  screen at all
	g_sci->getEventManager()->updateScreen();

	_ports->setPort(oldPort);

	// Now trigger speed throttler
	throttleSpeed();
}

void GfxAnimate::throttleSpeed() {
	switch (_lastCastData.size()) {
	case 0:
		// No entries drawn -> no speed throttler triggering
		break;
	case 1: {

		// One entry drawn -> check if that entry was a speed benchmark view, if not enable speed throttler
		AnimateEntry *onlyCast = &_lastCastData[0];
		if ((onlyCast->viewId == 0) && (onlyCast->loopNo == 13) && (onlyCast->celNo == 0)) {
			// this one is used by jones talkie
			if ((onlyCast->celRect.height() == 8) && (onlyCast->celRect.width() == 8)) {
				_s->_gameIsBenchmarking = true;
				return;
			}
		}
		// first loop and first cel used?
		if ((onlyCast->loopNo == 0) && (onlyCast->celNo == 0)) {
			// and that cel has a known speed benchmark resolution
			int16 onlyHeight = onlyCast->celRect.height();
			int16 onlyWidth = onlyCast->celRect.width();
			if (((onlyWidth == 12) && (onlyHeight == 35)) || // regular benchmark view ("fred", "Speedy", "ego")
				((onlyWidth == 29) && (onlyHeight == 45)) || // King's Quest 5 french "fred"
				((onlyWidth == 1) && (onlyHeight == 5)) || // Freddy Pharkas "fred"
				((onlyWidth == 1) && (onlyHeight == 1))) { // Laura Bow 2 Talkie
				// check further that there is only one cel in that view
				GfxView *onlyView = _cache->getView(onlyCast->viewId);
				if ((onlyView->getLoopCount() == 1) && (onlyView->getCelCount(0))) {
					_s->_gameIsBenchmarking = true;
					return;
				}
			}
		}
		_s->_gameIsBenchmarking = false;
		_s->_throttleTrigger = true;
		break;
	}
	default:
		// More than 1 entry drawn -> time for speed throttling
		_s->_gameIsBenchmarking = false;
		_s->_throttleTrigger = true;
		break;
	}
}

void GfxAnimate::addToPicSetPicNotValid() {
	if (getSciVersion() <= SCI_VERSION_1_EARLY)
		_screen->_picNotValid = 1;
	else
		_screen->_picNotValid = 2;
}

void GfxAnimate::kernelAddToPicList(reg_t listReference, int argc, reg_t *argv) {
	List *list;

	_ports->setPort((Port *)_ports->_picWind);

	list = _s->_segMan->lookupList(listReference);
	if (!list)
		error("kAddToPic called with non-list as parameter");

	makeSortedList(list);
	addToPicDrawCels();

	addToPicSetPicNotValid();
}

void GfxAnimate::kernelAddToPicView(GuiResourceId viewId, int16 viewNo, int16 loopNo, int16 celNo, int16 x, int16 y, int16 priority, int16 control) {
	_ports->setPort((Port *)_ports->_picWind);
	addToPicDrawView(viewId, viewNo, loopNo, celNo, x, y, priority, control);
	addToPicSetPicNotValid();
}

void GfxAnimate::printAnimateList(Console *con) {
	AnimateList::iterator it;
	const AnimateList::iterator end = _list.end();

	for (it = _list.begin(); it != end; ++it) {
		Script *scr = _s->_segMan->getScriptIfLoaded(it->object.getSegment());
		int16 scriptNo = scr ? scr->getScriptNumber() : -1;

		con->debugPrintf("%04x:%04x (%s), script %d, view %d (%d, %d), pal %d, "
			"at %d, %d, scale %d, %d / %d (z: %d, prio: %d, shown: %d, signal: %d)\n",
			PRINT_REG(it->object), _s->_segMan->getObjectName(it->object),
			scriptNo, it->viewId, it->loopNo, it->celNo, it->paletteNo,
			it->x, it->y, it->scaleX, it->scaleY, it->scaleSignal,
			it->z, it->priority, it->showBitsFlag, it->signal);
	}
}

} // End of namespace Sci
