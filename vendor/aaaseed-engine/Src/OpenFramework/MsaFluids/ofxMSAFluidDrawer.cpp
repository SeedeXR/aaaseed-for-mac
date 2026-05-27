/***********************************************************************

This class is for drawing a fluidsolver using the openFrameworks texture

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


#if AAASEED()
#	include "gol/gol.h"
#	include "gol/gol_draw.h"
#	include "gol/gol_color.h"
#	include "gol/gol_matrix.h"
#endif

#include "ofxMSAFluidDrawer.h"


ofxMSAFluidDrawer::ofxMSAFluidDrawer()
#if AAASEED()
	:_img(nullptr)
#endif
{
	DBG_PRINT_STRING("ofxMSAFluidDrawer::ofxMSAFluidDrawer()");
	_pixels					= nullptr;
	_byteCount				= 0;
	_fluidSolver			= nullptr;
	_didICreateTheFluid		= false;
	alpha					= 1;
	setDrawMode(FLUID_DRAW_COLOR);
}

ofxMSAFluidDrawer::~ofxMSAFluidDrawer()
{
	DBG_PRINT_STRING("ofxMSAFluidDrawer::~ofxMSAFluidDrawer()");
	deleteFluidSolver();
	FREE_ALIGNED_AND_NULL( _pixels );
}



ofxMSAFluidSolver* ofxMSAFluidDrawer::setup(int NX, int NY)
{
	deleteFluidSolver();
	_fluidSolver = new ofxMSAFluidSolver;
	_fluidSolver->setup(NX, NY);
	createTexture();

	return _fluidSolver;
}

ofxMSAFluidSolver* ofxMSAFluidDrawer::setSize(int NX, int NY)
{
	if( _fluidSolver )
	{
		//maa this +2 -2 will be dangerous at some point perhaps Width and Height should exclude the borders
		if( NX != _fluidSolver->getWidth()-2 || NY != _fluidSolver->getHeight()-2 )
		{
			_fluidSolver->setSize(NX, NY);
			createTexture();
		}
	}
	return _fluidSolver;
}


ofxMSAFluidSolver* ofxMSAFluidDrawer::setup(ofxMSAFluidSolver* f) {
	deleteFluidSolver();
	_fluidSolver = f;
	createTexture();

	return _fluidSolver;
}

ofxMSAFluidSolver* ofxMSAFluidDrawer::getFluidSolver() {
	return _fluidSolver;
}

void ofxMSAFluidDrawer::createTexture()
{
	int texWidth = _fluidSolver->getWidth()-2;
	int texHeight =_fluidSolver->getHeight()-2;

#if AAASEED()
	FREE_ALIGNED_AND_NULL( _pixels );
	_pixels = (UINT8*) MALLOC_ALIGNED( texWidth * texHeight * 4, 0 );
#else
	tex.allocate(texWidth, texHeight, GL_RGBA);
#endif
}

#if AAASEED()
void	ofxMSAFluidDrawer::set_dst_img( c_img_2d* dst )
{
	_img = dst;
}
#endif

void ofxMSAFluidDrawer::reset() {
	if(!isFluidReady()) {
	DBG_PRINT_STRING( "ofxMSAFluidDrawer::reset() - Fluid not ready" );
		return;
	}
	_fluidSolver->reset();
}

void ofxMSAFluidDrawer::update() {
	if(!isFluidReady()) {
		DBG_PRINT_STRING("ofxMSAFluidDrawer::updateFluid() - Fluid not ready" );
		return;
	}
	_fluidSolver->update();
}


void ofxMSAFluidDrawer::setDrawMode(int newDrawMode) {
	_drawMode = newDrawMode;
}

void ofxMSAFluidDrawer::draw(float x, float y)
{
#if !AAASEED()
	draw(x, y, ofGetWidth(), ofGetHeight());
#else
	draw(x, y, 1024, 1024);
#endif
}


void ofxMSAFluidDrawer::draw(float x, float y, float renderWidth, float renderHeight) {
	switch(_drawMode) {
				case FLUID_DRAW_COLOR:
					drawColor(x, y, renderWidth, renderHeight);
					break;

				case FLUID_DRAW_MOTION:
					drawMotion(x, y, renderWidth, renderHeight);
					break;

				case FLUID_DRAW_SPEED:
					drawSpeed(x, y, renderWidth, renderHeight);
					break;

				case FLUID_DRAW_VECTORS:
					drawVectors(x, y, renderWidth, renderHeight);
					break;

	}
}

void ofxMSAFluidDrawer::draw_low(float x, float y, float renderWidth, float renderHeight)
{
#if !AAASEED()
	tex.loadData( _pixels, tex.getWidth(), tex.get_sy(), GL_RGBA );
	tex.draw( x, y, renderWidth, renderHeight );
#else
	_img->init_from_mem( _img->get_size_x(), _img->get_size_y(), aaa::PIXEL_FORMAT::RGBA_8, _pixels );
#endif
}

void ofxMSAFluidDrawer::drawColor(float x, float y, float renderWidth, float renderHeight, bool withAlpha, bool alphaFlip)
{
	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->getHeight();

	ofPoint color;
	int index = 0;
	for(int j=1; j < fh-1; ++j )
	{
		for(int i=1; i < fw-1; ++i )
		{
			_fluidSolver->getInfoAtCell(i, j, nullptr, &color);
			int r = _pixels[index++] = UINT8( MIN(color.x * 255 * alpha, 255) );
			int g = _pixels[index++] = UINT8( MIN(color.y * 255 * alpha, 255) );
			int b = _pixels[index++] = UINT8( MIN(color.z * 255 * alpha, 255) );
			if( withAlpha )
			{
				int col_max = MAX(b, MAX(r, g));
				if( alphaFlip )
					col_max = 255 - col_max;
				_pixels[index++] =  col_max;
			}
			else
				_pixels[index++] =  255;
		}
	}  
	draw_low( x, y, renderWidth, renderHeight );
}



void ofxMSAFluidDrawer::drawMotion(float x, float y, float renderWidth, float renderHeight, bool withAlpha, bool alphaFlip)
{
	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->getHeight();

	ofPoint vel;
	int index = 0;
	for(int j=1; j < fh-1; ++j )
	{
		for(int i=1; i < fw-1; ++i )
		{
			_fluidSolver->getInfoAtCell( i, j, &vel, nullptr );
			float speed2 = fabs(vel.x) * fw + fabs(vel.y) * fh;
			int speed = MIN( int(speed2 * 255 * alpha), 255 );
			_pixels[index++] = MIN( UINT8(fabs(vel.x) * fw * 255 * alpha), 255 );
			_pixels[index++] = MIN( UINT8(fabs(vel.y) * fh * 255 * alpha), 255 );
			_pixels[index++] = 0;
			if( withAlpha )
				_pixels[index++] =  alphaFlip ? 255-speed : speed;
			else
				_pixels[index++] =  255;

		}
	}  
	draw_low( x, y, renderWidth, renderHeight );
}


void ofxMSAFluidDrawer::drawSpeed(float x, float y, float renderWidth, float renderHeight, bool withAlpha, bool alphaFlip)
{
	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->getHeight();

	ofPoint vel;
	int index = 0;
	for(int j=1; j < fh-1; ++j ) {
		for(int i=1; i < fw-1; ++i ) {
			_fluidSolver->getInfoAtCell( i, j, &vel, nullptr );
			float speed2 = fabs(vel.x) * fw + fabs(vel.y) * fh;
			UINT8 speed = MIN( UINT8(speed2 * 255 * alpha), 255 );
			_pixels[index++] = speed;
			_pixels[index++] = speed;
			_pixels[index++] = speed;
			if( withAlpha )
				_pixels[index++] =  alphaFlip ? 255-speed : speed;
			else
				_pixels[index++] =  255;
		}
	}  
	draw_low( x, y, renderWidth, renderHeight );
}



void ofxMSAFluidDrawer::drawVectors(float x, float y, float renderWidth, float renderHeight) {
	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->getHeight();

	//      int xStep = renderWidth / 10;           // every 10 pixels
	//      int yStep = renderHeight / 10;          // every 10 pixels

	GOL::matrix::push();
	GOL::matrix::translate( x, y, 0 );
	GOL::matrix::scale( renderWidth/(fw-2), renderHeight/(fh-2), 1.0 );

	float velMult = 50000;
	float maxVel = float(5./20000);

	ofPoint vel;
//	GOL::enable_line_smooth();
//	GOL::set_line_width( 1 );
	for( int j=0; j<fh-2; ++j )
	{
		FP32 fj = FP32(j);
		for( int i=0; i<fw-2; ++i )
		{
			FP32 fi = FP32(i);
			_fluidSolver->getInfoAtCell( i+1, j+1, &vel, nullptr );
			float d2 = vel.x * vel.x + vel.y * vel.y;
			if(d2 > maxVel * maxVel) {
				float mult = maxVel * maxVel/ d2;
				//                              float mult = (d2 - maxVel * maxVel) / d2;
				vel.x *= mult;
				vel.y *= mult;
			}
			vel *= velMult;

			//                      if(dx*dx+dy*dy > velThreshold) {
			//                              float speed2 = fabs(vel.x) * fw + fabs(vel.y) * fh;
#ifndef TARGET_OPENGLES
			GOL::begin(GL_LINES);
				GOL::color_black();
				GOL::vertex2f( fi,fj );
				GOL::color_white();
				GOL::vertex2f( fi + vel.x, fj + vel.y);
			GOL::end();
#endif
			//                      printf("%.8f, %.8f\n", vel.x, vel.y);
			//                      }
		}
	}
	GOL::matrix::pop();
}



void ofxMSAFluidDrawer::deleteFluidSolver() {
	DBG_PRINT_STRING("ofxMSAFluidDrawer::deleteFluidSolver()");    
	if(_fluidSolver && _didICreateTheFluid)
	{
		delete _fluidSolver;
		_fluidSolver = nullptr;

		FREE_ALIGNED_AND_NULL( _pixels );

#if !AAASEED()
		tex.clear();
#else
		_img->lock();
		_img->dealloc_data();
		_img->unlock();
		
#endif
	}
}

bool ofxMSAFluidDrawer::isFluidReady() {
	if(!_fluidSolver) {
		DBG_PRINT_STRING("ofxMSAFluidDrawer::isFluidReady() - No fluid solver");
		return false;
	}

	if(!_fluidSolver->isInited()) {
		DBG_PRINT_STRING("ofxMSAFluidDrawer::isFluidReady() - Fluid solver not initialized yet, call init()");
		return false;
	}

	return true;
}

