#include "MSAFluidDrawerGl-AAASeed.h"

c_rand_lin MSA::Rand::msa_rand;

MSA::FluidDrawerGl::FluidDrawerGl()
:_tex(nullptr)
{
}

void MSA::FluidDrawerGl::createTexture()
{
	int texWidth = _fluidSolver->getWidth()-2;
	int texHeight =_fluidSolver->getHeight()-2;

	deleteTexture();
#if AAASEED()
	_pixels = (UINT8*) MALLOC_ALIGNED( texWidth * texHeight * 4, 0 );
#else
	tex.allocate(texWidth, texHeight, GL_RGBA );
#endif
}

void MSA::FluidDrawerGl::deleteTexture()
{
#if AAASEED()
	FREE_ALIGNED_AND_NULL( _pixels );
#else
	tex.clear();
#endif
}
void MSA::FluidDrawerGl::draw_low(float x, float y, float renderWidth, float renderHeight)
{
#if !AAASEED()
	tex.loadData( _pixels, tex.getWidth(), tex.get_sy(), GL_RGBA );
	tex.draw( x, y, renderWidth, renderHeight );
#else
	_tex->init_from_mem( _tex->get_size_x(), _tex->get_size_y(), aaa::PIXEL_FORMAT::RGBA_8, _pixels );
#endif
}

//maa i have to do this or compile complain
REAL alpha = 1.;
#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

#if 0
void MSA::FluidDrawerGl::drawColor(float x, float y, float renderWidth, float renderHeight, bool withAlpha, bool alphaFlip)
{
	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->get_sy();

	//Vec2f vel;
	Color color;
	int index = 0;
	for(int j=1; j < fh-1; ++j )
	{
		for(int i=1; i < fw-1; ++i )
		{
			color = _fluidSolver->getColorAtCell(i, j);
			//float speed2 = fabs(vel.x) * fw + fabs(vel.y) * fh;
			//int speed = MIN(speed2 * 255 * alpha, 255);
			int r = _pixels[index++] = MIN( color.r * 255 * alpha, 255 );
			int g = _pixels[index++] = MIN( color.g * 255 * alpha, 255 );
			int b = _pixels[index++] = MIN( color.b * 255 * alpha, 255 );
			if( withAlpha )
			{
				int col_max = aaa::MAX( b, r, g );
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
#else
void MSA::FluidDrawerGl::drawColor(float x, float y, float renderWidth, float renderHeight, bool withAlpha, bool alphaFlip)
{
	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->getHeight();

	//Vec2f vel;
	Vec3f* color;
	int index = 0;
	unsigned char*	pixel = _pixels-1;
	if( withAlpha )
	{
		for( int j=1; j < fh-1; ++j )
		{
			color = _fluidSolver->getColorPtAtCell(1, j);
			for(int i=1; i < fw-1; ++i )
			{
				//float speed2 = fabs(vel.x) * fw + fabs(vel.y) * fh;
				//int speed = MIN(speed2 * 255 * alpha, 255);
				int r = *++pixel = MIN( UINT8(color->x * 255 * alpha), 255 );
				int g = *++pixel = MIN(	UINT8(color->y * 255 * alpha), 255 );
				int b = *++pixel = MIN( UINT8(color->z * 255 * alpha), 255 );
				++color;
				int col_max = aaa::MAX( b, r, g );
				if( alphaFlip )
					col_max = 255 - col_max;
				*++pixel =  col_max;
			}
		} 
	}
	else
	{
		for( int j=1; j < fh-1; ++j )
		{
			color = _fluidSolver->getColorPtAtCell(1, j);
			for(int i=1; i < fw-1; ++i )
			{
				//float speed2 = fabs(vel.x) * fw + fabs(vel.y) * fh;
				//int speed = MIN(speed2 * 255 * alpha, 255);
				*++pixel = MIN( UINT8(color->x * 255 * alpha), 255);
				*++pixel = MIN( UINT8(color->y * 255 * alpha), 255);
				*++pixel = MIN( UINT8(color->z * 255 * alpha), 255);
				++color;
				*++pixel =  255;
			}
		}
	}
	draw_low( x, y, renderWidth, renderHeight );
}
#endif

void MSA::FluidDrawerGl::drawMotion(float x, float y, float renderWidth, float renderHeight, bool withAlpha, bool alphaFlip)
{
	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->getHeight();

	Vec2f vel;
	int index = 0;
	for(int j=1; j < fh-1; ++j )
	{
		for(int i=1; i < fw-1; ++i )
		{
			_fluidSolver->getInfoAtCell( i, j, &vel, nullptr );
			float speed2 = fabs(vel.x) * fw + fabs(vel.y) * fh;
			 UINT8 speed = MIN( UINT8(speed2 * 255 * alpha), UINT8(255) );
			_pixels[index++] = MIN( UINT8(fabs(vel.x) * fw * 255 * alpha), UINT8(255) );
			_pixels[index++] = MIN( UINT8(fabs(vel.y) * fh * 255 * alpha), UINT8(255) );
			_pixels[index++] = 0;
			if( withAlpha )
				_pixels[index++] =  alphaFlip ? UINT8(255)-speed : speed;
			else
				_pixels[index++] =  255;

		}
	}  
	draw_low( x, y, renderWidth, renderHeight );
}


void MSA::FluidDrawerGl::drawSpeed(float x, float y, float renderWidth, float renderHeight, bool withAlpha, bool alphaFlip)
{
	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->getHeight();

	Vec2f vel;
	int index = 0;
	for(int j=1; j < fh-1; ++j ) {
		for(int i=1; i < fw-1; ++i ) {
			_fluidSolver->getInfoAtCell( i, j, &vel, nullptr );
			float speed2 = fabs(vel.x) * fw + fabs(vel.y) * fh;
			UINT8 speed = MIN( UINT8(speed2 * 255 * alpha), UINT8(255) );
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
