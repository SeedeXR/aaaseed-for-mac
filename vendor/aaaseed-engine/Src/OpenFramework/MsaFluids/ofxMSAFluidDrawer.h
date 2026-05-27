/***********************************************************************

This class is for drawing a fluidsolver using the OpenFrameworks texture

/***********************************************************************

Copyright (c) 2008, 2009, Memo Akten, www.memo.tv
*** The Mega Super Awesome Visuals Company ***
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of MSA Visuals nor the names of its contributors
*       may be used to endorse or promote products derived from this software
*       without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* ***********************************************************************/
//      April 2009 optimized and extended by Maa (http://www.lagraine.com/ - new content coming soon)



#pragma once

#if !AAASEED()
#	include "ofmain.h"
#else
#	ifndef OF_MAIN
#		include "aaa_msa_ofmain.h"
#	endif
#	ifndef AAA_IMG_H
#		include "image/img.h"
#	endif
#endif

#include "ofxMSAFluidSolver.h"

#define FLUID_DRAW_COLOR	0
#define FLUID_DRAW_MOTION	1
#define FLUID_DRAW_SPEED	2
#define FLUID_DRAW_VECTORS	3

class ofxMSAFluidDrawer : public ofBaseDraws {
private:
	virtual void draw_low(float x, float y, float renderWidth, float renderHeight);
public:
	float alpha;

	ofxMSAFluidDrawer();
	virtual ~ofxMSAFluidDrawer();

	ofxMSAFluidSolver* setup(int NX = FLUID_DEFAULT_NX, int NY = FLUID_DEFAULT_NY);
	ofxMSAFluidSolver* setup(ofxMSAFluidSolver* f);
	ofxMSAFluidSolver* setSize(int NX, int NY);
	ofxMSAFluidSolver* getFluidSolver();

	void update();

	void drawColor(float x, float y, float renderWidth, float renderHeight, bool withAlpha = false, bool alphaFlip = false );
	void drawMotion(float x, float y, float renderWidth, float renderHeight, bool withAlpha = false, bool alphaFlip = false );
	void drawSpeed(float x, float y, float renderWidth, float renderHeight, bool withAlpha = false, bool alphaFlip = false );
	void drawVectors(float x, float y, float renderWidth, float renderHeight);

	//	these ones do choose one of the previous based on _drawMode set by setDrawMode()
	virtual void draw(float x = 0, float y = 0);
	virtual void draw(float x, float y, float renderWidth, float renderHeight);

	void setDrawMode(int newDrawMode);

	void reset();

#if !AAASEED()
	FINLINE	float	getWidth()					{	return tex.getWidth(); }
	FINLINE	float	get_sy()					{	return tex.get_sy();	}
#else
	FINLINE	float	getWidth()					{	return float(_img->get_size_x()); }
	FINLINE	float	getHeight()					{	return float(_img->get_size_y()); }
			void	set_dst_img(c_img_2d* dst);
#endif


protected:
	UINT8*				_pixels;		// pixels array to be drawn
	int					_byteCount;		// number of byes in the pixel array (size * 3)
	int					_drawMode;

#if !AAASEED()
	ofTexture			tex;
#else
	c_img_2d*			_img;
#endif

	ofxMSAFluidSolver*	_fluidSolver;
	bool				_didICreateTheFluid;

	virtual void	createTexture();

			void	deleteFluidSolver();
			bool	isFluidReady();
};

