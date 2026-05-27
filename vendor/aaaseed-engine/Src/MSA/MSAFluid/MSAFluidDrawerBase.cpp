/***********************************************************************
 
 This class is for drawing a fluidsolver using the openFrameworks texture
 
 /***********************************************************************
 
 Copyright (c) 2008, 2009, 2010, Memo Akten, www.memo.tv
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

#include "MSAFluidDrawerBase.h"

static FINLINE	float		MIN( float a, float b )				{	return	(a < b) ? a : b;	}

#include <algorithm>
#if AAASEED()
#	include "err.h"
#	include	"gol/gol_color.h"
#	include	"gol/gol_draw.h"
#	include	"gol/gol_matrix.h"
#endif

namespace MSA {
	
	const string FluidDrawerBase::drawOptionTitles[] = {
		"kFluidDrawColor",
		"kFluidDrawMotion",
		"kFluidDrawSpeed",
		"kFluidDrawVectors"
	};
		
	
	FluidDrawerBase::FluidDrawerBase() {
		_pixels				= nullptr;
		_fluidSolver		= nullptr;
		_didICreateTheFluid	= false;
		
		enabled				= true;
		useAdditiveBlending = false;
		brightness			= 1;
		doInvert			= false;
		velDrawMult				= 1;
		vectorSkipCount		= 0;
		
		enableAlpha(false);
		
		setDrawMode(kFluidDrawColor);
	}
	
	FluidDrawerBase::~FluidDrawerBase() {
		deleteFluidSolver();
	}
	
	
	
	FluidSolver* FluidDrawerBase::setup(int NX, int NY) {
		deleteFluidSolver();
		_fluidSolver = new FluidSolver;
		_fluidSolver->setup(NX, NY);
		allocatePixels();
		createTexture();
		
		return _fluidSolver;
	}
	
	FluidSolver* FluidDrawerBase::setup(FluidSolver* f) {
		deleteFluidSolver();
		_fluidSolver = f;
		allocatePixels();
		createTexture();
		
		return _fluidSolver;
	}
	
	FluidSolver* FluidDrawerBase::getFluidSolver() {
		return _fluidSolver;
	}
	
	void FluidDrawerBase::enableAlpha(bool b) {
		_alphaEnabled = b;
		if(_alphaEnabled) {
			_glType = GL_RGBA;
			_bpp = 4;
		} else {
			_glType = GL_RGB;
			_bpp = 3;
		}
		
		if(isFluidReady()) {
			allocatePixels();
			createTexture();
		}
	}
	
	
	void FluidDrawerBase::allocatePixels() {
		if(_pixels) delete []_pixels;
		int texWidth = _fluidSolver->getWidth()-2;
		int texHeight =_fluidSolver->getHeight()-2;
		_pixels = new unsigned char[texWidth * texHeight * _bpp];
	}
	
	
	void FluidDrawerBase::reset() {
		if(!isFluidReady()) {
			DBG_PRINT_STRING("FluidDrawerBase::reset() - Fluid not ready" );
			return;
		}
		_fluidSolver->reset();
	}
	
	void FluidDrawerBase::update() {
		if(!isFluidReady()) {
			DBG_PRINT_STRING( "FluidDrawerBase::updateFluid() - Fluid not ready" );
			return;
		}
		_fluidSolver->update();
	}
	
	
	void FluidDrawerBase::setDrawMode(FluidDrawMode newDrawMode) {
		drawMode = newDrawMode;
		if(drawMode < 0) drawMode = (FluidDrawMode)(kFluidDrawCount-1);
		else if(drawMode >= kFluidDrawCount) drawMode = (FluidDrawMode)0;
	}
	
	void FluidDrawerBase::incDrawMode() {
		setDrawMode((FluidDrawMode)((int)drawMode + 1));
	}
	
	void FluidDrawerBase::decDrawMode() {
		setDrawMode((FluidDrawMode)(drawMode - 1));
	}
	
	FluidDrawMode FluidDrawerBase::getDrawMode() {
		return drawMode;
	}
	
	string FluidDrawerBase::getDrawModeName() {
		return drawOptionTitles[drawMode];
	}
	
	
	
	void FluidDrawerBase::draw(float x, float y) {
		if(enabled == false) return;
		
		draw( x,y, float(getWindowWidth()), float(getWindowHeight()) );
	}
	
	
	void FluidDrawerBase::draw(float x, float y, float renderWidth, float renderHeight) {
		if(enabled == false) return;
		
		switch(drawMode) {
			case kFluidDrawColor:
				drawColor(x, y, renderWidth, renderHeight);
				break;
				
			case kFluidDrawMotion:
				drawMotion(x, y, renderWidth, renderHeight);
				break;
				
			case kFluidDrawSpeed:
				drawSpeed(x, y, renderWidth, renderHeight);
				break;
				
			case kFluidDrawVectors:
				drawVectors(x, y, renderWidth, renderHeight);
				break;
				
		}
	}
	
	
	void FluidDrawerBase::drawColor(float x, float y, float renderWidth, float renderHeight, bool withAlpha) {
		if(enabled == false) return;
		
		if(useAdditiveBlending) {
			GOL::set_blend_func(GL_ONE, GL_ONE);
			GOL::enable_blend();
		} else {
			GOL::disable_blend();
		}
		
		int fw = _fluidSolver->getWidth();
		int fh = _fluidSolver->getHeight();
		
		Vec2f vel;
		Color color;
		int index = 0;
		for(int j=1; j < fh-1; ++j ) {
			for(int i=1; i < fw-1; ++i ) {
				_fluidSolver->getInfoAtCell(i, j, &vel, &color);
				int r = (unsigned char) MIN( color.r * 255 * brightness, 255.0f );
				int g = (unsigned char) MIN( color.g * 255 * brightness, 255.0f );
				int b = (unsigned char) MIN( color.b * 255 * brightness, 255.0f );
				if(doInvert) {
					r = 255 - r;
					g = 255 - g;
					b = 255 - b;
				}
				_pixels[index++] = r;
				_pixels[index++] = g;
				_pixels[index++] = b;
				
				if(_alphaEnabled) _pixels[index++] = withAlpha ? max(b, max(r, g)) : 255;
			}
		}
		
		updateTexture();
		drawTexture(x, y, renderWidth, renderHeight);
	}
	
	
	
	void FluidDrawerBase::drawMotion(float x, float y, float renderWidth, float renderHeight, bool withAlpha)
	{
		if(enabled == false)
			return;
		
		if(useAdditiveBlending) {
			GOL::set_blend_func(GL_ONE, GL_ONE);
			GOL::enable_blend();
		} else {
			GOL::disable_blend();
		}
		
		int fw = _fluidSolver->getWidth();
		int fh = _fluidSolver->getHeight();
		
		Vec2f vel;
		int index = 0;
		for(int j=1; j < fh-1; ++j ) {
			for(int i=1; i < fw-1; ++i ) {
				_fluidSolver->getInfoAtCell( i, j, &vel, nullptr );
				float speed2 = fabs(vel.x) * fw + fabs(vel.y) * fh;
				int speed = (int)MIN(speed2 * 255 * brightness, 255.0f);
				_pixels[index++] = (unsigned char)MIN(fabs(vel.x) * fw * 255 * brightness, 255.0f);
				_pixels[index++] = (unsigned char)MIN(fabs(vel.y) * fh * 255 * brightness, 255.0f);
				_pixels[index++] = (unsigned char)0;
				
				if(_alphaEnabled) _pixels[index++] = withAlpha ? speed : 255;
				
			}
		}
		
		updateTexture();
		drawTexture(x, y, renderWidth, renderHeight);
	}
	
	
	void FluidDrawerBase::drawSpeed(float x, float y, float renderWidth, float renderHeight, bool withAlpha) {
		if(enabled == false)
			return;
		
		if(useAdditiveBlending) {
			GOL::set_blend_func(GL_ONE, GL_ONE);
			GOL::enable_blend();
		} else {
			GOL::disable_blend();
		}
		
		int fw = _fluidSolver->getWidth();
		int fh = _fluidSolver->getHeight();
		
		Vec2f vel;
		int index = 0;
		for(int j=1; j < fh-1; ++j ) {
			for(int i=1; i < fw-1; ++i ) {
				_fluidSolver->getInfoAtCell( i, j, &vel, nullptr );
				float speed2 = fabs(vel.x) * fw + fabs(vel.y) * fh;
				int speed = (int)MIN(speed2 * 255 * brightness, 255.0f);
				_pixels[index++] = (unsigned char)speed;
				_pixels[index++] = (unsigned char)speed;
				_pixels[index++] = (unsigned char)speed;
				
				if(_alphaEnabled) _pixels[index++] = withAlpha ? speed : 255;
			}
		}
		
		updateTexture();
		drawTexture(x, y, renderWidth, renderHeight);
	}
	
	
	void FluidDrawerBase::drawVectors(float x, float y, float renderWidth, float renderHeight) {
		if(enabled == false)
			return;
		
		int fw = _fluidSolver->getWidth();
		int fh = _fluidSolver->getHeight();
		
		if(useAdditiveBlending) {
			GOL::set_blend_func(GL_ONE, GL_ONE);
			GOL::enable_blend();
		} else {
			GOL::disable_blend();
		}
		
		//	int xStep = renderWidth / 10;		// every 10 pixels
		//	int yStep = renderHeight / 10;		// every 10 pixels
		
		GOL::matrix::push();
		GOL::matrix::translate(x, y, 0);
		GOL::matrix::scale( renderWidth/(fw-2), renderHeight/(fh-2), 1.0 );
		
		float maxVel = 5.0f/20000;
		
		Vec2f vel;
		float vt = velDrawThreshold * _fluidSolver->getInvWidth() * _fluidSolver->getInvHeight();
		vt *= vt;
		
		for (int j=0; j<fh-2; j+=vectorSkipCount+1 ){
			for (int i=0; i<fw-2; i+=vectorSkipCount+1 ){
				vel = _fluidSolver->getVelocityAtCell(i+1, j+1);
				float d2 = vel.lengthSquared();
				if(d2>vt) {
					if(d2 > maxVel * maxVel) {
						float mult = maxVel * maxVel/ d2;
						//				float mult = (d2 - maxVel * maxVel) / d2;
						vel.x *= mult;
						vel.y *= mult;
					}
					vel *= velDrawMult * 50000;
					
#ifndef MSA_TARGET_OPENGLES
					float b = mapRange(d2, vt, maxVel, 0.0f, brightness);
					b = brightness;
					GOL::color3( b,b,b );
					
					GOL::begin(GL_LINES);
						GOL::vertex2f( FP32(i), FP32(j) );
						GOL::vertex2f( i + vel.x, j + vel.y);
					GOL::end();
#endif
				}
			}
		}
		GOL::matrix::pop();
		
	}
	
	
	
	void FluidDrawerBase::deleteFluidSolver() {
		DBG_PRINT_STRING( "FluidDrawerBase::deleteFluidSolver()" );
		if(_fluidSolver && _didICreateTheFluid) {
			delete _fluidSolver;
			_fluidSolver = nullptr;
			
			if(_pixels) delete []_pixels;
			_pixels = nullptr;
			
			deleteTexture();
		}
	}
	
	bool FluidDrawerBase::isFluidReady() {
		if(!_fluidSolver) {
			DBG_PRINT_STRING( "FluidDrawerBase::isFluidReady() - No fluid solver" );
			return false;
		}
		
		if(!_fluidSolver->isInited()) {
			DBG_PRINT_STRING( "FluidDrawerBase::isFluidReady() - Fluid solver not initialized yet, call init()" );
			return false;
		}
		
		return true;
	}
}