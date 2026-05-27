
#pragma once


#define cinder	MSA			// so cinder namespaces are also accessible within MSA


#ifndef	AAA_AAA_TYPE_H
#	include "code_utils/aaa_type.h"
#endif

//#include "ofMain.h"
//typedef UINT32	uint32_t;
//typedef	UINT8	uint8_t;
//typedef __int32           int32_t;
//typedef unsigned __int8   uint8_t;
//typedef unsigned __int16  uint16_t;
//typedef unsigned __int32  uint32_t;

#include "cinder-lite/CinderMath.h"
#include "cinder-lite/Color.h"
#include "cinder-lite/Vector.h"

#include <string>
#include <vector>
#include <map>

#include "gol/gol.h"
#include "math/rand.h"

using namespace std;

namespace MSA {
	
#define MSA_HOST_SUFFIX		"-AAASeed"
	
#if defined (TARGET_OSX)
#define MSA_TARGET_OSX
	
#elif defined (TARGET_LINUX)
#define MSA_TARGET_LINUX
	
#elif defined (TARGET_WIN32)
#define MSA_TARGET_WIN32
	
#elif defined (TARGET_IPHONE)
#define MSA_TARGET_IPHONE)
#endif
	
#if defined (TARGET_OPENGLES)
#define MSA_TARGET_OPENGLES
#endif
	
/*
inline string dataPath(string path, bool absolute = false)		{	return ofToDataPath(path, absolute);	}
	
	inline double getElapsedSeconds()								{	return ofGetElapsedTimef(); }
	inline long int getElapsedFrames()								{	return ofGetFrameNum(); }
*/	
#if AAASEED()
	inline int getWindowWidth()										{	return 640;	}
	inline int getWindowHeight()									{	return 480;	}
#else
	inline int getWindowWidth()										{	return ofGetWidth();  }
	inline int getWindowHeight()									{	return ofGetHeight(); }
#endif
/*
	inline float getWindowAspectRatio()								{	return getWindowWidth() * 1.0f / getWindowHeight(); }
	inline Vec2f getWindowSize()									{	return Vec2f(getWindowWidth(), getWindowHeight()); }
	inline Vec2f getWindowCenter()									{	return Vec2f(getWindowWidth() * 0.5f, getWindowHeight() * 0.5f ); } 
	
	inline void drawString(string s, float x, float y)				{	ofDrawBitmapString(s, x, y); }
*/	
	
	class Rand
	{
	private:
		static	::c_rand_lin		msa_rand;
		static	float ofRandomf()	{	return msa_rand.get_fp32_01();	}
	public:
		FINLINE static float randFloat()									{	return msa_rand.get_fp32_01();		}
		FINLINE	static float randFloat(float f)								{	return msa_rand.get_fp32_max(f);	}
		//static float randFloat(float a, float b)							{	return ofRandom(a, b);	}
		
/*
		//! returns a random Vec3f that represents a point on the unit circle
		static Vec3f randVec3f() {
			float phi = randFloat( (float)M_PI * 2.0f );
			float costheta = randFloat( -1.0f, 1.0f );
			
			float rho = sqrt( 1.0f - costheta * costheta ); 
			float x = rho * cos( phi );
			float y = rho * sin( phi );
			float z = costheta;
			
			return Vec3f( x, y, z );
		}
		
		//! returns a random Vec2f that represents a point on the unit circle
		static Vec2f randVec2f() {
			float theta = randFloat( (float)M_PI * 2.0f );
			return Vec2f( cos( theta ), sin( theta ) );
		}
*/
	};	
	
}

namespace ci = MSA;			// for compatibility
