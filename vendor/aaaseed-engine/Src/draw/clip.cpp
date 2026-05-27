#include "clip.h"
#include "infrastructure/param/param_declare.h"
#include "ui/strsymbo.h"
#include "gol/gol.h"


FACTORY_CREATE_V1( c_clip, clip, Clip, clip );

c_clip*	c_clip::cur = nullptr;

namespace n_clip
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 6;
	CONSTEXPR INT32 GROUP_NB			= 0;
	CONSTEXPR INT32 PARAM_NB_MAX		=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_SYMBO_PSTR_ZERO(	active, gstr::borrow )
		PARAM_DEF_INT32(			plane_index,	1, 0,	0, 5 )

		PARAM_DEF_DOUBLE_INF(		equation_x,		1, 0	)
		PARAM_DEF_DOUBLE_INF(		equation_y,		1, 0	)
		PARAM_DEF_DOUBLE_INF(		equation_z,		1, 0	)
		PARAM_DEF_DOUBLE_INF(		equation_w,		1, 0	)
	};
}

void	c_clip::param_init_pt_static()
{
	INT32	h = 0;

	param_set_pt( h, _s_clip_active );
	param_set_pt( h, _plane_index );

	param_set_pt_4( h, _equation );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_clip)
{
	param_init_with( n_clip::param, n_clip::PARAM_NB_MAX);
}

c_clip::~c_clip()
{
	if( cur == this )
		cur = nullptr;
}

//todo deal with other planes
void	c_clip::disable()
{
	GOL::disable( GL_CLIP_PLANE0 );
}

void	c_clip::update()
{
	if( _s_clip_active > 0 ) // Not current
	{
		GLenum plane_enum = GL_CLIP_PLANE0 + _plane_index;
		if( _s_clip_active == 2 )	// Owner
		{
			GOL::enable(			plane_enum );
			GOL::set_clip_plane(	plane_enum, _equation );
		}
		else	// none
			GOL::disable(	plane_enum );
	}
	c_clip::cur = this;
}