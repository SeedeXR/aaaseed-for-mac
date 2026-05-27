if CLASS.DECLARE( "TEXT", nil, {ref = {}} ) then
	TEXT:set_class_status_doc(	CLASS.STATUS.CORE,
								"use a c Layer with bdd_text to display text" )

	local ref = TEXT.ref
	local pgr = param.get_ref
	local function pgr( ... )
		local r = param.get_ref(...)
		param.set_save( r, false )
		return r
	end
	local o = aaa.get_caller()
	o = aaa.obj.get_root( o )
	ref.layers = o
	aaa.obj.update_then_draw( ref.layers )	--make sure it is updated so we have a color object
	ref.text_layer			= aaa.obj.get_branch_by_name_symbo( o, "gabu_text_layer" )
		-- --	todo change name to avoid confusion
		-- ref.text_color			= aaa.layer.create_color( ref.text_layer, "text" )
	ref.text_model			=	aaa.layer.get_model( ref.text_layer )
		ref.text_su				= pgr( ref.text_model, "size_u" )
		ref.text_sv				= pgr( ref.text_model, "size_v" )
		ref.text_sf				= pgr( ref.text_model, "size_factor" )
	local bdd = aaa.obj.get_branch_by_name_symbo( ref.text_layer, "gabu_text_bdd" )
	ref.text_bdd = bdd
		ref.text_x				= pgr( bdd, "origin_x" )
		ref.text_y				= pgr( bdd, "origin_y" )
		ref.text_z				= pgr( bdd, "origin_z" )
		ref.text_str			= pgr( bdd, "text" )

	-- ref needed for TEXT using bdd_text
	ref.text_font_type			= pgr( bdd, "type" )
	ref.text_font 				= pgr( bdd, "font" )
	ref.text_id 				= pgr( bdd, "buffer_id" )
	ref.text_alignment 			= pgr( bdd, "alignment" )
	ref.text_alignment_vert		= pgr( bdd, "alignment_vertical" )
	ref.text_justification		= pgr( bdd, "text_justification" )
	ref.text_interline	 		= pgr( bdd, "interline" )
	ref.text_line_length 		= pgr( bdd, "line_length" )
	ref.text_draw_box	 		= pgr( bdd, "draw_bounding_box" )
	ref.text_face_size	 		= pgr( bdd, "face_size" )
	ref.text_face_dpi	 		= pgr( bdd, "face_dpi" )

	ref.shadow_draw	 			= pgr( bdd, "shadow_draw" )
	ref.shadow_offset	 		= {	pgr( bdd, "shadow_offset_u" ),
									pgr( bdd, "shadow_offset_v" ),
									pgr( bdd, "shadow_offset_axe" ) }

	ref.fit						= pgr( bdd, "fit" )
	ref.fit_size_u				= pgr( bdd, "fit_size_u" )
	ref.fit_size_v				= pgr( bdd, "fit_size_v" )


	ref.text_bbox_min_x	 		= pgr( bdd, "bounding_box_min_x" )
	ref.text_bbox_max_x	 		= pgr( bdd, "bounding_box_max_x" )
	ref.text_bbox_min_y	 		= pgr( bdd, "bounding_box_min_y" )
	ref.text_bbox_max_y	 		= pgr( bdd, "bounding_box_max_y" )
	ref.text_bbox_min_z	 		= pgr( bdd, "bounding_box_min_z" )
	ref.text_bbox_max_z	 		= pgr( bdd, "bounding_box_max_z" )

	ref.mapping				= aaa.layer.get_mapping( ref.text_layer )
		ref.tex_dim				= pgr( ref.mapping, "tex_use" )

	ref.use_shading			= pgr( ref.text_layer, "use_shading" )
	ref.shading				= aaa.layer.get_shading( ref.text_layer )
		--ref.shading_active		= pgr( ref.shading, "active" )
		--ref.shading_color_r		= pgr( ref.shading, "fu_01_r_x" )
		--ref.shading_color_g		= pgr( ref.shading, "fu_01_g_y" )
		--ref.shading_color_b		= pgr( ref.shading, "fu_01_b_z" )
		--ref.shading_color_a		= pgr( ref.shading, "fu_01_a_w" )

	TEXT.__t_font_type_str	=	{
							"MAA_LINE", "GLUT_LINE",
							"FONT_TEXTURE", "FONT_BUFFER",
							"FONT_OUTLINE", "FONT_POLYGON", "FONT_EXTRUDE",
							"FONT_BITMAP", "FONT_PIXMAP", "FONT_SDF"
							}

	TEXT.__font_type = 1	-- Line roman / derived from glut

	TEXT.last = {}
end

-- these are set starting at zero as the param
local FONT_TYPE_MAA_LINE = 0
local FONT_TYPE_GLUT_LINE = 1
local FONT_TYPE_TEXTURE = 2
local FONT_TYPE_BUFFER = 3
local FONT_TYPE_OUTLINE = 4
local FONT_TYPE_POLYGON = 5
local FONT_TYPE_EXTRUDE = 6
local FONT_TYPE_SDF = 9
local doc = TEXT.doc
--
-- SET/GET
--
--todofont this is heavy
function TEXT:set_font_type( str )
	if type(str)=="string" then
		local id
		if str == "font_nice" then
			id = 4
		else
			id = array.find_index_by_val( self.__t_font_type_str, string.upper(str) )
		end
		if id then
			self.__font_type = id - 1
		else
			self:print_error( "in TEXT:set_font_type() Can't set font type to unrecognized type: "..str )
		end
	else
		self:print_error( "in TEXT:set_font_type() font type should be a str not "..str )
	end
end
function TEXT:get_font_type()			return self.__t_font_type_str[self.__font_type+1]	end

function TEXT:set_face_size( val )		self.face_size = val	self.b_need_bdd_param_update = true	end
function TEXT:get_face_size()			return self.face_size	end

function TEXT:set_face_dpi( val )		self.face_dpi = val		self.b_need_bdd_param_update = true	 end
function TEXT:get_face_dpi()			return self.face_dpi	end

function TEXT:set_font( val )			self.font_index = clamp( val, 0, 64 )	self.b_need_bdd_param_update = true	end
function TEXT:get_font()				return self.font_index					end

function TEXT:get_justification()		return self.justification		end
function TEXT:set_justification( val )
-- values are Left, Center, Right, Justify	
	if val ~= "center" and val ~= "justify" and val ~= "right" then
		self.justification = "left"			-- default value
	else
		self.justification = val
	end
end

function TEXT:get_alignment()			return self.alignment			end
function TEXT:set_alignment( val )
-- values are Left, Center, Right, Justify
	self.alignment = val
	if val ~= "center" and val ~= "right" and val ~= "justify" then
		self.alignment = "left"		-- default value
	end
end

function TEXT:get_alignment_vert()		return self.alignment_vert		end
function TEXT:set_alignment_vert( val )
-- values are line_first, bottom, middle, top
	if val ~= "bottom" and val ~= "middle" and val ~= "top" then
		self.alignment_vert = "line_first"			-- default value
	else
		self.alignment_vert = val
	end
end

function TEXT:get_interligne()			return self.interligne		end
function TEXT:set_interligne( val )		self.interligne = val		self.b_need_bdd_param_update = true	end

function TEXT:get_line_length()			return self.line_length		end
function TEXT:set_line_length( val )	self.line_length = val		self.b_need_bdd_param_update = true	end

function TEXT:get_draw_box()			return self.b_draw_bbox		end
function TEXT:set_draw_box( b )			self.b_draw_bbox = b		self.b_need_bdd_param_update = true	end

function TEXT:get_width()
	local ref = self.ref
	local pg = param.get
	return pg( ref.text_bbox_max_x)	- pg( ref.text_bbox_min_x )
end
function TEXT:get_height()
	local ref = self.ref
	local pg = param.get
	return pg( ref.text_bbox_max_y)	- pg( ref.text_bbox_min_y )
end

-- function TEXT.set_font_low( index )
-- 	local ref = TEXT.ref
-- 	param.set( ref.text_font, 	index )
-- end
-- function TEXT.set_color_low( r,g,b, a )
-- 	local ref = TEXT.ref
-- 	ref.text_color:set_rgba( r,g,b, a )
-- end

function TEXT:set_color_sdf( r,g,b, a )	self.color = { r,g,b, a }		end
function TEXT:set_color( r,g,b, a )		self:set_color_sdf( r,g,b, a )	end

if false then
	doc.__fn_draw_str_xy_sxy = { "( str, x,y, sx,sy ) draw the str according to the coor specified and the state of the TEXT object.",
								"this a function not a methad" }
	function TEXT.__fn_draw_str_xy_sxy( str, x,y, sx,sy )
		local ref = TEXT.ref
		local pset = param.set
		local l = TEXT.last
		if l.x ~= x then
			pset( ref.text_x, x )
			l.x = x
		end
		if l.y ~= y then
			pset( ref.text_y, y )
			l.y = y
		end
		if l.sx ~= sx then
			pset( ref.text_su, sx )
			l.sx = sx
		end
		if l.sy ~= sy then
			pset( ref.text_sv, sy )
			l.sy = sy
		end	

		param.set( ref.text_str, str )
		aaa.obj.update_then_draw( ref.text_layer )
	end
end

function TEXT:update_bdd_param()
	local ref = TEXT.ref
	local pset = param.set
	local last = TEXT.last

	local font_type = self.__font_type
	--if last.font_type ~= font_type then
		pset( ref.text_font_type, font_type )
		--last.font_type = font_type
	--end
	-- ftgl stuff
	--self:print( self.font_index 
		--if self.font_index  ~= 0 then
	--	self:print( "self.font_index "..self.font_index )
	--end
	local v = self.font_index
	if last.font_index ~= v then
		pset( ref.text_font, v )
		last.font_index = v
	end

	v = self.alignment
	if last.alignment ~= v then
		pset(	ref.text_alignment,			v )
		last.alignment = v
	end
	v = self.alignment_vert
	if last.alignment_vert ~= v then
		pset(	ref.text_alignment_vert, 	v	)
		last.alignment_vert = v
	end
	v = self.justification
	if last.justification ~= v then
		pset(	ref.text_justification, 	v	)
		last.justification = v
	end
	v = self.interligne
	if last.interligne ~= v then
		pset(	ref.text_interline, 		v	)
		last.interligne = v
	end
	v = self.line_length
	if last.line_length ~= v then
		pset(	ref.text_line_length, 		v	)
		last.line_length = v
	end
	v = self.b_draw_bbox
	if last.b_draw_bbox ~= v then
		pset(	ref.text_draw_box, 			v	)
		last.b_draw_bbox = v
	end
	v = self.face_size
	if last.face_size ~= v then
		pset(	ref.text_face_size, 		v	)
		last.face_size = v
	end
	v = self.face_dpi
	if last.face_dpi ~= v then
		pset(	ref.text_face_dpi, 			v	)
		last.face_dpi = v
	end
end

function TEXT:set_next_shadow()
	self.b_shadow = true
end
function TEXT:set_next_fit( su,sv )
	self.b_fit = true
	self.fit_size_u = su
	self.fit_size_v = sv
end
-- hack for GA_HELP
function TEXT:draw( 			id,			text,	x,y,   sx,sy )
	self:__draw_for_id(			id,			text,	x,y,0, sx,sy, "left", "left" )
end
function TEXT:__draw_for_bu(	bu,			text,	x,y,z, sx,sy, justification, align_x )
	self:__draw_for_id( 		bu.__bu_id, text,	x,y,z, sx,sy, justification, align_x )
end
function TEXT:__draw_line( 		text,	x,y,z, sx,sy )
--	if not self:is_active() then return end
--	if true then return end
--	if not text or text == "" then return end

	local b_maa = BU.__s_text_type == 2
	local draw_str
	if b_maa then
		--todofont similarity (in particular base line) should be addressed in V
		sy = sy * 1.2
		y = y - sy * .3
		draw_str = aaa.draw_str_maa_xyz
	else
		draw_str = aaa.draw_str_xyz
	end

	if self.b_shadow then
		self.b_shadow = false
		local du,dv
		du = self.__shadow_du
		if du then
			dv = self.__shadow_dv
			self.__shadow_du = nil
			self.__shadow_dv = nil
		else
			local d = sy * .06	 -- GA_HELP value infact should make it better
			du,dv = d,-d
		end
		local r,g,b, a = gol.get_color()
			gol.color_black()
			draw_str( text, x+du,y+dv,z, sx,sy )
		gol.color( r,g,b, a )
	end
	
	draw_str( text, x,y,z, sx,sy )
	--self:print( "we should not pas here" )
end

function TEXT:__draw_for_id( 	id,			text,	x,y,z, sx,sy, justification, align_x )
	--sx = sx * 2
	if not self:is_active() then return end
	--if true then return end
	if not text or text == "" then return end

	if justification then
		self.justification = justification
	end
	if align_x then
		self.alignment = align_x
	end
	--self.justification = "left"
	--self.alignment = "left"
	--text = "TEXT TO TEST"
	--self:print( "TEXT "..id..", draw() "..text )
	--self:print( self.font_type)
	--self:print( "self.buffer_index".. self.buffer_index )				
	--local font_type = FONT_TYPE_TEXTURE
	--self.justification = "center"
	--self.alignment = "center"
	local font_type = self.__font_type

	--aaa.print_fn( )
	if font_type >= FONT_TYPE_TEXTURE then
		local pset = param.set
		local ref = TEXT.ref
		local last = TEXT.last
		--aaa.print_fn()
		--if self.b_need_bdd_param_update then
--
--	BEGIN SET
--
			local v = self.font_index
			if last.font_index ~= v then
				pset( ref.text_font, v )
				last.font_index = v
			end

			v = self.alignment
			if last.alignment ~= v then
				pset(	ref.text_alignment,			v )
				last.alignment = v
			end
			v = self.alignment_vert
			if last.alignment_vert ~= v then
				pset(	ref.text_alignment_vert, 	v	)
				last.alignment_vert = v
			end
			v = self.justification
			if last.justification ~= v then
				pset(	ref.text_justification, 	v	)
				last.justification = v
			end
			v = self.interligne
			if last.interligne ~= v then
				pset(	ref.text_interline, 		v	)
				last.interligne = v
			end
			v = self.line_length
			if last.line_length ~= v then
				pset(	ref.text_line_length, 		v	)
				last.line_length = v
			end
			v = self.b_draw_bbox
			if last.b_draw_bbox ~= v then
				pset(	ref.text_draw_box, 			v	)
				last.b_draw_bbox = v
			end
			v = self.face_size
			if last.face_size ~= v then
				pset(	ref.text_face_size, 		v	)
				last.face_size = v
			end
			v = self.face_dpi
			if last.face_dpi ~= v then
				pset(	ref.text_face_dpi, 			v	)
				last.face_dpi = v
			end
--
--	END SET
--
		--end
		--gol.set_default()
		local dim_pushed = gol.get_texture_dim()

			--gol.bind_texture(36)
			if font_type <= FONT_TYPE_BUFFER then
				gol.set_texture_dim( 2 )
				gol.set_blend_add( 2 )
			else
				gol.set_texture_dim( 0 )
			end
			--gol.enable_blend()	done by enable_blend

			if last.font_type ~= font_type then
				pset( ref.text_font_type, font_type )
				last.font_type = font_type
			
				--self:print( self.font_type )
				if font_type == FONT_TYPE_SDF then
					pset( ref.use_shading, "once" )
					--pset( ref.shading_active, 1 )
		--			pset( ref.tex_dim, 2 )
					pset( ref.text_id, id )	-- todofont explain text_id
					y = y + sy / 3
					sy = sy * 1.4
					sx = sx * 1.2
				else
					pset( ref.use_shading, "none" )
					--pset( ref.shading_active, 0 )
		--			pset( ref.tex_dim, 0	 )
					pset( ref.text_id, 0 )
				end
			end

			if last.sx ~= sx then
				pset( ref.text_su, sx )
				last.sx = sx
			end
			if last.sy ~= sy then
				pset( ref.text_sv, sy )
				last.sy = sy
			end
			pset( ref.text_str, text )

			if last.x ~= x then
				pset( ref.text_x, x )
				last.x = x
			end
			if last.y ~= y then
				pset( ref.text_y, y )
				last.y = y
			end
			if last.z ~= z then
				pset( ref.text_z, z )
				last.z = z
			end

			local b_shadow = self.b_shadow
			if b_shadow then
				if not last.b_shadow then
					pset( ref.shadow_draw, true )
					last.b_shadow = true
				end
				self.b_shadow = false	--reset for next

-- SHADOW
				local du,dv
				du = self.__shadow_du
				if du then
					dv = self.__shadow_dv
					self.__shadow_du = nil
					self.__shadow_dv = nil
				else
					du,dv = 1,-1
				end

				if last.shadow_offset_u ~= du then
					pset( ref.shadow_offset[1], du )
					last.shadow_offset_u = du
				end
				if last.shadow_offset_v ~= dv then
					pset( ref.shadow_offset[2], dv )
					last.shadow_offset_v = dv
				end
			else
				if last.b_shadow then
					pset( ref.shadow_draw, false )
					last.b_shadow = false
				end
			end

-- FIT
			local b_fit = self.b_fit
			if b_fit then
				-- if text == "Flatland" then
				-- 	--aaa.debug.print_traceback()
				-- 	self:print( "Hit "..self.b_shadow )
				-- end
				if not last.b_fit then
					pset( ref.fit, true )
					last.b_fit = true
				end
				self.b_fit = false	--reset for next

			-- prepare fit
				local du = self.fit_size_u
				du = du or 100000.
				if last.fit_size_u ~= du then
					pset( ref.fit_size_u, du )
					last.fit_size_u = du
				end
				local dv = self.fit_size_v
				dv = dv or 100000.
				if last.fit_size_v ~= dv then
				 	pset( ref.fit_size_v, dv )
				 	last.fit_size_v = dv
				end
			else
				if last.b_fit then
					pset( ref.fit, false )
					last.b_fit = false
				end
			end

			--	aaa.layer.update_draw_shader_bdd( ref.text_layer )
			aaa.obj.update_then_draw( ref.text_layer )

			-- pset( ref.use_shading, "owner" )
			-- gol.use_program( 0 )

		if dim_pushed then
			gol.set_texture_dim( dim_pushed )
		end
	else
		local f = 0.66666666
		self:__draw_line(	text,	x,y,z,	sx*f,sy*f )
	end
end

function TEXT:set_font_texture( font )
	self.__font_type = FONT_TYPE_TEXTURE -- now the faster
	self.b_need_bdd_param_update = true
	self:set_font( font )		-- arial
	--self:set_face_size( 72 )
	--self:set_face_dpi( 72 )
--	self:set_alignment( "left" )
--	self:set_alignment_vert( "line_first" )
--	self:set_interligne( 1. )
	--self:set_line_length( 200 )
end

function TEXT:set_font_sdf( font )
	self.__font_type = FONT_TYPE_SDF
	self.b_need_bdd_param_update = true
	self:set_font( font )	 
	--self:set_face_size( 144 )
	--self:set_face_dpi( 36 )
--	self:set_alignment( "left" )
--	self:set_alignment_vert( "line_first" )
--	self:set_justification( "left" )
--	self:set_interligne( 1. )
--	self:print( "self.__font_type "..self.__font_type )
--	self:print( "font "..font )
	--self:set_line_length( 200 )
end

function TEXT:create( name )
	local self = TEXT:create_instance( name )

	self.font_index		= 0
	self.alignment		= "left"		-- values are left, center, right, justify
	self.alignment_vert	= "line_first"	-- values are line_first, bottom, middle, top
	self.justification 	= "left"		-- values are left, center, justify, right
	self.interligne		= 1.
	self.line_length	= 320
	self.b_draw_bbox	= false
	self.face_dpi		= 72
	self.face_size		= 96
--	self.color = { 1, 1, 1, 1 }

	self.b_need_bdd_param_update = true
	return self
end


