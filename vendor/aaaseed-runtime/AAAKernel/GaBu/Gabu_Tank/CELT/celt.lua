--
--	CELT	infact this is a Creature ELemenT
--			body and eye for example
--
if CLASS.DECLARE( "CELT" ) then
--	CELT.__shader_vert_def = 151
--	CELT.__shader_frag_def = 150

	-- by default Creature ELemenT use texture
	function CELT:set_texture_use( b_use )	self.__b_texture_use = b_use	end
	function CELT:is_texture_use()			return self.__b_texture_use		end
	CELT:set_texture_use( true )
end

--
--	BIND / TEXTURE
--
local NB_BIND = 4
function CELT:add_bind_2d( id, obj )
	local ref = self.ref
	ref.map[id] = {}
	ref.map[id].bind_2d = param.get_ref( obj, "bind_2d" )
	ref.map[id].bank_2d = param.get_ref( obj, "bank_2d" )
end
function CELT:set_bind_2d( bind, id )
	--aaa.print_method()
	local map = self.ref.map[id]
	local bank
	bank, bind = aaa.img.make_bank_bind_2d( bind )
	--self:print( bank )
	--self:print( r )
	param.set( map.bank_2d, bank )
	param.set( map.bind_2d, bind )
end


--
--	REF
--
--function CELT:__build_ref( layers )
--end
function CELT:set_fog( v )
	for i = 1,5 do --hack
		if self.ref.layer[i] then
			param.set( self.ref.layer[i], "use_fog", v )
		end
	end
end
function CELT:set_fog_current()
	self:set_fog( 0 )
end

function CELT:__assign( layers )
-- do the job when using bdd_tri, not called for fbx
-- do the job when using bdd_tri, not called for fbx
	local ref = self.ref

	ref.layers	=	layers
	ref.layer = {}
	ref.map = {}
	ref.active	=	param.get_ref( layers, "active" )

	--self:print( aaa.get_filename() )
	ref.layer[1]	=	aaa.layers.get_layer( ref.layers, 1 )
		self:add_bind_2d( 1, ref.layer[1] )
		ref.render		=	aaa.layer.get_rendering( ref.layer[1] )
			ref.light		=	param.get_ref( ref.render, "light" )
		ref.mapping		=	aaa.layer.get_mapping( ref.layer[1] )
		ref.tex_dim = {}
			ref.tex_dim[1] = param.get_ref( ref.mapping, "tex_use" )
		ref.texturing	=	aaa.obj.get_branch_by_class( ref.layer[1], "texturing" )
		local filename = aaa.obj.get_name( ref.texturing )
		local tab	=	aaa.obj.get_branchs_by_class( ref.texturing, "texture_unit" )
		if tab then
			local nb = #tab
			--self:print( "found "..nb.." texture_unit" )
			for i = 1,nb do
				local unit = tab[i]
				ref.tex_dim[i+1] = param.get_ref( aaa.obj.get_branch_by_class( tab[i], "mapping" ), "tex_use" )
				--self:print( aaa.obj.get_filename( unit ) )
				self:add_bind_2d( i+1, unit )
			end
		end
	self:print( "layer A done :"..aaa.obj.get_name( ref.layer[1] ) )

	ref.layer[2]	=	aaa.layers.get_layer( ref.layers, 2 )
		self.__shading	= aaa.layer.create_shading( ref.layer[2],  "layer_2" )
		ref.layer_2_shader_bdd_only = param.get_ref( ref.layer[2], "shader_bdd_only" )
		self:add_bind_2d( 5, ref.layer[2] )
	self:print( "layer B done :"..aaa.obj.get_name( ref.layer[2] ) )

	ref.layer[3]	=	aaa.layers.get_layer( ref.layers, 3 )
		--todo encapsulate bdd_tri
		--todo encapsulate bdd_tri
		ref.bdd_used			=	aaa.obj.get_branch_by_class_no_error( ref.layer[3], "bdd_tri" )
		if ref.bdd_used then
			ref.model_filename	=	param.get_ref( ref.bdd_used, "model_filename" )
			ref.bbox_min = {}
				ref.bbox_min[1]		=	param.get_ref( ref.bdd_used, "bounding_box_min_x" )
				ref.bbox_min[2]		=	param.get_ref( ref.bdd_used, "bounding_box_min_y" )
				ref.bbox_min[3]		=	param.get_ref( ref.bdd_used, "bounding_box_min_z" )
			ref.bbox_max = {}
				ref.bbox_max[1]		=	param.get_ref( ref.bdd_used, "bounding_box_max_x" )
				ref.bbox_max[2]		=	param.get_ref( ref.bdd_used, "bounding_box_max_y" )
				ref.bbox_max[3]		=	param.get_ref( ref.bdd_used, "bounding_box_max_z" )
			param.set( ref.layer[2], "use_bdd", "update_and_draw" )
		else
			self.b_v2021 = true --todo 2021 Oct we do the new path in a tmps way for now : no fbx no bdd_tri
			--self:box_debug( "We deceide generic here" )
			self:print_debug( "no bdd_tri in layer C use current bdd" )
			ref.bdd_used = aaa.layer.get_bdd( ref.layer[3] )
		end
	self:print( "layer C done :"..aaa.obj.get_name( ref.layer[3] ) )
--[[
	self:add_bind_2d( 1, ref.layer[3] )
	local texturing = aaa.obj.get_branch_by_class( ref.layer[1], "texturing" )
	local filename = aaa.obj.get_name( texturing )
	local tab = aaa.obj.get_branchs_by_class( texturing, "texture_unit" )
	if tab then
		local nb = #tab
		self:print( "found "..nb.." texture_unit" )
		for i = 1,nb do
			local unit = tab[i]
			--self:print( aaa.obj.get_filename( unit ) )
			self:add_bind_2d( i+1, unit )
		end
	end
--]]
	aaa.layer.set_bdd_external( ref.layer[2], ref.bdd_used )

	local ref_layer = ref.layer[1]
	if not self.b_v2021 then
		param.set( ref_layer, "use_model",		"CURRENT"	)
		param.set( ref_layer, "use_color",		"CURRENT"	)
		self:set_fog_current()
		--	param.set( ref_layer, "use_rendering",	"CURRENT"	)
		param.set( ref_layer, "use_shading",	"CURRENT"	)
	--	param.set( ref_layer, "use_fog",		"CURRENT"	)
	end

	--	self:attach_curve( "fish_curve" )


	--todo move to a call for grea on every frame ?
	--	set to 0 to deactivate target morph
	--todomona do this init too for fbx ?
	for i=1,6 do
		self.__shading:set_vert_vec4_w( i, 0. )
	end

	self.__b_valid = true

	return self
end

--
-- DIR FILENAME
--
function CELT:set_dir( dir )	self.__dir_celt = dir	end
function CELT:get_dir()			return self.__dir_celt	end

function CELT:build_filename( name )
	return self:get_dir()..name.."/Celt_"..self.letter
end

--
-- SHADER
--
function CELT:get_shading()
	local sha = self.__shading
	if not sha then
		aaa.debug.print_traceback()
		self:print_error( "no shading" )
	end
	return sha
end
function CELT:apply_to_shader( method, ... )
	local sha = self:get_shading()
	if sha then
		sha[method]( sha, ... )
	end
end
function CELT:reload_shader()
	SHADING.trig_reload_include()
	local sha = self:get_shading()
	if sha then	sha:trig_reload()
	else		self:print_error( "reload_shader() no shading" )
	end
end

function CELT:create( name, i )
	local	letter	= string.char( i + 64 )
	local	self = CELT:create_instance( name.."_"..letter )
	self.letter = letter
	self.letter_lowercase = string.char( i + 64 + 32 )
	self.celt_index = i
	self.name_pure = name
	self.ref = {}
	self.bind = {}
	self:set_active( true )
	return self
end

function CELT:draw_attr( )
-- do the job when using bdd_tri, not called for fbx
	aaa.obj.update_then_draw( self.ref.layer[1] )
end

function CELT:process_def( def, b_fbx, meu_fish, tail_vert )

	self:print( "\tprocess_def() Begin" )

	--self:print_inverse( "process_def()" )
	local ref = self.ref

	self.b_do_anim = not def.b_skip_anim
	--todo merge the branches
	if meu_fish then
		self.meu_fish = meu_fish
		self.b_v2021 = true
		self.__b_valid = true
		self.__b_ui_no = true
		--self:box_error( "CELT:process_def() : "..meu_fish_name.." "..self.b_v2021  )
	elseif b_fbx then
		local fbx = { meu_name = def.meu_name or def.model }
		--self:box_debug( "process_def() meu_name is "..fbx.meu_name )

		local meu_shader = app:get_meu_by_name_cached( "ShaSkinBoid_1" )

		--self:print( "process_def() meu_shader is "..meu_shader )
		fbx.layer_shading		= meu_shader:get_layer_shading()
		fbx.ref_materiel_front	= meu_shader:get_ref_material_front()
		self.material_front 	= def.material_front or 40
		self.__shading			= meu_shader:get_shading()

		local meu_name = "fbx_"..fbx.meu_name
		local meu_fbx = app:get_meu_by_name_cached( meu_name )
		if meu_fbx then
			if meu_fbx:assign_fbx_from_def( def ) then
				fbx.meu_fbx		= meu_fbx
				fbx.bdd_fbx		= meu_fbx:get_bdd_fbx()
				fbx.obj_ref		= fbx.bdd_fbx:get_obj()
				fbx.layer_fbx	= meu_fbx:get_layer_fbx()
				self.fbx		= fbx

				self.bbox_min = fbx.bdd_fbx:get_param( "mesh_bbox_min_x" )
				self.bbox_max = fbx.bdd_fbx:get_param( "mesh_bbox_max_x" )
				--hack monaco PELAGIA
				if (self.bbox_max - self.bbox_min) < 0.01 then
					self.bbox_min = fbx.bdd_fbx:get_param( "mesh_bbox_min_y" )
					self.bbox_max = fbx.bdd_fbx:get_param( "mesh_bbox_max_y" )
				end

				self.anim_time_begin	= fbx.bdd_fbx:get_param( "time_begin" )
				self.anim_time_end		= fbx.bdd_fbx:get_param( "time_end" )
				self.anim_duration		= self.anim_time_end - self.anim_time_begin
				self.anim_duration_over	= 1. / self.anim_duration
			end
			self.__b_valid = true
		else
			self:box_debug( "CELT:process_def() :  No MEU named "..meu_name )
		end
	elseif def.meu_name then
		self:box_debug( "CELT:process_def() :  No MEU name is "..def.meu_name )
	else
		--assign shader from def
		local	shading = self.__shading
		if shading then
			local	vertex = def.vertex and (128 + def.vertex - 1) or CELT.__shader_vert_def
			shading:set_vert_bind( vertex )
			local	pixel  = def.pixel  and (128 + def.pixel  - 1) or CELT.__shader_frag_def
			shading:set_frag_bind( pixel )

			if def.model then
				local name = MEDIA.get_dir()..def.model
				self:print( "set model to ", name )
				param.set( ref.model_filename, name )
				aaa.obj.update( ref.bdd_used )
				self.bbox_min = param.get( ref.bbox_min[1] )
				self.bbox_max = param.get( ref.bbox_max[1] )
			end

			--shading:set_vert_float_8( tail.begin )	--Loyal now depend on each fish
			shading:set_vert_int_4( tail_vert )
		else
			self:print_error( "CELT:process_def() : No shading here (non fbx case)" )
		end
	end

	--2021 Nov Mâa move it after previous block so we could use whay's done before like Type (original,fbx,meu_fish)
	if self:is_texture_use() then
		local maps = def.maps
		if maps then
			--	todo refine
			local bind
			for i=1,NB_BIND do
				if i==1 then
					param.set( ref.layer_2_shader_bdd_only, 0 )
				end
				local name = maps[i]
				if name then
					if type(name) == "table" then
						if not self.seq then self.seq = {} end
						self.seq[i] = IMG_SEQ:create_from_array( "seq"..i, name )
						bind = -2
						if i==1 then
							param.set( ref.layer_2_shader_bdd_only, 0 )
						end
					elseif name == "caustic" then
						bind = -1	--todo refine with a seq nb
					else
						bind = IMGS.get_bind_sxy_with_error_dialog( name )
					end
				else
					bind = nil
				end
				self.bind[i] = bind
				--if not bind then bind = 64 end	--refine this should be an empty texture for now
				if bind and bind > 0 then
					self:set_bind_2d( bind, i )
					--todo this go around a bug
					if i==1 then
						self:set_bind_2d( bind, 5 )
					end
				end
			end
		end
	end

	self:print( "\tprocess_def() End" )
end

function CELT:pick_touch_map()
	local seqs = self.seq
	if seqs then
		local seq = seqs[4]
		if seq then
			return seq:get_bind_rnd( aaa.time.t * 42 )
		end
	end
end




function CELT.begin_draw()
--	aaa.print_inverse( "CELT.begin_draw" )
	CELT.draw_last = nil	--- to be sure first in the loop have a "cache miss"
end

function CELT:draw_transparent( crea )
	if not self:is_active() then return end
	--self:print( "draw_transparent" )

	--if GA.b_spy then aaa.spy.push_range( "CELT:draw_deformed", 2 ) end

		--local shading = self.__shading
		-- here we set the 6 points for the catmullrom curve

	--shading:set_vert_vec3_6_table18( crea.pos )


	local ref_layer
		--	1 2 3 for deformation on curve
		--	4 size factor ( < 1. )
		--	5 phase used for animation
		--							   				NO,				SEG_LEN_OVER,		NO, 				3-size_factor,		4-tail.angle, 	5-tail.amp,	6,		7-tail.begin)
		--shading:set_vert_float_1_8(	crea.seg_len,	crea.seg_len_over,	crea.s_inter, 		crea.size_factor,	tail.phase,		tail.amp,	nil,	tail.begin )

end

function CELT:draw_deformed( crea )

	if not self:is_active() then return end
--	if not self.__b_valid then return end

	--self:print( "draw_deformed "..crea )
	local shading = self.__shading
	if not shading then
		crea:print_error( "CELT:draw_deformed() No shading" )
		return
	end
	
	if GA.b_spy then aaa.spy.push_range( "CELT:draw_deformed", 2 ) end

		-- here we set the 6 points for the catmullrom curve
		if GA.b_spy then aaa.spy.push_range( "set_vert_vec3_6_table18", 3 ) end
			shading:set_vert_vec3_6_table18( crea.pos )
		if GA.b_spy then aaa.spy.pop_range() end

		local fbx = self.fbx
		local b_tail
		local ref_layer = self.ref.layer
		local meu_fish
		if self.b_v2021 then
			b_tail = true
			meu_fish = self.meu_fish
			if meu_fish then
				if CELT.draw_last ~= crea.__grea then
					if GA.b_spy then aaa.spy.push_range( "MEU Fish Attr", 3 ) end
						meu_fish:draw_fish_attr( crea )
						CELT.draw_last = crea.__grea
					if GA.b_spy then aaa.spy.pop_range() end
				end
			else
				--	attributes
				if CELT.draw_last ~= self then
					--crea:print( "draw_attribute" )
					if GA.b_spy then aaa.spy.push_range( "Attr", 1 ) end
						aaa.obj.update_then_draw( ref_layer[1] )
						aaa.obj.update_then_draw( ref_layer[2] )
						aaa.obj.update( ref_layer[3] )

						CELT.draw_last = self
					if GA.b_spy then aaa.spy.pop_range() end
				end
			end
		elseif fbx then
		else
			b_tail = true
			ref_layer = self.ref.layer
			local seqs = self.seq
			if seqs then
				local seq = seqs[1]
				if seq then
					self:set_bind_2d( seq:get_bind_rnd( crea.id_in_pool ), 5 )
				end
				if crea.__touch_map then
					self:set_bind_2d( crea.__touch_map, 4 )
				--todo avoid to do this every time
				--self:set_bind_2d( self.bind[1], 5 )
				end
			end

			--	attributes
			if CELT.draw_last ~= self or crea.__touch_map then
				--crea:print( "draw_attribute" )
				if GA.b_spy then aaa.spy.push_range( "Attr", 1 ) end
					aaa.obj.update_then_draw( ref_layer[1] )
					CELT.draw_last = self
				if GA.b_spy then aaa.spy.pop_range() end
			end

			-- if self.s_reload then
			-- 	local l = self.s_reload
			-- 	l = l - 1
			-- 	if l <= 0 then
			-- 		self:print( "reload shader" )
			-- 		self.shading:trig_reload()
			-- 		self.s_reload = nil
			-- 	else
			-- 		self.s_reload = l
			-- 	end
			-- end
		end

		--	1 2 3 for deformation on curve
		--	4 size factor ( < 1. )
		--	5 phase used for animation
		--							   			NO,				SEG_LEN_OVER,		NO, 				3-size_factor,		4-tail.angle, 	5-tail.amp,	6,		7-tail.begin)
		if b_tail then
			local tail = crea.tail
			--self:print( tail.phase )
			if TANK.cur.b_machine_mono then
				if self.b_v2021 then
					shading:set_vert_float_1_8(		crea.seg_len,	crea.seg_len_over,	crea.s_inter, 		crea.size_factor,	tail.phase,		tail.amp )
				else
				--	shading:set_vert_float_1_8( 	nil,			crea.seg_len_over,	nil, 				crea.size_factor,	tail.phase,		tail.amp,	nil,	tail.begin )
					shading:set_vert_float_1_8(		crea.seg_len,	crea.seg_len_over,	crea.s_inter, 		crea.size_factor,	tail.phase,		tail.amp,	nil,	tail.begin )
				end
			else
				shading:set_vert_float_1_6(   	crea.seg_len,	crea.seg_len_over,  crea.s_inter,		crea.size_factor,	tail.phase,		tail.amp )
			end
		else
			shading:set_vert_float_1_4( 	 	crea.seg_len,	crea.seg_len_over,	crea.s_inter, 		crea.size_factor	)
		end

		if GA.b_spy then aaa.spy.push_range( "CELT Draw", 2 ) end

			if self.b_v2021 then
				if meu_fish then
					meu_fish:draw_fish(  crea )
				else
					if true then
						gol.update_uniform_vertex()
						--gol.update_uniform_vertex_vec4()
						--gol.update_uniform_vertex_float()
						aaa.obj.draw( self.ref.bdd_used )
					else
						--	param.set( self.ref.layer_2_shader_bdd_only, 0 )
						--aaa.obj.update_then_draw( ref_layer[2] )
						--gol.update_uniform_vertex_vec4()
						--gol.update_uniform_vertex()
						aaa.obj.update_then_draw( ref_layer[3] )
						--aaa.obj.draw( ref_layer[3] )
					end
				end
			elseif fbx then
				--self:print( "go" )
				if CELT.draw_last ~= fbx.meu_fbx then
					--self:print( "CELT:draw_deformed do attrb" )
					-- we draw the first of a flock of fish here
					--  so we have to set all we need for it
					-- we don't anim in the shadow pass
					self.__b_do_anim = self.b_do_anim and ( ga:get_pass_info().name~="shadow" )
					fbx.bdd_fbx:set_time_update( self.__b_do_anim )

					--self:print( "attr" )
					--todomona call direct
					-- we call the shader here
					param.set( fbx.ref_materiel_front, self.material_front )
					aaa.obj.update_then_draw( fbx.layer_shading )

					fbx.meu_fbx:draw_attr()

					if self.__b_do_anim then
						fbx.bdd_fbx:set_time( wrap_01( crea.len_moved * crea.len_to_anim_factor * self.anim_duration_over ) * self.anim_duration )
					end
					--meu_fbx:draw_fbx_min()
					aaa.obj.update_then_draw( fbx.layer_fbx )

					CELT.draw_last = fbx.meu_fbx
				else
					--meu_fbx:draw_fbx_min()
					if true then --self.__b_do_anim then
						--self:print( "CELT:draw_deformed anim" )
						fbx.bdd_fbx:set_time( wrap_01(crea.len_moved * crea.len_to_anim_factor * self.anim_duration_over ) * self.anim_duration )

						--todomona can we can do better here ?
						-- this update the shader uniform too
						--meu_fbx:draw_fbx_min()
						aaa.obj.update_then_draw( fbx.layer_fbx	)
						--aaa.obj.update( fbx.bdd_fbx )
						--aaa.obj.draw( fbx.bdd_fbx )
					else
						--self:print( "CELT:draw_deformed" )
						--we don't anim so we draw at minimnum
						--aaa.layer.update_draw_shader_bdd( layer.obj )
						--	tested it function

						--gol.update_uniform_render()
						gol.update_uniform_vertex_vec4()

						--aaa.obj.draw( layer.obj	)
						--meu_fbx:draw()
						aaa.obj.draw( fbx.obj_ref )
					end
					--self:print( "cached" )
					--gol.update_uniform_render()
				end

			else	-- non fbx case

				--aaa.show( crea.tail.angle, "angle" )
			--	param.set( self.ref.layer_2_shader_bdd_only, 0 )
					aaa.obj.update_then_draw( ref_layer[2] )
				--	bdd
				--	aaa.obj.update_then_draw( ref_layer[3] )
			--	gol.flush()
			--[[
				local hack = self.__hack
				if not hack then hack = 0 end
				if hack == 127 then
					self.__hack = 0
					gol.flush()
				else
					self.__hack =  hack + 1
				end
			--]]
			end

		if GA.b_spy then aaa.spy.pop_range() end

	if GA.b_spy then aaa.spy.pop_range() end
end

function CELT:draw_at( node_id, s, ds )
	if not self:is_active() then return end

	self:draw_attr()
	local curve = self.ref.curve
	local shading = self.__shading
	--gol.set_default()
	for i=1,6 do
		local x,y,z = aaa.bdd.get_point_s( curve, node_id, s )
		s = s + ds
		--self:print( i.." x "..x.." y "..y.." z "..z )
		shading:set_vert_vec4( i, x,y,z, 1 )
		--aaa.draw_null( x, y, z, .1 )
	end
	aaa.obj.update_then_draw( self.ref.layer[3] )
end

function CELT:update()
	--aaa.print_method()
	--self:print( "CELT:update() ")
	if self.b_v2021 then
		local meu_fish = self.meu_fish
		if meu_fish then
			self.__shading = meu_fish:update_celt()
		end
	elseif self.fbx then
	else
		self:set_active( param.get_bool( self.ref.active ) )
		--self:print( "update()" )
		local tank = TANK.cur
		local seq_bind = tank.caustic and tank.b_caustic_play and tank.caustic:get_bind()
		--self:print( "update() : ".. tank.b_caustic_play.." "..seq_bind )
		if seq_bind then
			local bind = self.bind
			local bu_tex = self.ui.bu_tex
			for i=1,self.__bind_nb do
				--self:print( i )
				local bi = bind[i]
				if bi and bi == -1 then
					self:set_bind_2d( seq_bind, i )
					bu_tex[i]:set_texture_bind_2d( seq_bind )
				end
			end
		end
		--self:print( "update "..self:is_active() )
	end
end

--
--	LOAD / SAVE
--
function CELT:load( name )
	local filename = self:build_filename( name )
	--self:print( "load() try to be load -> "..filename )

	local layers, err = app:create_layers( filename )
	self.__b_valid = false
	if layers then
		self:print( filename.." loaded layers" )
		self:__assign( layers )
		self:print( "assign done" )
	else
		self:print_error( "err "..err.." loading file : "..filename )
	end
end
function CELT:reload( name )
	if self.ref.layers then
		self.ref.layers = nil
	end
	self:load()
end
function CELT:save( name )
	if self.fbx then
	else
		local filename = self:build_filename( name )
		local err = app:save_layers_or_module( self.ref.layers, filename )
		if err then
			self:print_error( "err "..err.." saving file : "..filename )
		else
			self:print( filename.." saved" )
		end
	end
end

--
-- UI
--
function CELT:set_focus()				aaa.obj.set_focus_ui( self.ref.layers		)	end
function CELT:set_focus_render()		aaa.obj.set_focus_ui( self.ref.render		)	end
function CELT:focus_shading()			self:get_shading():set_focus_ui(		)	end
function CELT:set_focus_mapping()		aaa.obj.set_focus_ui( self.ref.mapping		)	end
function CELT:set_focus_texturing()		aaa.obj.set_focus_ui( self.ref.texturing	)	end
function CELT:set_focus_layer( index )
	if self.fbx then
		self:print_error( "CELT:set_focus_layer() should be implemented for fbx" )
	else
		aaa.obj.set_focus_ui( self.ref.layer[index]	)
	end
end

function CELT:add_window( title, bus_down,	cx, cy )
	local bu = bus_cur:add_window( title, bus_down,		{cx,cy, 1,2} )
	bu:set_size_orig_for_mini( .1, .1 )
	return bu
end

function CELT:define_ui_internal( bus )
	if self.fbx then return end

	local bu

	bu = bus:add_but_target_param( "Active", nil, self.ref.active )

--	bus:add_but_trig_method( "Layers",  nil,	self, "set_focus" )
	bus:move_right( 1 )
		bus:add_but_trig_method(	"Layer A",		nil,	self,	"set_focus_layer",	1	)
		bus:move_right( 1 )
			bus:add_but_target_param(	"Light",		nil,	self.ref.light	)
			bus:add_but_trig_method(	"Mapping",		nil,	self,	"set_focus_mapping"		)
			bus:add_but_trig_method(	"Texturing",	nil,	self,	"set_focus_texturing"	)
			bus:move_right( 1 )
				for i=1,NB_BIND do
					bu = bus:add_but_target_param(	tostring(i-1),	nil,	self.ref.tex_dim[i]	)
					bu:set_min_max( 0., 2. )
				end
			bus:move_left( 1 )
		bus:move_left( 1 )
		bus:add_but_trig_method(	"Layer B",		nil,	self,	"set_focus_layer",	2	)
		bus:move_right( 1 )
			bus:add_but_trig_method(	"Shader",		nil,	self,	"focus_shading"		)
			bus:move_right( 1 )
				bus:add_but_target_param(	"active",		nil,	self.__shading.ref.obj_active	)
				bus:add_but_trig_method(	"Reload",		nil,	self,	"reload_shader"			)
			bus:move_left( 1 )
		bus:move_left( 1 )
		bus:add_but_trig_method(	"Layer C",		nil,	self,	"set_focus_layer",	3	)
	bus:move_left( 1 )
	bus:move_next_col()

	self.ui = {}
	self.ui.bu_tex = {}
	local s = 1.
	self.__bind_nb = 0
	for i = 1,NB_BIND do
		local bind = self.bind[i]
		if bind then
			self.__bind_nb = i
			bu = bus:add_monitor(	tostring(i), {3.,s*(4-i)+.5, 2*s,s}, bind )
			bu:set_text( i - 1, .1 )
			bu:set_text_xy( .3, .3 )
			bu:set_text_draw( true )
			bu:set_text_color( 0, 1, 1 )
			self.ui.bu_tex[i] = bu
		end
	end
end

function CELT:define_ui( x,y )
	if self.__b_valid and not self.__b_ui_no then
		if not self.fbx and not self.meu_fish_name then
			--table.print( self, "CELT:define_ui()", 2 )
			local bus = BUS:begin_window( self.letter )
				bus:set_bu_pos_load_save( false )
				bus:set_grid_draw( false )
				self:define_ui_internal( bus )
			bus:end_window()

			local bu = self:add_window( self.letter, bus,	x,y )
			return bu
		end
	end
end