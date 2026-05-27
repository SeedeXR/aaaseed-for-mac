-- this the descroption of the Virtual Machine we use on Apple Ios

program, err_str = gol.create_program( str_vert, str_frag [, str_geo] )
gol.use_program( program )
gol.bind_program = gol.use_program
gol.delete_program( program )

-- these fns change uniform for the cur used program
gol.set_uniform_vert_float( id, v1, [,v2 ...] )			--	id in[1,8] 	-> uniform float	aaa_vu_float[8];
gol.set_uniform_vert_int(   id, v1, [,v2 ...] )			--	id in[1,4]	-> uniform int		aaa_vu_int[4];
gol.set_uniform_vert_vec4(  id, x [,y [,z [, w] ] ] )	--	id in[1,8]	-> uniformvec4		aaa_vu_vec4[8];

gol.set_uniform_frag_float( id, v1, [,v2 ...] )			--	id in[1,8]	-> uniform float	aaa_fu_float[8];
gol.set_uniform_frag_int(   id, v1, [,v2 ...] )			--	id in[1,4]	-> uniform int		aaa_fu_int[4];
gol.set_uniform_frag_vec4(  id, x [,y [,z [, w] ] ] )	--	id in[1,8]	-> uniformvec4		aaa_fu_vec4[8];

gol.set_uniform_geom_float( id, v1, [,v2 ...] )			--	id in[1,4]	-> uniform float	aaa_gu_float[4];
gol.set_uniform_geom_int(   id, v1, [,v2 ...] )			--	id in[1,4]	-> uniform int		aaa_gu_int[4];
gol.set_uniform_geom_vec4(  id, x [,y [,z [, w] ] ] )	--	id in[1,4]	-> uniformvec4		aaa_gu_vec4[4];

--reste sampler / matrix / attrib a traiter

fbo = gol.create_fbo()
gol.attach_to_fbo( fbo, bind [,id] )	--	texture bind will have to be defined too, id from 0 to 7 (default is 0)
bool = gol.is_fbo_valid()
gol.bind_fbo( fbo )						--	0 no fbo
gol.delete_fbo( fbo )

tex = gol.create_tex2d()
gol.set_image( image )					--	image to define
gol.set_tex2d( size_x ,size_y [,type] )
gol.bind_tex2d( tex )
gol.delete_tex2d( tex )
