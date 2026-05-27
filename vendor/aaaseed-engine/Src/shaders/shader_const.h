
#ifdef AAA_SHADER_CONST_H
#error "SHADER_CONST_H included more than once."
#endif
#define AAA_SHADER_CONST_H 1

#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

namespace n_shader {

	CONSTEXPR	INT32 VERTEX_FLOAT_NB	= 8;
	CONSTEXPR	INT32 VERTEX_INT_NB		= 4;
	CONSTEXPR	INT32 VERTEX_VEC4_NB	= 12;

	CONSTEXPR	INT32 GEOMETRY_FLOAT_NB	= 4;
	CONSTEXPR	INT32 GEOMETRY_INT_NB	= 4;
	CONSTEXPR	INT32 GEOMETRY_VEC4_NB	= 4;

	CONSTEXPR	INT32 FRAGMENT_FLOAT_NB	= 24;
	CONSTEXPR	INT32 FRAGMENT_INT_NB	= 4;
	CONSTEXPR	INT32 FRAGMENT_VEC4_NB	= 8;

	CONSTEXPR	INT32 COMPUTE_FLOAT_NB	= 24;
	CONSTEXPR	INT32 COMPUTE_INT_NB	= 4;
	CONSTEXPR	INT32 COMPUTE_VEC4_NB	= 8;

	//2022 July this struct is used to regroup the uniform for c_shading
	//todo perhaps we should also split render and compute here too
	//todo we decided not to pass it for now to the shader (we did: it was slower because we can pass less)
	typedef struct st_uni
	{
		FP32		_vs_float	[	VERTEX_FLOAT_NB		];
		INT32		_vs_int		[	VERTEX_INT_NB		];
		FP32		_vs_vec4	[	VERTEX_VEC4_NB		][ 4 ];		

		FP32		_gs_float	[	GEOMETRY_FLOAT_NB	];
		INT32		_gs_int		[	GEOMETRY_INT_NB		];
		FP32		_gs_vec4	[	GEOMETRY_VEC4_NB	][ 4 ];
	
		FP32		_fs_float	[	FRAGMENT_FLOAT_NB	];
		INT32		_fs_int		[	FRAGMENT_INT_NB		];
		FP32		_fs_vec4	[	FRAGMENT_VEC4_NB	][ 4 ];

		FP32		_cs_float	[	COMPUTE_FLOAT_NB	];
		INT32		_cs_int		[	COMPUTE_INT_NB		];
		FP32		_cs_vec4	[	COMPUTE_VEC4_NB		][ 4 ];		

		FP32		_fs_external_mouse	[ 3 ];
		FP32		_fs_external_time;
		FP32		_fs_src;
		FP32		_fs_out;
	} st_uni;
};

