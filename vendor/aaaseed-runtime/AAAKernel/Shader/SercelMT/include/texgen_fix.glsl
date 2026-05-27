//
//	TEXTURE COOR
//

// --------------------------------------------------------------------------
// TEXTURE COORD FUNCTION
//	generate texture coordonate

void ftexgen_for_texcoor( const in vec3 normal, const in vec4 ecPosition )
{
	for( int i=0; i<TEX_UNIT_NB; ++i )
	{
		if( aaa_tex_dim[i] > 0 )	//	dim is negative when unit unused, dim 0 don't need mapping
		{
			int	tex_gen	= aaa_tex_gen[i];
			// Compute texture coordinate
			TEX_COOR_VEC tex_coor;
			if( tex_gen == 0 )	//	implicit
			{
				#if (TEX_COOR_DIM == 4)
					tex_coor = gl_TextureMatrix[i] * gl_MultiTexCoord0;	//	gl_TexCoord[i] would be better but dont work for now
				#else
					tex_coor = (gl_TextureMatrix[i] * gl_MultiTexCoord0 ).st;	//	gl_TexCoord[i] would be better but dont work for now
				#endif
/*
				if( i==1 )
				{
					tex_coor = vec2(0,0);
				}
*/
			}
			else if( tex_gen == 1 )	//	Object
			{
				tex_coor.s = dot( gl_Vertex, gl_ObjectPlaneS[i] );
				tex_coor.t = dot( gl_Vertex, gl_ObjectPlaneT[i] );
				#if (TEX_COOR_DIM == 4)
					//	ok but unused for now
					tex_coor.p = dot( gl_Vertex, gl_ObjectPlaneR[i] );
					tex_coor.q = dot( gl_Vertex, gl_ObjectPlaneQ[i] );
				#endif
			}
			else if( tex_gen == 2 )	//	Eye
			{
				tex_coor.s = dot( ecPosition, gl_EyePlaneS[i] );
				tex_coor.t = dot( ecPosition, gl_EyePlaneT[i] );
				#if (TEX_COOR_DIM == 4)
					//	ok but unused for now
					tex_coor.p = dot( ecPosition, gl_EyePlaneR[i] );
					tex_coor.q = dot( ecPosition, gl_EyePlaneQ[i] );
				#endif
			}
			else if( tex_gen == 3 )	//	Sphere
			{
				vec3 ecPosition3;
				ecPosition3 = (vec3(ecPosition)) / ecPosition.w;
				vec2 sphereMap;
				sphereMap = SphereMap( ecPosition3, normal );
				#if (TEX_COOR_DIM == 4)
					tex_coor = vec4( sphereMap, 0.0, 1.0 );
				#else
					tex_coor = sphereMap;
				#endif
			}
			else if( tex_gen == 4 )	//	Reflection ?
			{
				vec3 ecPosition3;
				ecPosition3 = (vec3(ecPosition)) / ecPosition.w;
				vec3 reflection;
					reflection = ReflectionMap( ecPosition3, normal );
				#if (TEX_COOR_DIM == 4)
					tex_coor = vec4( reflection, 1.0 );
				#else
					tex_coor = reflection.xy;
				#endif
			}
			else if( tex_gen == 5 )	//	Normal ?
			{
				#if (TEX_COOR_DIM == 4)
					tex_coor = vec4( normal, 1.0 );
				#else
					tex_coor = normal.xy;
				#endif
			}
			else	//			PROJ_CAMERA and PROJ_CAMERA_MODELVIEW
					//			check it
			{
				tex_coor.s = dot( gl_Vertex, gl_ObjectPlaneS[i] );
				tex_coor.t = dot( gl_Vertex, gl_ObjectPlaneT[i] );
				#if (TEX_COOR_DIM == 4)
					//	ok but unused for now
					tex_coor.p = dot( gl_Vertex, gl_ObjectPlaneR[i] );
					tex_coor.q = dot( gl_Vertex, gl_ObjectPlaneQ[i] );
				#endif
			}
			//gl_TexCoord[i] = tex_coor;
			//if ( i  == 0 )
			vs_out.tex_coor[i] = tex_coor;
		}
	}
}