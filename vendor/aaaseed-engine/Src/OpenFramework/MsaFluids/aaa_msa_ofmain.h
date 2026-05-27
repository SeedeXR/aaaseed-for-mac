#ifndef OF_MAIN
#define OF_MAIN

//--------------------------
// utils
#include "aaa_msa_ofConstants.h"
#if !AAASEED()
#include "ofMath.h"
#include "ofUtils.h"
#endif
#include "openframeworks/utils/ofTypes.h"

//--------------------------
// communication
#ifndef TARGET_OF_IPHONE
#if !AAASEED()
	#include "ofSerial.h"
	#include "ofStandardFirmata.h"
	#include "ofArduino.h"
#endif
#endif

//--------------------------
// graphics
#if !AAASEED()
#include "graphics/ofTexture.h"
#include "ofTrueTypeFont.h"
#include "ofGraphics.h"
#include "ofImage.h"
#endif

//--------------------------
// app
#if !AAASEED()
#include "ofBaseApp.h"
#include "ofAppRunner.h"
#endif

//--------------------------
// audio
#if !AAASEED()
#include "ofSoundStream.h"
#include "ofSoundPlayer.h"
#endif

//--------------------------
// video
#ifndef TARGET_OF_IPHONE			//(temp for now, until this is ported)
#if !AAASEED()
	#include "ofVideoGrabber.h"
	#include "ofVideoPlayer.h"
#endif
#endif

//--------------------------
// events
#if !AAASEED()
#include "ofEvents.h"
#endif

#endif
