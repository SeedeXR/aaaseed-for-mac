/***********************************************************************
 This class is for drawing a fluidsolver using the AAASeed texture
*/

#pragma once

#include "MSAFluidDrawerBase.h"
#ifndef AAA_IMG_H
#	include "image/img.h"
#endif

namespace MSA {
	
	class FluidDrawerGl : public MSA::FluidDrawerBase {
	public:	
		FINLINE	float	getWidth()	{ return float( _tex->get_size_x() ); }
		FINLINE	float	getHeight()	{ return float( _tex->get_size_y() ); }
		
		FluidDrawerGl();

//		ofTexture&	getTextureReference() {
//			return tex;
//		}
		void	set_dst_img( c_img_2d* dst )
		{
			_tex = dst;
		}
		FluidSolver* setSize(int NX, int NY)
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
	protected:	
		c_img_2d*				_tex;

		void createTexture();

		
		void updateTexture()
		{
//			tex.loadData(_pixels, (int)tex.getWidth(), (int)tex.getHeight(), _glType);
		}
		
		void deleteTexture();	//			tex.clear();
		
		void drawTexture(float x, float y, float w, float h)
		{
//			tex.draw(x, y, w, h);
		}		
	public:
		void draw_low(float x, float y, float renderWidth, float renderHeight);
		void drawColor(float x, float y, float renderWidth, float renderHeight, bool withAlpha = false, bool alphaFlip = false );
		void drawMotion(float x, float y, float renderWidth, float renderHeight, bool withAlpha = false, bool alphaFlip = false );
		void drawSpeed(float x, float y, float renderWidth, float renderHeight, bool withAlpha = false, bool alphaFlip = false );
	};
	
}
