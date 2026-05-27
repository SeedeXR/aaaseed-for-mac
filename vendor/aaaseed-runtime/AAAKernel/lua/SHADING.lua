--
--	SHADING
--
--	id numbering convention:
--		On the Lua side (and on the C shading object's params), uniform ids start at 1, matching
--		Lua's array convention. So self:set_STAGE_float( 1, v ) writes the first float slot.
--		Inside the GLSL shader, the corresponding arrays (aaa_vu_float, aaa_fu_vec4, etc.) are
--		indexed from 0 as in C, so Lua id N maps to GLSL index N-1.
--
--	vec4 storage layout:
--		Each per-stage vec4 array is stored flat: vec4 id N occupies 4 consecutive slots
--		(id*4-3 .. id*4) so component accessors index directly. vec4 ids run 1..vec4_max; the
--		underlying array length is vec4_max*4.
--
if CLASS.DECLARE( "SHADING" ) then
	SHADING:set_class_status_doc(	CLASS.STATUS.CORE,
									"Encapsulate the C shading object" )

	SHADING.master = { ref = {} }
	if not aaa.b_ios then
		local ref = SHADING.master.ref
		ref.obj = aaa.obj.get_from_top_by_class( "shader_master" )
		ref.include_reload = param.get_ref( ref.obj, "include_reload_trig" )
	end

--todo get it from AAASeed
	-- AAA_VERTEX_.. on the glsl side
	SHADING.vert_nb = {		float = 8,		int = 4,	vec4 = 12	}
	SHADING.vertex_nb	= SHADING.vert_nb
	-- AAA_GEOMETRY_.. on the glsl side
	SHADING.geom_nb = {		float = 4,		int = 4,	vec4 = 4	}
	SHADING.geometry_nb	= SHADING.geom_nb
	-- AAA_FRAGMENT_.. on the glsl side
	SHADING.frag_nb = {		float = 24,		int = 4,	vec4 = 8	}
	SHADING.fragment_nb	= SHADING.frag_nb
	-- AAA_COMPUTE_.. on the glsl side
	SHADING.comp_nb = {		float = 24,		int = 4,	vec4  = 8	}
	SHADING.compute_nb	= SHADING.comp_nb
end


if not aaa.b_ios then
	function SHADING:create( name, obj_ref )
		local self= SHADING:create_instance_with_obj( name, obj_ref )
		self:set_obj( obj_ref )
		return self
	end

	local set_real_1	= param.set_real
	local set_real_2	= param.set_real_2
	local set_real_3	= param.set_real_3
	local set_real_4	= param.set_real_4
	local set_real_5	= param.set_real_5
	local set_real_6	= param.set_real_6
	local set_real_7	= param.set_real_7
	local set_real_8	= param.set_real_8

	local get_real_1	= param.get_real
	local get_real_2	= param.get_real_2
	local get_real_3	= param.get_real_3
	local get_real_4	= param.get_real_4
	local get_real_5	= param.get_real_5
	local get_real_6	= param.get_real_6
	local get_real_7	= param.get_real_7
	local get_real_8	= param.get_real_8

	function SHADING.trig_reload_include()
		local ref = SHADING.master.ref
		set_real_1( ref.include_reload, 1 )
		aaa.obj.update( ref.obj )	-- to make sure it done right away
	end

	function SHADING:trig_reload()				param.set( self.ref.reload_all, true )		end

	function SHADING:trig_reload_vert()			param.set( self.vert.reload, true )			end
	function SHADING:trig_reload_geom()			param.set( self.geom.reload, true )			end
	function SHADING:trig_reload_frag()			param.set( self.frag.reload, true )			end
	function SHADING:trig_reload_comp()			param.set( self.comp.reload, true )			end

	-- depreciated
	-- deprecated: set_<stage>_bind family is no longer recommended.
	function SHADING:set_vert_bind( val )		set_real_1( self.vert.bind, val )			end
	function SHADING:set_geom_bind( val )		set_real_1( self.geom.bind, val )			end
	function SHADING:set_frag_bind( val )		set_real_1( self.frag.bind, val )			end
	function SHADING:set_comp_bind( val )		set_real_1( self.comp.bind, val )			end

	function SHADING:edit_shader_vert()			param.do_action_open( self.ref.obj, "vertex_fname_used" )	end
	function SHADING:edit_shader_geom()			param.do_action_open( self.ref.obj, "geometry_fname_used" )	end
	function SHADING:edit_shader_frag()			param.do_action_open( self.ref.obj, "fragment_fname_used" )	end
	function SHADING:edit_shader_comp()			param.do_action_open( self.ref.obj, "compute_fname_used" )	end

	function SHADING:edit_shader( what )
		if what then
			self["edit_shader_"..what](self)
		else
			self:edit_shader_vert()
			self:edit_shader_geom()
			self:edit_shader_frag()
			self:edit_shader_comp()
		end
	end

	function SHADING:get_fname_vert()			param.get( self.vert.fname )				end
	function SHADING:get_fname_geom()			param.get( self.geom.fname )				end
	function SHADING:get_fname_frag()			param.get( self.frag.fname )				end
	function SHADING:get_fname_comp()			param.get( self.comp.fname )				end

-- REF
--
	function SHADING:__build_ref( pre, which )
		local nbs = SHADING[which.."_nb"]
		local  float_nb, int_nb, vec_nb = nbs.float, nbs.int, nbs.vec4
		pre = pre.."_"
		which = which.."_"
		local obj = self.ref.obj
		local ref = {	float={},	int={},		vec4={}		}
		ref.reload	= param.get_ref( obj, which.."reload_trig"	)
		ref.bind	= param.get_ref( obj, which.."bind"			)
		ref.fname	= param.get_ref( obj, which.."fname_used"	)
		ref.valid	= param.get_ref( obj, which.."valid"		)
		for i= 1,float_nb	do	ref.float[i]	= param.get_ref( obj, pre.."float_"..aaa.format.int_to_char2(i) )	end
		for i= 1,int_nb		do	ref.int[i]		= param.get_ref( obj, pre.."int_"..aaa.format.int_to_char2(i) )		end
		local str = pre.."vec4_0"
		local vec4 = ref.vec4
		for i= 1,vec_nb	do
			local off = (i-1) * 4 + 1
			local str =  pre..aaa.format.int_to_char2(i)
			vec4[ off	] = param.get_ref( obj, str.."_r_x" )
			vec4[ off+1	] = param.get_ref( obj, str.."_g_y" )
			vec4[ off+2	] = param.get_ref( obj, str.."_b_z" )
			vec4[ off+3	] = param.get_ref( obj, str.."_a_w" )
		end
		return ref
	end
	function SHADING:set_obj( obj )
		local ref = self.ref
		ref.obj = obj
		ref.active		= param.get_ref( obj, "active"		)
		ref.valid		= param.get_ref( obj, "valid"		)
		ref.reload_all	= param.get_ref( obj, "reload_all"	)	--todomona deal with compute (in C ?)
		local nbs = SHADING.vert_nb
		self.vert		= self:__build_ref( "vu", "vertex"		)
		self.geom		= self:__build_ref( "gu", "geometry"	)
		self.frag		= self:__build_ref( "fu", "fragment"	)
		self.frag.src	= param.get_ref( obj, "fu_src"				)
		self.frag.out	= param.get_ref( obj, "fu_out"				)
		--todo add the mouve and time fields
		self.frag.auto	= param.get_ref( obj, "fu_mouse_time_auto"	)
		self.comp		= self:__build_ref( "cu", "compute"		)
		ref.comp_active	= param.get_ref( obj, "compute_active"		)
		--todomona add fns
		return ref
	end

-- GENERIC
--
	function SHADING:is_active()							return	param.get_bool( self.ref.active )	end
	function SHADING:is_valid()
		local ref = self.ref
		local gb = param.get_bool
		return	gb(ref.valid) and (not gb(ref.comp_active) or gb(self.comp.valid) )
	end

	-- vec4 consume 4 index for each : 1-4 for the first one, 5-8 for the second ...
	SHADING.doc.get_ref = "( which, what, id ) ex get_ref( \"frag\", \"int\", 3 )"
	SHADING.doc.get_val = "( which, what, id ) ex get_ref( \"geom\", \"int\", 3 )"
	SHADING.doc.set_val = "( which, what, id, val ) ex get_ref( \"frag\", \"float\", 3, 1.5 )"
	SHADING.doc.set_save = "( which, what, id, bool ) ex get_ref( \"frag\", \"float\", 3, false )"
	function SHADING:get_ref(	sha_type, val_type, id		)	return					self[sha_type][val_type][id]			end
	function SHADING:get_val(	sha_type, val_type, id		)	return	param.get(		self[sha_type][val_type][id]	)		end
	function SHADING:set_val(	sha_type, val_type, id, val )	return	param.set(		self[sha_type][val_type][id],	val )	end
	function SHADING:set_save(	sha_type, val_type, id, b	)	return	param.set_save(	self[sha_type][val_type][id],	b )		end

-- GET_REF
--
	function SHADING:get_ref_frag_src()						return	self.frag.src					end
	function SHADING:get_ref_frag_out()						return	self.frag.out					end
	function SHADING:get_ref_frag_auto()					return	self.frag.auto					end

	function SHADING:get_ref_vert_float( id )				return	self.vert.float[id]				end
	function SHADING:get_ref_geom_float( id )				return	self.geom.float[id]				end
	function SHADING:get_ref_frag_float( id )				return	self.frag.float[id]				end
	function SHADING:get_ref_comp_float( id )				return	self.comp.float[id]				end

	function SHADING:get_ref_vert_int(	id )				return	self.vert.int[id]				end
	function SHADING:get_ref_geom_int(	id )				return	self.geom.int[id]				end
	function SHADING:get_ref_frag_int(	id )				return	self.frag.int[id]				end
	function SHADING:get_ref_comp_int(	id )				return	self.comp.int[id]				end

	function SHADING:get_ref_vert_vec4_x( id )				return	self.vert.vec4[id*4-3]			end
	function SHADING:get_ref_vert_vec4_y( id )				return	self.vert.vec4[id*4-2]			end
	function SHADING:get_ref_vert_vec4_z( id )				return	self.vert.vec4[id*4-1]			end
	function SHADING:get_ref_vert_vec4_w( id )				return	self.vert.vec4[id*4]			end
	function SHADING:get_ref_vert_vec4( id, idb )			return	self.vert.vec4[(id-1)*4+idb]	end

	function SHADING:get_ref_geom_vec4_x( id )				return	self.geom.vec4[id*4-3]			end
	function SHADING:get_ref_geom_vec4_y( id )				return	self.geom.vec4[id*4-2]			end
	function SHADING:get_ref_geom_vec4_z( id )				return	self.geom.vec4[id*4-1]			end
	function SHADING:get_ref_geom_vec4_w( id )				return	self.geom.vec4[id*4]			end
	function SHADING:get_ref_geom_vec4( id, idb )			return	self.geom.vec4[(id-1)*4+idb]	end

	function SHADING:get_ref_frag_vec4_x( id )				return	self.frag.vec4[id*4-3]			end
	function SHADING:get_ref_frag_vec4_y( id )				return	self.frag.vec4[id*4-2]			end
	function SHADING:get_ref_frag_vec4_z( id )				return	self.frag.vec4[id*4-1]			end
	function SHADING:get_ref_frag_vec4_w( id )				return	self.frag.vec4[id*4]			end
	function SHADING:get_ref_frag_vec4( id, idb )			return	self.frag.vec4[(id-1)*4+idb]	end

	function SHADING:get_ref_comp_vec4_x( id )				return	self.comp.vec4[id*4-3]			end
	function SHADING:get_ref_comp_vec4_y( id )				return	self.comp.vec4[id*4-2]			end
	function SHADING:get_ref_comp_vec4_z( id )				return	self.comp.vec4[id*4-1]			end
	function SHADING:get_ref_comp_vec4_w( id )				return	self.comp.vec4[id*4]			end
	function SHADING:get_ref_comp_vec4( id, idb )			return	self.comp.vec4[(id-1)*4+idb]	end

	-- deprecated: get_ref_<stage>_bind family is no longer recommended.
	function SHADING:get_ref_vert_bind()					return	self.vert.bind					end
	function SHADING:get_ref_geom_bind()					return	self.geom.bind					end
	function SHADING:get_ref_frag_bind()					return	self.frag.bind					end
	function SHADING:get_ref_comp_bind()					return	self.comp.bind					end

-- SET_SAVE
--
	function SHADING.__set_save( ref, i_beg, i_end, b  )
		i_beg = i_beg or 1
		b = b or false
		for i=i_beg,i_end do
			param.set_save( ref[i], b )
		end
	end

	function SHADING:set_save_vert_int	( b, i_beg, i_end )	SHADING.__set_save( self.vert.int,   i_beg, i_end or self.vert_nb.int, b ) return self end
	function SHADING:set_save_geom_int	( b, i_beg, i_end )	SHADING.__set_save( self.geom.int,   i_beg, i_end or self.geom_nb.int, b ) return self end
	function SHADING:set_save_frag_int	( b, i_beg, i_end )	SHADING.__set_save( self.frag.int,   i_beg, i_end or self.frag_nb.int, b ) return self end
	function SHADING:set_save_comp_int	( b, i_beg, i_end )	SHADING.__set_save( self.comp.int,   i_beg, i_end or self.comp_nb.int, b ) return self end

	function SHADING:set_save_vert_float( b, i_beg, i_end )	SHADING.__set_save( self.vert.float, i_beg, i_end or self.vert_nb.float, b ) return self end
	function SHADING:set_save_geom_float( b, i_beg, i_end )	SHADING.__set_save( self.geom.float, i_beg, i_end or self.geom_nb.float, b ) return self end
	function SHADING:set_save_frag_float( b, i_beg, i_end )	SHADING.__set_save( self.frag.float, i_beg, i_end or self.frag_nb.float, b ) return self end
	function SHADING:set_save_comp_float( b, i_beg, i_end )	SHADING.__set_save( self.comp.float, i_beg, i_end or self.comp_nb.float, b ) return self end

	function SHADING:set_save_vert_vec4(  b, i_beg, i_end )	SHADING.__set_save( self.vert.vec4,  (i_beg or 1)*4-3, (i_end or self.vert_nb.vec4)*4, b ) return self end
	function SHADING:set_save_geom_vec4(  b, i_beg, i_end )	SHADING.__set_save( self.geom.vec4,  (i_beg or 1)*4-3, (i_end or self.geom_nb.vec4)*4, b ) return self end
	function SHADING:set_save_frag_vec4(  b, i_beg, i_end )	SHADING.__set_save( self.frag.vec4,  (i_beg or 1)*4-3, (i_end or self.frag_nb.vec4)*4, b ) return self end
	function SHADING:set_save_comp_vec4(  b, i_beg, i_end )	SHADING.__set_save( self.comp.vec4,  (i_beg or 1)*4-3, (i_end or self.comp_nb.vec4)*4, b ) return self end

-- SET /  GET
--
	function SHADING:__set_float	(	shader_tab, id, val )	set_real_1( shader_tab.float[id],	val )	end
	function SHADING:__set_int		(	shader_tab, id, val )	set_real_1( shader_tab.int[id],		val )	end
	--todo called fns dont like nil for now ( nil became 0 )
	function SHADING:__set_vec4	(	shader_tab, id, x, y, z, w )
		local ref = shader_tab.vec4[ ((id-1)*4) + 1 ]
		if w then		set_real_4(	ref, x, y, z, w )
		elseif z then	set_real_3(	ref, x, y, z )
		elseif y then	set_real_2(	ref, x, y )
		else			set_real_1(	ref, x )
		end
	end

	function SHADING:set_vert_float( id, val )		self:__set_float( self.vert, id, val )	end
	function SHADING:set_geom_float( id, val )		self:__set_float( self.geom, id, val )	end
	function SHADING:set_frag_float( id, val )		self:__set_float( self.frag, id, val )	end
	function SHADING:set_comp_float( id, val )		self:__set_float( self.comp, id, val )	end

	function SHADING:get_vert_float( id )			return get_real_1( self.vert.float[id] )	end
	function SHADING:get_geom_float( id )			return get_real_1( self.geom.float[id] )	end
	function SHADING:get_frag_float( id )			return get_real_1( self.frag.float[id] )	end
	function SHADING:get_comp_float( id )			return get_real_1( self.comp.float[id] )	end

	function SHADING:set_vert_int( id, val )		self:__set_int( self.vert, id, val )	end
	function SHADING:set_geom_int( id, val )		self:__set_int( self.geom, id, val )	end
	function SHADING:set_frag_int( id, val )		self:__set_int( self.frag, id, val )	end
	function SHADING:set_comp_int( id, val )		self:__set_int( self.comp, id, val )	end

	function SHADING:get_vert_int( id )				return get_real_1( self.vert.int[id] )	end
	function SHADING:get_geom_int( id )				return get_real_1( self.geom.int[id] )	end
	function SHADING:get_frag_int( id )				return get_real_1( self.frag.int[id] )	end
	function SHADING:get_comp_int( id )				return get_real_1( self.comp.int[id] )	end

	function SHADING:set_vert_vec4( id, x,y,z, w )	self:__set_vec4( self.vert, id, x,y,z, w )	end
	function SHADING:set_geom_vec4( id, x,y,z, w )	self:__set_vec4( self.geom, id, x,y,z, w )	end
	function SHADING:set_frag_vec4( id, x,y,z, w )	self:__set_vec4( self.frag, id, x,y,z, w )	end
	function SHADING:set_comp_vec4( id, x,y,z, w )	self:__set_vec4( self.comp, id, x,y,z, w )	end

	function SHADING:clear_vert_vec4( id )			set_real_4( self.vert.vec4[ ((id-1)*4) + 1 ], 0,0,0,0 )	end
	function SHADING:clear_geom_vec4( id )			set_real_4( self.geom.vec4[ ((id-1)*4) + 1 ], 0,0,0,0 )	end
	function SHADING:clear_frag_vec4( id )			set_real_4( self.frag.vec4[ ((id-1)*4) + 1 ], 0,0,0,0 )	end
	function SHADING:clear_comp_vec4( id )			set_real_4( self.comp.vec4[ ((id-1)*4) + 1 ], 0,0,0,0 )	end

	SHADING.doc = { "( id, a1, a2 ) equivalent to set_vert_vec4( id, math.cos(a1), math.sin(a1), math.cos(a2), math.sin(a2) )",
					" a2 nil is considered as 0" }
	function SHADING:set_vert_vec4_cos_sin( id, a1, a2 )
		a1 = a1 * math.pi2
		if a2 then
			a2 = a2 * math.pi2
			self:__set_vec4( self.vert, id, math.cos(a1), math.sin(a1), math.cos(a2), math.sin(a2) )
		else
			self:__set_vec4( self.vert, id, math.cos(a1), math.sin(a1), 1, 0 )
		end
	end
	function SHADING:set_geom_vec4_cos_sin( id, a1, a2 )
		a1 = a1 * math.pi2
		if a2 then
			a2 = a2 * math.pi2
			self:__set_vec4( self.geom, id, math.cos(a1), math.sin(a1), math.cos(a2), math.sin(a2) )
		else
			self:__set_vec4( self.geom, id, math.cos(a1), math.sin(a1), 1, 0 )
		end
	end
	function SHADING:set_frag_vec4_cos_sin( id, a1, a2 )
		a1 = a1 * math.pi2
		if a2 then
			a2 = a2 * math.pi2
			self:__set_vec4( self.frag, id, math.cos(a1), math.sin(a1), math.cos(a2), math.sin(a2) )
		else
			self:__set_vec4( self.frag, id, math.cos(a1), math.sin(a1), 1, 0 )
		end
	end
	function SHADING:set_comp_vec4_cos_sin( id, a1, a2 )
		a1 = a1 * math.pi2
		if a2 then
			a2 = a2 * math.pi2
			self:__set_vec4( self.comp, id, math.cos(a1), math.sin(a1), math.cos(a2), math.sin(a2) )
		else
			self:__set_vec4( self.comp, id, math.cos(a1), math.sin(a1), 1, 0 )
		end
	end

	function SHADING:set_vert_vec4_tex_size( id, sx,sy )	self:__set_vec4( self.vert, id, sx,sy, 1./sx,1./sy )	end
	function SHADING:set_geom_vec4_tex_size( id, sx,sy )	self:__set_vec4( self.geom, id, sx,sy, 1./sx,1./sy )	end
	function SHADING:set_frag_vec4_tex_size( id, sx,sy )	self:__set_vec4( self.frag, id, sx,sy, 1./sx,1./sy )	end
	function SHADING:set_comp_vec4_tex_size( id, sx,sy )	self:__set_vec4( self.comp, id, sx,sy, 1./sx,1./sy )	end

	function SHADING:set_vert_vec4_x( id, x )		set_real_1( self.vert.vec4[id*4-3], x )		end
	function SHADING:set_vert_vec4_y( id, y )		set_real_1( self.vert.vec4[id*4-2], y )		end
	function SHADING:set_vert_vec4_z( id, z )		set_real_1( self.vert.vec4[id*4-1], z )		end
	function SHADING:set_vert_vec4_w( id, w )		set_real_1( self.vert.vec4[id*4],	w )		end

	function SHADING:set_geom_vec4_x( id, x )		set_real_1( self.geom.vec4[id*4-3], x )		end
	function SHADING:set_geom_vec4_y( id, y )		set_real_1( self.geom.vec4[id*4-2], y )		end
	function SHADING:set_geom_vec4_z( id, z )		set_real_1( self.geom.vec4[id*4-1], z )		end
	function SHADING:set_geom_vec4_w( id, w )		set_real_1( self.geom.vec4[id*4],	w )		end

	function SHADING:set_frag_vec4_x( id, x )		set_real_1( self.frag.vec4[id*4-3], x )		end
	function SHADING:set_frag_vec4_y( id, y )		set_real_1( self.frag.vec4[id*4-2], y )		end
	function SHADING:set_frag_vec4_z( id, z )		set_real_1( self.frag.vec4[id*4-1], z )		end
	function SHADING:set_frag_vec4_w( id, w )		set_real_1( self.frag.vec4[id*4],	w )		end

	function SHADING:set_comp_vec4_x( id, x )		set_real_1( self.comp.vec4[id*4-3], x )		end
	function SHADING:set_comp_vec4_y( id, y )		set_real_1( self.comp.vec4[id*4-2], y )		end
	function SHADING:set_comp_vec4_z( id, z )		set_real_1( self.comp.vec4[id*4-1], z )		end
	function SHADING:set_comp_vec4_w( id, w )		set_real_1( self.comp.vec4[id*4],	w )		end

	function SHADING:get_vert_vec4_x( id )			return get_real_1( self.vert.vec4[id*4-3] )		end
	function SHADING:get_vert_vec4_y( id )			return get_real_1( self.vert.vec4[id*4-2] )		end
	function SHADING:get_vert_vec4_z( id )			return get_real_1( self.vert.vec4[id*4-1] )		end
	function SHADING:get_vert_vec4_w( id )			return get_real_1( self.vert.vec4[id*4]   )		end

	function SHADING:get_geom_vec4_x( id )			return get_real_1( self.geom.vec4[id*4-3] )		end
	function SHADING:get_geom_vec4_y( id )			return get_real_1( self.geom.vec4[id*4-2] )		end
	function SHADING:get_geom_vec4_z( id )			return get_real_1( self.geom.vec4[id*4-1] )		end
	function SHADING:get_geom_vec4_w( id )			return get_real_1( self.geom.vec4[id*4]   )		end

	function SHADING:get_frag_vec4_x( id )			return get_real_1( self.frag.vec4[id*4-3] )		end
	function SHADING:get_frag_vec4_y( id )			return get_real_1( self.frag.vec4[id*4-2] )		end
	function SHADING:get_frag_vec4_z( id )			return get_real_1( self.frag.vec4[id*4-1] )		end
	function SHADING:get_frag_vec4_w( id )			return get_real_1( self.frag.vec4[id*4]   )		end

	function SHADING:get_comp_vec4_x( id )			return get_real_1( self.comp.vec4[id*4-3] )		end
	function SHADING:get_comp_vec4_y( id )			return get_real_1( self.comp.vec4[id*4-2] )		end
	function SHADING:get_comp_vec4_z( id )			return get_real_1( self.comp.vec4[id*4-1] )		end
	function SHADING:get_comp_vec4_w( id )			return get_real_1( self.comp.vec4[id*4]   )		end


	function SHADING:set_vert_float_1( v1 )									set_real_1( self.vert.float[1], v1 )	end
	function SHADING:set_vert_float_1_2( v1, v2 )							set_real_2( self.vert.float[1], v1, v2 )	end
	function SHADING:set_vert_float_1_3( v1, v2, v3 )						set_real_3( self.vert.float[1], v1, v2, v3 )	end
	function SHADING:set_vert_float_1_4( v1, v2, v3, v4 )					set_real_4( self.vert.float[1], v1, v2, v3, v4 )	end
	function SHADING:set_vert_float_1_5( v1, v2, v3, v4, v5 )				set_real_5( self.vert.float[1], v1, v2, v3, v4, v5 )	end
	function SHADING:set_vert_float_1_6( v1, v2, v3, v4, v5, v6 )			set_real_6( self.vert.float[1], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_vert_float_1_7( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.vert.float[1], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_vert_float_1_8( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.vert.float[1], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_vert_float_2( v2 )									set_real_1( self.vert.float[2], v2 )	end
	function SHADING:set_vert_float_2_3( v2, v3 )							set_real_2( self.vert.float[2], v2, v3 )	end
	function SHADING:set_vert_float_2_4( v2, v3, v4 )						set_real_3( self.vert.float[2], v2, v3, v4 )	end
	function SHADING:set_vert_float_2_5( v2, v3, v4, v5 )					set_real_4( self.vert.float[2], v2, v3, v4, v5 )	end
	function SHADING:set_vert_float_2_6( v2, v3, v4, v5, v6 )				set_real_5( self.vert.float[2], v2, v3, v4, v5, v6 )	end
	function SHADING:set_vert_float_2_7( v2, v3, v4, v5, v6, v7 )			set_real_6( self.vert.float[2], v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_vert_float_2_8( v2, v3, v4, v5, v6, v7, v8 )		set_real_7( self.vert.float[2], v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_vert_float_3( v3 )									set_real_1( self.vert.float[3], v3 )	end
	function SHADING:set_vert_float_3_4( v3, v4 )							set_real_2( self.vert.float[3], v3, v4 )	end
	function SHADING:set_vert_float_3_5( v3, v4, v5 )						set_real_3( self.vert.float[3], v3, v4, v5 )	end
	function SHADING:set_vert_float_3_6( v3, v4, v5, v6 )					set_real_4( self.vert.float[3], v3, v4, v5, v6 )	end
	function SHADING:set_vert_float_3_7( v3, v4, v5, v6, v7 )				set_real_5( self.vert.float[3], v3, v4, v5, v6, v7 )	end
	function SHADING:set_vert_float_3_8( v3, v4, v5, v6, v7, v8 )			set_real_6( self.vert.float[3], v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_vert_float_4( v4 )									set_real_1( self.vert.float[4], v4 )	end
	function SHADING:set_vert_float_4_5( v4, v5 )							set_real_2( self.vert.float[4], v4, v5 )	end
	function SHADING:set_vert_float_4_6( v4, v5, v6 )						set_real_3( self.vert.float[4], v4, v5, v6 )	end
	function SHADING:set_vert_float_4_7( v4, v5, v6, v7 )					set_real_4( self.vert.float[4], v4, v5, v6, v7 )	end
	function SHADING:set_vert_float_4_8( v4, v5, v6, v7, v8 )				set_real_5( self.vert.float[4], v4, v5, v6, v7, v8 )	end

	function SHADING:set_vert_float_5( v5 )									set_real_1( self.vert.float[5], v5 )	end
	function SHADING:set_vert_float_5_6( v5, v6 )							set_real_2( self.vert.float[5], v5, v6 )	end
	function SHADING:set_vert_float_5_7( v5, v6, v7 )						set_real_3( self.vert.float[5], v5, v6, v7 )	end
	function SHADING:set_vert_float_5_8( v5, v6, v7, v8 )					set_real_4( self.vert.float[5], v5, v6, v7, v8 )	end

	function SHADING:set_vert_float_6( v6 )									set_real_1( self.vert.float[6], v6 )	end
	function SHADING:set_vert_float_6_7( v6, v7 )							set_real_2( self.vert.float[6], v6, v7 )	end
	function SHADING:set_vert_float_6_8( v6, v7, v8 )						set_real_3( self.vert.float[6], v6, v7, v8 )	end

	function SHADING:set_vert_float_7( v7 )									set_real_1( self.vert.float[7], v7 )	end
	function SHADING:set_vert_float_7_8( v7, v8 )							set_real_2( self.vert.float[7], v7, v8 )	end

	function SHADING:set_vert_float_8( v8 )									set_real_1( self.vert.float[8], v8 )	end

	function SHADING:set_frag_float_1(   v1 )								set_real_1( self.frag.float[1], v1 )	end
	function SHADING:set_frag_float_1_2( v1, v2 )							set_real_2( self.frag.float[1], v1, v2 )	end
	function SHADING:set_frag_float_1_3( v1, v2, v3 )						set_real_3( self.frag.float[1], v1, v2, v3 )	end
	function SHADING:set_frag_float_1_4( v1, v2, v3, v4 )					set_real_4( self.frag.float[1], v1, v2, v3, v4 )	end
	function SHADING:set_frag_float_1_5( v1, v2, v3, v4, v5 )				set_real_5( self.frag.float[1], v1, v2, v3, v4, v5 )	end
	function SHADING:set_frag_float_1_6( v1, v2, v3, v4, v5, v6 )			set_real_6( self.frag.float[1], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_frag_float_1_7( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.frag.float[1], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_frag_float_1_8( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.frag.float[1], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_frag_float_2(   v1 )								set_real_1( self.frag.float[2], v1 )	end
	function SHADING:set_frag_float_2_3( v1, v2 )							set_real_2( self.frag.float[2], v1, v2 )	end
	function SHADING:set_frag_float_2_4( v1, v2, v3 )						set_real_3( self.frag.float[2], v1, v2, v3 )	end
	function SHADING:set_frag_float_2_5( v1, v2, v3, v4 )					set_real_4( self.frag.float[2], v1, v2, v3, v4 )	end
	function SHADING:set_frag_float_2_6( v1, v2, v3, v4, v5 )				set_real_5( self.frag.float[2], v1, v2, v3, v4, v5 )	end
	function SHADING:set_frag_float_2_7( v1, v2, v3, v4, v5, v6 )			set_real_6( self.frag.float[2], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_frag_float_2_8( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.frag.float[2], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_frag_float_2_9( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.frag.float[2], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_frag_float_3(   v1 )								set_real_1( self.frag.float[3], v1 )	end
	function SHADING:set_frag_float_3_4( v1, v2 )							set_real_2( self.frag.float[3], v1, v2 )	end
	function SHADING:set_frag_float_3_5( v1, v2, v3 )						set_real_3( self.frag.float[3], v1, v2, v3 )	end
	function SHADING:set_frag_float_3_6( v1, v2, v3, v4 )					set_real_4( self.frag.float[3], v1, v2, v3, v4 )	end
	function SHADING:set_frag_float_3_7( v1, v2, v3, v4, v5 )				set_real_5( self.frag.float[3], v1, v2, v3, v4, v5 )	end
	function SHADING:set_frag_float_3_8( v1, v2, v3, v4, v5, v6 )			set_real_6( self.frag.float[3], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_frag_float_3_9( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.frag.float[3], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_frag_float_3_10( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.frag.float[3], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_frag_float_4(   v1 )								set_real_1( self.frag.float[4], v1 )	end
	function SHADING:set_frag_float_4_5( v1, v2 )							set_real_2( self.frag.float[4], v1, v2 )	end
	function SHADING:set_frag_float_4_6( v1, v2, v3 )						set_real_3( self.frag.float[4], v1, v2, v3 )	end
	function SHADING:set_frag_float_4_7( v1, v2, v3, v4 )					set_real_4( self.frag.float[4], v1, v2, v3, v4 )	end
	function SHADING:set_frag_float_4_8( v1, v2, v3, v4, v5 )				set_real_5( self.frag.float[4], v1, v2, v3, v4, v5 )	end
	function SHADING:set_frag_float_4_9( v1, v2, v3, v4, v5, v6 )			set_real_6( self.frag.float[4], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_frag_float_4_10( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.frag.float[4], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_frag_float_4_11( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.frag.float[4], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_frag_float_5(   v1 )								set_real_1( self.frag.float[5], v1 )	end
	function SHADING:set_frag_float_5_6( v1, v2 )							set_real_2( self.frag.float[5], v1, v2 )	end
	function SHADING:set_frag_float_5_7( v1, v2, v3 )						set_real_3( self.frag.float[5], v1, v2, v3 )	end
	function SHADING:set_frag_float_5_8( v1, v2, v3, v4 )					set_real_4( self.frag.float[5], v1, v2, v3, v4 )	end
	function SHADING:set_frag_float_5_9( v1, v2, v3, v4, v5 )				set_real_5( self.frag.float[5], v1, v2, v3, v4, v5 )	end
	function SHADING:set_frag_float_5_10( v1, v2, v3, v4, v5, v6 )			set_real_6( self.frag.float[5], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_frag_float_5_11( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.frag.float[5], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_frag_float_5_12( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.frag.float[5], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_frag_float_6( 	 v1 )								set_real_1( self.frag.float[6], v1 )	end
	function SHADING:set_frag_float_6_7( v1, v2 )							set_real_2( self.frag.float[6], v1, v2 )	end
	function SHADING:set_frag_float_6_8( v1, v2, v3 )						set_real_3( self.frag.float[6], v1, v2, v3 )	end
	function SHADING:set_frag_float_6_9( v1, v2, v3, v4 )					set_real_4( self.frag.float[6], v1, v2, v3, v4 )	end
	function SHADING:set_frag_float_6_10( v1, v2, v3, v4, v5 )				set_real_5( self.frag.float[6], v1, v2, v3, v4, v5 )	end
	function SHADING:set_frag_float_6_11( v1, v2, v3, v4, v5, v6 )			set_real_6( self.frag.float[6], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_frag_float_6_12( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.frag.float[6], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_frag_float_6_13( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.frag.float[6], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_frag_float_7(    v1 )								set_real_1( self.frag.float[7], v1 )	end
	function SHADING:set_frag_float_7_8(  v1, v2 )							set_real_2( self.frag.float[7], v1, v2 )	end
	function SHADING:set_frag_float_7_9(  v1, v2, v3 )						set_real_3( self.frag.float[7], v1, v2, v3 )	end
	function SHADING:set_frag_float_7_10( v1, v2, v3, v4 )					set_real_4( self.frag.float[7], v1, v2, v3, v4 )	end
	function SHADING:set_frag_float_7_11( v1, v2, v3, v4, v5 )				set_real_5( self.frag.float[7], v1, v2, v3, v4, v5 )	end
	function SHADING:set_frag_float_7_12( v1, v2, v3, v4, v5, v6 )			set_real_6( self.frag.float[7], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_frag_float_7_13( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.frag.float[7], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_frag_float_7_14( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.frag.float[7], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_frag_float_8(    v1 )								set_real_1( self.frag.float[8], v1 )	end
	function SHADING:set_frag_float_8_9(  v1, v2 )							set_real_2( self.frag.float[8], v1, v2 )	end
	function SHADING:set_frag_float_8_10( v1, v2, v3 )						set_real_3( self.frag.float[8], v1, v2, v3 )	end
	function SHADING:set_frag_float_8_11( v1, v2, v3, v4 )					set_real_4( self.frag.float[8], v1, v2, v3, v4 )	end
	function SHADING:set_frag_float_8_12( v1, v2, v3, v4, v5 )				set_real_5( self.frag.float[8], v1, v2, v3, v4, v5 )	end
	function SHADING:set_frag_float_8_13( v1, v2, v3, v4, v5, v6 )			set_real_6( self.frag.float[8], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_frag_float_8_14( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.frag.float[8], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_frag_float_8_15( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.frag.float[8], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_frag_float_9(	  v1 )								set_real_1( self.frag.float[9], v1 )	end
	function SHADING:set_frag_float_9_10( v1, v2 )							set_real_2( self.frag.float[9], v1, v2 )	end
	function SHADING:set_frag_float_9_11( v1, v2, v3 )						set_real_3( self.frag.float[9], v1, v2, v3 )	end
	function SHADING:set_frag_float_9_12( v1, v2, v3, v4 )					set_real_4( self.frag.float[9], v1, v2, v3, v4 )	end
	function SHADING:set_frag_float_9_13( v1, v2, v3, v4, v5 )				set_real_5( self.frag.float[9], v1, v2, v3, v4, v5 )	end
	function SHADING:set_frag_float_9_14( v1, v2, v3, v4, v5, v6 )			set_real_6( self.frag.float[9], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_frag_float_9_15( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.frag.float[9], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_frag_float_9_16( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.frag.float[9], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_frag_float_10(	   v1 )								set_real_1( self.frag.float[10], v1 )	end
	function SHADING:set_frag_float_10_11( v1, v2 )							set_real_2( self.frag.float[10], v1, v2 )	end
	function SHADING:set_frag_float_10_12( v1, v2, v3 )						set_real_3( self.frag.float[10], v1, v2, v3 )	end
	function SHADING:set_frag_float_10_13( v1, v2, v3, v4 )					set_real_4( self.frag.float[10], v1, v2, v3, v4 )	end
	function SHADING:set_frag_float_10_14( v1, v2, v3, v4, v5 )				set_real_5( self.frag.float[10], v1, v2, v3, v4, v5 )	end
	function SHADING:set_frag_float_10_15( v1, v2, v3, v4, v5, v6 )			set_real_6( self.frag.float[10], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_frag_float_10_16( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.frag.float[10], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_frag_float_10_17( ... )							set_real_7( self.frag.float[10], ... )	end

	function SHADING:set_frag_float_11(	   v1 )								set_real_1( self.frag.float[11], v1 )	end
	function SHADING:set_frag_float_11_12( v1, v2 )							set_real_2( self.frag.float[11], v1, v2 )	end
	function SHADING:set_frag_float_11_13( v1, v2, v3 )						set_real_3( self.frag.float[11], v1, v2, v3 )	end
	function SHADING:set_frag_float_11_14( v1, v2, v3, v4 )					set_real_4( self.frag.float[11], v1, v2, v3, v4 )	end
	function SHADING:set_frag_float_11_15( v1, v2, v3, v4, v5 )				set_real_5( self.frag.float[11], v1, v2, v3, v4, v5 )	end
	function SHADING:set_frag_float_11_16( v1, v2, v3, v4, v5, v6 )			set_real_6( self.frag.float[11], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_frag_float_11_17( ... )							set_real_7( self.frag.float[11], ... )	end
	function SHADING:set_frag_float_11_18( ... )							set_real_8( self.frag.float[11], ... )	end

	function SHADING:set_frag_float_12(	   v1 )								set_real_1( self.frag.float[12], v1 )	end
	function SHADING:set_frag_float_12_13( v1, v2 )							set_real_2( self.frag.float[12], v1, v2 )	end
	function SHADING:set_frag_float_12_14( v1, v2, v3 )						set_real_3( self.frag.float[12], v1, v2, v3 )	end
	function SHADING:set_frag_float_12_15( v1, v2, v3, v4 )					set_real_4( self.frag.float[12], v1, v2, v3, v4 )	end
	function SHADING:set_frag_float_12_16( v1, v2, v3, v4, v5 )				set_real_5( self.frag.float[12], v1, v2, v3, v4, v5 )	end
	function SHADING:set_frag_float_12_17( ... )							set_real_6( self.frag.float[12], ... )	end
	function SHADING:set_frag_float_12_18( ... )							set_real_7( self.frag.float[12], ... )	end
	function SHADING:set_frag_float_12_19( ... )							set_real_8( self.frag.float[12], ... )	end

	function SHADING:set_frag_float_13(	   v1 )								set_real_1( self.frag.float[13], v1 )	end
	function SHADING:set_frag_float_13_14( v1, v2 )							set_real_2( self.frag.float[13], v1, v2 )	end
	function SHADING:set_frag_float_13_15( v1, v2, v3 )						set_real_3( self.frag.float[13], v1, v2, v3 )	end
	function SHADING:set_frag_float_13_16( v1, v2, v3, v4 )					set_real_4( self.frag.float[13], v1, v2, v3, v4 )	end
	function SHADING:set_frag_float_13_17( ... )							set_real_5( self.frag.float[13], ... )	end
	function SHADING:set_frag_float_13_18( ... )							set_real_6( self.frag.float[13], ... )	end
	function SHADING:set_frag_float_13_19( ... )							set_real_7( self.frag.float[13], ... )	end
	function SHADING:set_frag_float_13_20( ... )							set_real_8( self.frag.float[13], ... )	end

	function SHADING:set_frag_float_14(	   v1 )								set_real_1( self.frag.float[14], v1 )	end
	function SHADING:set_frag_float_14_15( v1, v2 )							set_real_2( self.frag.float[14], v1, v2 )	end
	function SHADING:set_frag_float_14_16( v1, v2, v3 )						set_real_3( self.frag.float[14], v1, v2, v3 )	end
	function SHADING:set_frag_float_14_17( ... )							set_real_4( self.frag.float[14], ... )	end
	function SHADING:set_frag_float_14_18( ... )							set_real_5( self.frag.float[14], ... )	end
	function SHADING:set_frag_float_14_19( ... )							set_real_6( self.frag.float[14], ... )	end
	function SHADING:set_frag_float_14_20( ... )							set_real_7( self.frag.float[14], ... )	end
	function SHADING:set_frag_float_14_21( ... )							set_real_8( self.frag.float[14], ... )	end
	
	function SHADING:set_frag_float_15(	   v1 )								set_real_1( self.frag.float[15], v1 )	end
	function SHADING:set_frag_float_15_16( v1, v2 )							set_real_2( self.frag.float[15], v1, v2 )	end
	function SHADING:set_frag_float_15_17( ... )							set_real_3( self.frag.float[15], ... )	end
	function SHADING:set_frag_float_15_18( ... )							set_real_4( self.frag.float[15], ... )	end
	function SHADING:set_frag_float_15_19( ... )							set_real_5( self.frag.float[15], ... )	end
	function SHADING:set_frag_float_15_20( ... )							set_real_6( self.frag.float[15], ... )	end
	function SHADING:set_frag_float_15_21( ... )							set_real_7( self.frag.float[15], ... )	end
	function SHADING:set_frag_float_15_22( ... )							set_real_8( self.frag.float[15], ... )	end

	function SHADING:set_frag_float_16(	   v1 )								set_real_1( self.frag.float[16], v1 )	end
	function SHADING:set_frag_float_16_17( ... )							set_real_2( self.frag.float[16], ... )	end
	function SHADING:set_frag_float_16_18( ... )							set_real_3( self.frag.float[16], ... )	end
	function SHADING:set_frag_float_16_19( ... )							set_real_4( self.frag.float[16], ... )	end
	function SHADING:set_frag_float_16_20( ... )							set_real_5( self.frag.float[16], ... )	end
	function SHADING:set_frag_float_16_21( ... )							set_real_6( self.frag.float[16], ... )	end
	function SHADING:set_frag_float_16_22( ... )							set_real_7( self.frag.float[16], ... )	end
	function SHADING:set_frag_float_16_23( ... )							set_real_8( self.frag.float[16], ... )	end

	function SHADING:set_frag_float_17(	   v1 )								set_real_1( self.frag.float[17], v1 )	end
	function SHADING:set_frag_float_17_18( ... )							set_real_2( self.frag.float[17], ... )	end
	function SHADING:set_frag_float_17_19( ... )							set_real_3( self.frag.float[17], ... )	end
	function SHADING:set_frag_float_17_20( ... )							set_real_4( self.frag.float[17], ... )	end
	function SHADING:set_frag_float_17_21( ... )							set_real_5( self.frag.float[17], ... )	end
	function SHADING:set_frag_float_17_22( ... )							set_real_6( self.frag.float[17], ... )	end
	function SHADING:set_frag_float_17_23( ... )							set_real_7( self.frag.float[17], ... )	end
	function SHADING:set_frag_float_17_24( ... )							set_real_8( self.frag.float[17], ... )	end
	
	function SHADING:set_frag_float_18(	   v1 )								set_real_1( self.frag.float[18], v1 )	end
	function SHADING:set_frag_float_18_19( ... )							set_real_2( self.frag.float[18], ... )	end
	function SHADING:set_frag_float_18_20( ... )							set_real_3( self.frag.float[18], ... )	end
	function SHADING:set_frag_float_18_21( ... )							set_real_4( self.frag.float[18], ... )	end
	function SHADING:set_frag_float_18_22( ... )							set_real_5( self.frag.float[18], ... )	end
	function SHADING:set_frag_float_18_23( ... )							set_real_6( self.frag.float[18], ... )	end
	function SHADING:set_frag_float_18_24( ... )							set_real_7( self.frag.float[18], ... )	end
	
	function SHADING:set_frag_float_19(	   v1 )								set_real_1( self.frag.float[19], v1 )	end
	function SHADING:set_frag_float_19_20( ... )							set_real_2( self.frag.float[19], ... )	end
	function SHADING:set_frag_float_19_21( ... )							set_real_3( self.frag.float[19], ... )	end
	function SHADING:set_frag_float_19_22( ... )							set_real_4( self.frag.float[19], ... )	end
	function SHADING:set_frag_float_19_23( ... )							set_real_5( self.frag.float[19], ... )	end
	function SHADING:set_frag_float_19_24( ... )							set_real_6( self.frag.float[19], ... )	end

	function SHADING:set_frag_float_20(	   v1 )								set_real_1( self.frag.float[20], v1 )	end
	function SHADING:set_frag_float_20_21( ... )							set_real_2( self.frag.float[20], ... )	end
	function SHADING:set_frag_float_20_22( ... )							set_real_3( self.frag.float[20], ... )	end
	function SHADING:set_frag_float_20_23( ... )							set_real_4( self.frag.float[20], ... )	end
	function SHADING:set_frag_float_20_24( ... )							set_real_5( self.frag.float[20], ... )	end

	function SHADING:set_frag_float_21(	   v1 )								set_real_1( self.frag.float[21], v1 )	end
	function SHADING:set_frag_float_21_22( ... )							set_real_2( self.frag.float[21], ... )	end
	function SHADING:set_frag_float_21_23( ... )							set_real_3( self.frag.float[21], ... )	end
	function SHADING:set_frag_float_21_24( ... )							set_real_4( self.frag.float[21], ... )	end

	function SHADING:set_frag_float_22(	   v1 )								set_real_1( self.frag.float[22], v1 )	end
	function SHADING:set_frag_float_22_23( ... )							set_real_2( self.frag.float[22], ... )	end
	function SHADING:set_frag_float_22_24( ... )							set_real_3( self.frag.float[22], ... )	end

	function SHADING:set_frag_float_23(	   v1 )								set_real_1( self.frag.float[23], v1 )	end
	function SHADING:set_frag_float_23_24( ... )							set_real_2( self.frag.float[23], ... )	end

	function SHADING:set_frag_float_24(	   v1 )								set_real_1( self.frag.float[24], v1 )	end

--COMPUTE
	function SHADING:set_comp_float_1(   v1 )								set_real_1( self.comp.float[1], v1 )	end
	function SHADING:set_comp_float_1_2( v1, v2 )							set_real_2( self.comp.float[1], v1, v2 )	end
	function SHADING:set_comp_float_1_3( v1, v2, v3 )						set_real_3( self.comp.float[1], v1, v2, v3 )	end
	function SHADING:set_comp_float_1_4( v1, v2, v3, v4 )					set_real_4( self.comp.float[1], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_1_5( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[1], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_1_6( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[1], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_1_7( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[1], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_1_8( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[1], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_2(   v1 )								set_real_1( self.comp.float[2], v1 )	end
	function SHADING:set_comp_float_2_3( v1, v2 )							set_real_2( self.comp.float[2], v1, v2 )	end
	function SHADING:set_comp_float_2_4( v1, v2, v3 )						set_real_3( self.comp.float[2], v1, v2, v3 )	end
	function SHADING:set_comp_float_2_5( v1, v2, v3, v4 )					set_real_4( self.comp.float[2], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_2_6( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[2], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_2_7( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[2], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_2_8( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[2], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_2_9( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[2], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_3(   v1 )								set_real_1( self.comp.float[3], v1 )	end
	function SHADING:set_comp_float_3_4( v1, v2 )							set_real_2( self.comp.float[3], v1, v2 )	end
	function SHADING:set_comp_float_3_5( v1, v2, v3 )						set_real_3( self.comp.float[3], v1, v2, v3 )	end
	function SHADING:set_comp_float_3_6( v1, v2, v3, v4 )					set_real_4( self.comp.float[3], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_3_7( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[3], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_3_8( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[3], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_3_9( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[3], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_3_10( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[3], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_4(   v1 )								set_real_1( self.comp.float[4], v1 )	end
	function SHADING:set_comp_float_4_5( v1, v2 )							set_real_2( self.comp.float[4], v1, v2 )	end
	function SHADING:set_comp_float_4_6( v1, v2, v3 )						set_real_3( self.comp.float[4], v1, v2, v3 )	end
	function SHADING:set_comp_float_4_7( v1, v2, v3, v4 )					set_real_4( self.comp.float[4], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_4_8( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[4], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_4_9( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[4], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_4_10( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[4], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_4_11( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[4], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_5(   v1 )								set_real_1( self.comp.float[5], v1 )	end
	function SHADING:set_comp_float_5_6( v1, v2 )							set_real_2( self.comp.float[5], v1, v2 )	end
	function SHADING:set_comp_float_5_7( v1, v2, v3 )						set_real_3( self.comp.float[5], v1, v2, v3 )	end
	function SHADING:set_comp_float_5_8( v1, v2, v3, v4 )					set_real_4( self.comp.float[5], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_5_9( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[5], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_5_10( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[5], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_5_11( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[5], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_5_12( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[5], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_6( 	 v1 )								set_real_1( self.comp.float[6], v1 )	end
	function SHADING:set_comp_float_6_7( v1, v2 )							set_real_2( self.comp.float[6], v1, v2 )	end
	function SHADING:set_comp_float_6_8( v1, v2, v3 )						set_real_3( self.comp.float[6], v1, v2, v3 )	end
	function SHADING:set_comp_float_6_9( v1, v2, v3, v4 )					set_real_4( self.comp.float[6], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_6_10( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[6], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_6_11( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[6], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_6_12( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[6], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_6_13( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[6], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_7(    v1 )								set_real_1( self.comp.float[7], v1 )	end
	function SHADING:set_comp_float_7_8(  v1, v2 )							set_real_2( self.comp.float[7], v1, v2 )	end
	function SHADING:set_comp_float_7_9(  v1, v2, v3 )						set_real_3( self.comp.float[7], v1, v2, v3 )	end
	function SHADING:set_comp_float_7_10( v1, v2, v3, v4 )					set_real_4( self.comp.float[7], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_7_11( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[7], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_7_12( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[7], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_7_13( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[7], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_7_14( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[7], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_8(    v1 )								set_real_1( self.comp.float[8], v1 )	end
	function SHADING:set_comp_float_8_9(  v1, v2 )							set_real_2( self.comp.float[8], v1, v2 )	end
	function SHADING:set_comp_float_8_10( v1, v2, v3 )						set_real_3( self.comp.float[8], v1, v2, v3 )	end
	function SHADING:set_comp_float_8_11( v1, v2, v3, v4 )					set_real_4( self.comp.float[8], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_8_12( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[8], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_8_13( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[8], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_8_14( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[8], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_8_15( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[8], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_9(	  v1 )								set_real_1( self.comp.float[9], v1 )	end
	function SHADING:set_comp_float_9_10( v1, v2 )							set_real_2( self.comp.float[9], v1, v2 )	end
	function SHADING:set_comp_float_9_11( v1, v2, v3 )						set_real_3( self.comp.float[9], v1, v2, v3 )	end
	function SHADING:set_comp_float_9_12( v1, v2, v3, v4 )					set_real_4( self.comp.float[9], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_9_13( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[9], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_9_14( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[9], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_9_15( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[9], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_9_16( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[9], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_10(	   v1 )								set_real_1( self.comp.float[10], v1 )	end
	function SHADING:set_comp_float_10_11( v1, v2 )							set_real_2( self.comp.float[10], v1, v2 )	end
	function SHADING:set_comp_float_10_12( v1, v2, v3 )						set_real_3( self.comp.float[10], v1, v2, v3 )	end
	function SHADING:set_comp_float_10_13( v1, v2, v3, v4 )					set_real_4( self.comp.float[10], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_10_14( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[10], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_10_15( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[10], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_10_16( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[10], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_10_17( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[10], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_11(	   v1 )								set_real_1( self.comp.float[11], v1 )	end
	function SHADING:set_comp_float_11_12( v1, v2 )							set_real_2( self.comp.float[11], v1, v2 )	end
	function SHADING:set_comp_float_11_13( v1, v2, v3 )						set_real_3( self.comp.float[11], v1, v2, v3 )	end
	function SHADING:set_comp_float_11_14( v1, v2, v3, v4 )					set_real_4( self.comp.float[11], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_11_15( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[11], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_11_16( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[11], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_11_17( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[11], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_11_18( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[11], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_12(	   v1 )								set_real_1( self.comp.float[12], v1 )	end
	function SHADING:set_comp_float_12_13( v1, v2 )							set_real_2( self.comp.float[12], v1, v2 )	end
	function SHADING:set_comp_float_12_14( v1, v2, v3 )						set_real_3( self.comp.float[12], v1, v2, v3 )	end
	function SHADING:set_comp_float_12_15( v1, v2, v3, v4 )					set_real_4( self.comp.float[12], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_12_16( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[12], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_12_17( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[12], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_12_18( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[12], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_12_19( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[12], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_13(	   v1 )								set_real_1( self.comp.float[13], v1 )	end
	function SHADING:set_comp_float_13_14( v1, v2 )							set_real_2( self.comp.float[13], v1, v2 )	end
	function SHADING:set_comp_float_13_15( v1, v2, v3 )						set_real_3( self.comp.float[13], v1, v2, v3 )	end
	function SHADING:set_comp_float_13_16( v1, v2, v3, v4 )					set_real_4( self.comp.float[13], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_13_17( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[13], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_13_18( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[13], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_13_19( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[13], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_13_20( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[13], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_14(	   v1 )								set_real_1( self.comp.float[14], v1 )	end
	function SHADING:set_comp_float_14_15( v1, v2 )							set_real_2( self.comp.float[14], v1, v2 )	end
	function SHADING:set_comp_float_14_16( v1, v2, v3 )						set_real_3( self.comp.float[14], v1, v2, v3 )	end
	function SHADING:set_comp_float_14_17( v1, v2, v3, v4 )					set_real_4( self.comp.float[14], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_14_18( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[14], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_14_19( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[14], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_14_20( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[14], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_14_21( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[14], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_15(	   v1 )								set_real_1( self.comp.float[15], v1 )	end
	function SHADING:set_comp_float_15_16( v1, v2 )							set_real_2( self.comp.float[15], v1, v2 )	end
	function SHADING:set_comp_float_15_17( v1, v2, v3 )						set_real_3( self.comp.float[15], v1, v2, v3 )	end
	function SHADING:set_comp_float_15_18( v1, v2, v3, v4 )					set_real_4( self.comp.float[15], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_15_19( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[15], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_15_20( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[15], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_15_21( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[15], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_15_22( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[15], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_16(	   v1 )								set_real_1( self.comp.float[16], v1 )	end
	function SHADING:set_comp_float_16_17( v1, v2 )							set_real_2( self.comp.float[16], v1, v2 )	end
	function SHADING:set_comp_float_16_18( v1, v2, v3 )						set_real_3( self.comp.float[16], v1, v2, v3 )	end
	function SHADING:set_comp_float_16_19( v1, v2, v3, v4 )					set_real_4( self.comp.float[16], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_16_20( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[16], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_16_21( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[16], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_16_22( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[16], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_16_23( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[16], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_17(	   v1 )								set_real_1( self.comp.float[17], v1 )	end
	function SHADING:set_comp_float_17_18( v1, v2 )							set_real_2( self.comp.float[17], v1, v2 )	end
	function SHADING:set_comp_float_17_19( v1, v2, v3 )						set_real_3( self.comp.float[17], v1, v2, v3 )	end
	function SHADING:set_comp_float_17_20( v1, v2, v3, v4 )					set_real_4( self.comp.float[17], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_17_21( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[17], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_17_22( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[17], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_17_23( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[17], v1, v2, v3, v4, v5, v6, v7 )	end
	function SHADING:set_comp_float_17_24( v1, v2, v3, v4, v5, v6, v7, v8 )	set_real_8( self.comp.float[17], v1, v2, v3, v4, v5, v6, v7, v8 )	end

	function SHADING:set_comp_float_18(	   v1 )								set_real_1( self.comp.float[18], v1 )	end
	function SHADING:set_comp_float_18_19( v1, v2 )							set_real_2( self.comp.float[18], v1, v2 )	end
	function SHADING:set_comp_float_18_20( v1, v2, v3 )						set_real_3( self.comp.float[18], v1, v2, v3 )	end
	function SHADING:set_comp_float_18_21( v1, v2, v3, v4 )					set_real_4( self.comp.float[18], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_18_22( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[18], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_18_23( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[18], v1, v2, v3, v4, v5, v6 )	end
	function SHADING:set_comp_float_18_24( v1, v2, v3, v4, v5, v6, v7 )		set_real_7( self.comp.float[18], v1, v2, v3, v4, v5, v6, v7 )	end

	function SHADING:set_comp_float_19(	   v1 )								set_real_1( self.comp.float[19], v1 )	end
	function SHADING:set_comp_float_19_20( v1, v2 )							set_real_2( self.comp.float[19], v1, v2 )	end
	function SHADING:set_comp_float_19_21( v1, v2, v3 )						set_real_3( self.comp.float[19], v1, v2, v3 )	end
	function SHADING:set_comp_float_19_22( v1, v2, v3, v4 )					set_real_4( self.comp.float[19], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_19_23( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[19], v1, v2, v3, v4, v5 )	end
	function SHADING:set_comp_float_19_24( v1, v2, v3, v4, v5, v6 )			set_real_6( self.comp.float[19], v1, v2, v3, v4, v5, v6 )	end

	function SHADING:set_comp_float_20(	   v1 )								set_real_1( self.comp.float[20], v1 )	end
	function SHADING:set_comp_float_20_21( v1, v2 )							set_real_2( self.comp.float[20], v1, v2 )	end
	function SHADING:set_comp_float_20_22( v1, v2, v3 )						set_real_3( self.comp.float[20], v1, v2, v3 )	end
	function SHADING:set_comp_float_20_23( v1, v2, v3, v4 )					set_real_4( self.comp.float[20], v1, v2, v3, v4 )	end
	function SHADING:set_comp_float_20_24( v1, v2, v3, v4, v5 )				set_real_5( self.comp.float[20], v1, v2, v3, v4, v5 )	end

	function SHADING:set_comp_float_21(	   v1 )								set_real_1( self.comp.float[21], v1 )	end
	function SHADING:set_comp_float_21_22( v1, v2 )							set_real_2( self.comp.float[21], v1, v2 )	end
	function SHADING:set_comp_float_21_23( v1, v2, v3 )						set_real_3( self.comp.float[21], v1, v2, v3 )	end
	function SHADING:set_comp_float_21_24( v1, v2, v3, v4 )					set_real_4( self.comp.float[21], v1, v2, v3, v4 )	end

	function SHADING:set_comp_float_22(	   v1 )								set_real_1( self.comp.float[22], v1 )	end
	function SHADING:set_comp_float_22_23( v1, v2 )							set_real_2( self.comp.float[22], v1, v2 )	end
	function SHADING:set_comp_float_22_24( v1, v2, v3 )						set_real_3( self.comp.float[22], v1, v2, v3 )	end

	function SHADING:set_comp_float_23(	   v1 )								set_real_1( self.comp.float[23], v1 )	end
	function SHADING:set_comp_float_23_24( v1, v2 )							set_real_2( self.comp.float[23], v1, v2 )	end

	function SHADING:set_comp_float_24(	   v1 )								set_real_1( self.comp.float[24], v1 )	end

	function SHADING:set_comp_int_1(   v1 )									set_real_1( self.comp.int[1], v1 )	end
	function SHADING:set_comp_int_1_2( v1, v2 )								set_real_2( self.comp.int[1], v1, v2 )	end
	function SHADING:set_comp_int_1_3( v1, v2, v3 )							set_real_3( self.comp.int[1], v1, v2, v3 )	end
	function SHADING:set_comp_int_1_4( v1, v2, v3, v4 )						set_real_4( self.comp.int[1], v1, v2, v3, v4 )	end

	function SHADING:set_comp_int_2(   v2 )									set_real_1( self.comp.int[2], v2 )	end
	function SHADING:set_comp_int_2_3( v2, v3 )								set_real_2( self.comp.int[2], v2, v3 )	end
	function SHADING:set_comp_int_2_4( v2,v3, v4 )							set_real_3( self.comp.int[2], v2, v3, v4 )	end

	function SHADING:set_comp_int_3(   v3 )									set_real_1( self.comp.int[3], v3 )	end
	function SHADING:set_comp_int_3_4( v3, v4 )								set_real_2( self.comp.int[3], v3, v4 )	end

	function SHADING:set_comp_int_4(   v4 )									set_real_1( self.comp.int[4], v4 )	end

	function SHADING:set_vert_int_1(   v1 )									set_real_1( self.vert.int[1], v1 )	end
	function SHADING:set_vert_int_1_2( v1, v2 )								set_real_2( self.vert.int[1], v1, v2 )	end
	function SHADING:set_vert_int_1_3( v1, v2, v3 )							set_real_3( self.vert.int[1], v1, v2, v3 )	end
	function SHADING:set_vert_int_1_4( v1, v2, v3, v4 )						set_real_4( self.vert.int[1], v1, v2, v3, v4 )	end

	function SHADING:set_vert_int_2(   v2 )									set_real_1( self.vert.int[2], v2 )	end
	function SHADING:set_vert_int_2_3( v2, v3 )								set_real_2( self.vert.int[2], v2, v3 )	end
	function SHADING:set_vert_int_2_4( v2, v3, v4 )							set_real_3( self.vert.int[2], v2, v3, v4 )	end

	function SHADING:set_vert_int_3(   v3 )									set_real_1( self.vert.int[3], v3 )	end
	function SHADING:set_vert_int_3_4( v3, v4 )								set_real_2( self.vert.int[3], v3, v4 )	end

	function SHADING:set_vert_int_4( v4 )									set_real_1( self.vert.int[4], v4 )	end


	function SHADING:set_vert_vec3_6_table18( t )

		aaa.shading.set_vert_vec4_spe( self.ref.obj, t )
		
--this one is faster than the previous one when using table	and not user datum
		-- aaa.shading.set_vert_vec4_spe( self.ref.obj,
		-- 	t[1],	t[2],	t[3],
		-- 	t[4],	t[5],	t[6],
		-- 	t[7],	t[8],	t[9],
		-- 	t[10],	t[11],	t[12],
		-- 	t[13],	t[14],	t[15],
		-- 	t[16],	t[17],	t[18]
		-- )
--this one is slower
		-- local vref = self.vert.vec4
		-- set_real_3(	vref[1],	t[1],	t[2],	t[3] )
		-- set_real_3(	vref[5],	t[4],	t[5],	t[6] )
		-- set_real_3(	vref[9],	t[7],	t[8],	t[9] )
		-- set_real_3(	vref[13],	t[10],	t[11],	t[12] )
		-- set_real_3(	vref[17],	t[13],	t[14],	t[15] )
		-- set_real_3(	vref[21],	t[16],	t[17],	t[18] )
	end

-- GET
--
	function SHADING:get_frag_float_1()		return get_real_1( self.frag.float[1] )	end
	function SHADING:get_frag_float_1_2()	return get_real_2( self.frag.float[1] )	end
	function SHADING:get_frag_float_1_3()	return get_real_3( self.frag.float[1] )	end
	function SHADING:get_frag_float_1_4()	return get_real_4( self.frag.float[1] )	end
	function SHADING:get_frag_float_1_5()	return get_real_5( self.frag.float[1] )	end
	function SHADING:get_frag_float_1_6()	return get_real_6( self.frag.float[1] )	end
	function SHADING:get_frag_float_1_7()	return get_real_7( self.frag.float[1] )	end
	function SHADING:get_frag_float_1_8()	return get_real_8( self.frag.float[1] )	end

	function SHADING:get_frag_float_2()		return get_real_1( self.frag.float[2] )	end
	function SHADING:get_frag_float_3()		return get_real_1( self.frag.float[3] )	end
	function SHADING:get_frag_float_4()		return get_real_1( self.frag.float[4] )	end
	function SHADING:get_frag_float_5()		return get_real_1( self.frag.float[5] )	end
	function SHADING:get_frag_float_6()		return get_real_1( self.frag.float[6] )	end
	function SHADING:get_frag_float_7()		return get_real_1( self.frag.float[7] )	end
	function SHADING:get_frag_float_8()		return get_real_1( self.frag.float[8] )	end

	function SHADING:set_frag_int_1( v1 )				set_real_1( self.frag.int[1], v1 )	end
	function SHADING:set_frag_int_1_2( v1, v2 )			set_real_2( self.frag.int[1], v1, v2 )	end
	function SHADING:set_frag_int_1_3( v1, v2, v3 )		set_real_3( self.frag.int[1], v1, v2, v3 )	end
	function SHADING:set_frag_int_1_4( v1, v2, v3, v4 )	set_real_4( self.frag.int[1], v1, v2, v3, v4 )	end

	function SHADING:set_frag_int_2( v1 )				set_real_1( self.frag.int[2], v1 )	end
	function SHADING:set_frag_int_3( v1 )				set_real_1( self.frag.int[3], v1 )	end
	function SHADING:set_frag_int_4( v1 )				set_real_1( self.frag.int[4], v1 )	end
	
else	-- Apple Ios Case
	function SHADING:create( name, obj )
		local self = SHADING:create_instance( name )
		self.__loc = {}
		return self
	end
--IOS
	function SHADING:delete_program()
		if self.program then
			gol.delete_program( self.program )
			self.program = nil
		end
	end
--IOS
	function SHADING:create_program_from_str( str_vertex, str_fragment )
		self:delete_program()
		if str_vertex and str_fragment then
			self.b_geom = false		-- we should extend
			self.program, self.str_error = gol.create_program( str_vertex, str_fragment )
			if self.program then
				self:use_program()

				local loc = {}
				loc.vertex		= gol.get_attribute_location(	"aaa_vertex"	)
				loc.uv			= gol.get_attribute_location(	"aaa_uv"		)

				loc.color		= gol.get_uniform_location(		"aaa_color"		)
				loc.tex_dim 	= gol.get_uniform_location(		"aaa_tex_dim"	)
				loc.tex			= gol.get_uniform_location(		"aaa_tex2d"		)

				loc.vert_int	= gol.get_uniform_location(		"aaa_vu_int"	)
				loc.vert_float	= gol.get_uniform_location(		"aaa_vu_float"	)
				loc.vert_vec4	= gol.get_uniform_location(		"aaa_vu_vec4"	)

				if self.b_geom then
					loc.geom_int	= gol.get_uniform_location(		"aaa_gu_int"	)
					loc.geom_float	= gol.get_uniform_location(		"aaa_gu_float"	)
					loc.geom_vec4	= gol.get_uniform_location(		"aaa_gu_vec4"	)
				end

				loc.frag_int	= gol.get_uniform_location(		"aaa_fu_int"	)
				loc.frag_float	= gol.get_uniform_location(		"aaa_fu_float"	)
				loc.frag_vec4	= gol.get_uniform_location(		"aaa_fu_vec4"	)

				table.print( loc, "location", 2 )
				self.__loc = loc

				-- we do the texture uniforms because it will not change later
				gol.set_uniform_int(  loc.tex, 	0, 1, 2, 3 )
			else
				self:print( self.str_error )
			end
		end
		return self.program
	end
	function SHADING:create_program_from_file( fname_vertex, fname_fragment )
		local str_vertex 	= aaa.file.read_text(fname_vertex)
		local str_fragment	= aaa.file.read_text(fname_fragment)
		return self:create_program_from_str( str_vertex, str_fragment )
	end
--IOS
	function SHADING:use_program()
		if self.program then
			SHADING.cur = self
			gol.use_program( self.program )
		end
	end
--IOS
	function SHADING:is_valid()		return self.program ~= nil end
--IOS
	function SHADING:__set( name, id, val )
		self:get_table_always( name )[id] = val
	end
--IOS
	function SHADING:set_vert_int(		id, val )	self:__set( "vert_int", id, val )	end
	function SHADING:set_geom_int(		id, val )	self:__set( "geom_int", id, val )	end
	function SHADING:set_frag_int(		id, val )	self:__set( "frag_int", id, val )	end
--IOS
	function SHADING:set_vert_float(	id, val )	self:__set( "vert_float", id, val )	end
	function SHADING:set_geom_float(	id, val )	self:__set( "geom_float", id, val )	end
	function SHADING:set_frag_float(	id, val )	self:__set( "frag_float", id, val )	end
--IOS
	function SHADING:set_frag_float_1_2( v1, v2 )
		self:set_frag_float( 1, v1 )
		self:set_frag_float( 2, v2 )
	end
--IOS
	function SHADING:__set_vec4( name, id, x, y, z, w )
		local t = self:get_table_always( name )
		if not t[id] then
			t[id] = {}
		end
		t = t[id]
		if x then t[1] = x end
		if y then t[2] = y end
		if z then t[3] = z end
		if w then t[4] = w end
	end
--IOS
	function SHADING:set_vert_vec4(	shader_ref, ... )	self:__set_vec4( "vert_vec4", ... )	end
	function SHADING:set_geom_vec4(	shader_ref, ... )	self:__set_vec4( "geom_vec4", ... )	end
	function SHADING:set_frag_vec4(	shader_ref, ... )	self:__set_vec4( "frag_vec4", ... )	end
--IOS
	function SHADING:update_uniform_table_int( name )
		local loc = self.__loc[name]
		if loc >= 0 then
			local t = self:get_table( name )
			if t then gol.set_uniform_int( loc, unpack(t) ) end
		end
	end
	function SHADING:update_uniform_table_float( name )
		local loc = self.__loc[name]
		if loc >= 0 then
			local t = self:get_table( name )
			if t then gol.set_uniform_float( loc, unpack(t) ) end
		end
	end
	function SHADING:update_uniform_table_vec4( name )
		local loc = self.__loc[name]
		if loc >= 0 then
			local t = self:get_table( name )
			if t then gol.set_uniform_vec4( loc, t ) end
		end
	end
--IOS
	function SHADING:update_uniform_render()
		local loc = self.__loc
		gol.set_uniform_vec4( loc.color, 	{ gol.__color } )
		gol.set_uniform_int(  loc.tex_dim, 	gol.__texture_dim )

		self:update_uniform_table_int(		"vert_int" )
		self:update_uniform_table_float(	"vert_float" )
		self:update_uniform_table_vec4(		"vert_vec4" )

		if self.b_geom then
			self:update_uniform_table_int(		"geom_int" )
			self:update_uniform_table_float(	"geom_float" )
			self:update_uniform_table_vec4(		"geom_vec4" )
		end

		self:update_uniform_table_int(		"frag_int" )
		self:update_uniform_table_float(	"frag_float" )
		self:update_uniform_table_vec4(		"frag_vec4" )
	end
end

