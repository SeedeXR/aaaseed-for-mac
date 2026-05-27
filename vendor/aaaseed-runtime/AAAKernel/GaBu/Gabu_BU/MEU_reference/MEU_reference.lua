--
--	REF
--	Centralize reference functions on c_obj and use od self.ref
--

-- this file concern mainly the reference a MEU have on c_obj_ui
function MEU:get_layers()			return self.ref.__layers_main	end
function MEU:get_module()			return self.ref.__module_main	end
MEU.doc.__get_obj_main = "() return a layers or module depending how the meu was initialized with."
function MEU:__get_obj_main()		return self.ref.__obj_main		end

function MEU:get_obj_down_no_error			( name )	return aaa.obj.get_down_no_error 			( self.ref.__obj_main, name )	end
function MEU:get_obj_down					( name )	return aaa.obj.get_down						( self.ref.__obj_main, name )	end

function MEU:get_obj_down_by_class_no_error	( cname )	return aaa.obj.get_down_by_class_no_error	( self.ref.__obj_main, cname )	end
function MEU:get_obj_down_by_class			( cname )	return aaa.obj.get_down_by_class			( self.ref.__obj_main, cname )	end

--[[
function MEU:build_ref_more()
	--todoaqua refine for module
	local layers = self:get_layers()
	--aaa.obj.update_then_draw( slayers )
	for i=1,26 do
		local obj = aaa.layers.get_layer( layers, i )
		if obj then
			aaa.obj.update( obj )
		end
	end
end
--]]

function MEU:get_value_ref( id )	return aaa.layers.get_value_ref(	self:__get_obj_main(), id )	end
function MEU:get_trax( id )			return aaa.layers.get_trax(			self:__get_obj_main(), id )	end

function MEU:get_layer_ref_table( layer_id )		return self.ref.layer[layer_id]		end
function MEU:get_layer( layer_id )
	local layer_ref_table = self:get_layer_ref_table( layer_id )
	if layer_ref_table then
		return layer_ref_table.obj
	end
	return nil
	--return self.ref.__layer_marked
end
function MEU:get_layer_bdd( layer_id, bdd_class )
	local bdd
	local layer = self:get_layer( layer_id )
	if layer then
		bdd = aaa.layer.get_bdd( layer )
		if bdd_class then
			if not aaa.obj.is_class( bdd, bdd_class ) then
				self:print_error( "bdd is of class "..aaa.obj.get_class(bdd).." when we asked for "..bdd_class )
				bdd = nil
			end
		end
	else
		self:print_error( "no layer "..layer_id.." to find a bdd of class "..bdd_class )
	end
	return bdd
end
function MEU:get_layer_obj( layer_id, name_or_obj_type )
	local layer = self:get_layer( layer_id )
	if layer then
		local r 
		r = aaa.obj.get_branch_by_class_no_error( layer, name_or_obj_type )
		if r then return r end

		r = aaa.obj.get_down_no_error( layer, name_or_obj_type )
		if r then return r end

		if self.verbose >= 1 then
			self:print( "in layer_id "..layer_id.." can't find name_symbo or obj type : "..name_or_obj_type )
		end
	else
		if self.verbose >= 1 then
			self:print( "Can't find layer_id "..layer_id )
		end
	end
end

function MEU:get_layer_mapping( layer_id, tex_unit_index )
	local layer_ref = self:get_layer(layer_id)
	--self:print( layer_ref.." --- "..tex_unit_index )
	return aaa.layer.get_tex_unit_mapping( layer_ref, tex_unit_index )
	-- if tex_unit == nil or tex_unit == 0 then
	-- 	return self:get_layer_obj( layer_id, "mapping" )
	-- else
	-- 	if tex_unit==1 or tex_unit==2 or tex_unit==3 then
	-- 		return aaa.layer.get_tex_unit_mapping( self:get_layer(layer_id), id_unit )
	-- 	else
	-- 		self:print( "tex unit "..tex_unit.." have no sense, can't find it." )
	-- 	end
	-- end
end

function MEU:get_layer_bdd_if_class( layer_id, cid )
	local layer = self:get_layer( layer_id )
	if layer then
		local bdd = aaa.layer.get_bdd( layer )
		local b = aaa.obj.is_class( bdd, cid )
		if b then
			return bdd
		else
			self:box_error( "obj \""..aaa.obj.get_name( bdd ).."\"\nis not of class \""..cid.."\" as requested" )
		end
	end
end

function MEU:__find_ref( name_or_obj_type )
	--aaa.box_good( "__find_ref"..name_or_obj_type )
	local ref = self:get_obj_down_no_error( name_or_obj_type )
	if not ref then
		local layer_marked = self.ref.__layer_marked
		if layer_marked then
			ref = aaa.obj.get_branch_by_class_no_error( layer_marked, name_or_obj_type )
		end
	end
	if not ref then
		if self.verbose >= 1 then self:print( "can't find name_symbo or obj type : "..name_or_obj_type ) end
		-- if name_or_obj_type == "multiple" then
		-- 	self:box_debug( "can't find name_symbo or obj type : "..name_or_obj_type )
		-- end
	end
	return ref
end

--todo split into more generic usable piece in layer code
function MEU:__build_refs()
	--self:print_debug( "\t__build_refs() Begin" )

	if self:is_proto_and_isolated() then	--todo clean
		if self.verbose >= 1 then
			self:print_debug( "\t__build_refs() Exit : proto is isolated, so we dont need these refs" )
		end
		return
	end

--todoaqua we deal with layers only add module
	local ref		= self.ref
	local layers	= ref.__layers_main
	if not layers then
		self:print_debug( "\t__build_refs() End : no ref.__layers_main" )
		return
	end
	if not aaa.obj.is_class( layers, "layers" ) then
		self:box_error( "Expecting a layers here and got "..aaa.obj.get_class( layers ) )
		self:print_debug( "\t__build_refs() End : ref.__layers_main not a layers" )
		return
	end

	if GA.b_spy then aaa.spy.push_range( "MEU:__build_refs", 22 ) end

	-- ok we got a layers now we get layers'
	-- transfo
	--todo use the TRANS_TRS class and merge with add_transfo
		ref.trs	=	aaa.obj.get_branch_by_class_no_error( layers, "transfo_trs" )
		if ref.trs then
			ref.trs_tra_x	= param.get_ref( ref.trs, "translate_x" )
			ref.trs_tra_y	= param.get_ref( ref.trs, "translate_y" )
			ref.trs_tra_z	= param.get_ref( ref.trs, "translate_z" )

			ref.trs_rot_x	= param.get_ref( ref.trs, "rotate_x" )
			ref.trs_rot_y	= param.get_ref( ref.trs, "rotate_y" )
			ref.trs_rot_z	= param.get_ref( ref.trs, "rotate_z" )

			ref.trs_sca_x	= param.get_ref( ref.trs, "scale_x" )
			ref.trs_sca_y	= param.get_ref( ref.trs, "scale_y" )
			ref.trs_sca_z	= param.get_ref( ref.trs, "scale_z" )
			ref.trs_sca		= param.get_ref( ref.trs, "scale_factor" )
		else
		--	self:print_inverse( "________________NO TRS_______________" )
		end

	-- fbo
		self:__build_ref_fbo()
	--todoq refine mecanism to specialized generalized extend
	--  we should "declare it" they building ref update _ui update should be implicit
	--	self:build_ref_more()

	-- for each existing layer
	--		get refs for
	--			use_bdd use_shading
	--			color and its alpha if it exist
		local layer = {}
		local i_beg	= 0	--index_begin have being removed 2023 Jan, was param.get( layers, "index_begin" ) + 1
		local i_end	= param.get( layers, "index_end" ) + 1
		for i = i_beg,i_end do
			local obj = aaa.layers.get_layer( layers, i )
			if obj then
				local tab = { obj = obj }
				tab.use_bdd		=	param.get_ref( obj, "use_bdd"		)
				tab.use_shading	=	param.get_ref( obj, "use_shading"	)
				tab.color = aaa.obj.get_down_by_class_no_error( obj, "color" )
				if tab.color then
					tab.alpha = param.get_ref( tab.color, "global_alpha" )
				end
				--todo we should get the bdd and make also a fn to get it easy later
				layer[i] = tab
			end
		end
		ref.layer = layer

	-- one layer can be marked by using "layer" in the name_symbo param
	--	or we use the first layer as branch of layers
	--	this layer will be considered as the default one
	--todo get_obj_down_no_error don't garanty order
		local layer_marked = self:get_obj_down_no_error( "layer" ) or aaa.obj.get_branch_by_class_no_error( ref.__layers_main, "layer" )
		if layer_marked then
			ref.__layer_marked = layer_marked
			--self:print_inverse( "______________________________Oh YES____________________________" )
			ref.bank_2d, ref.bind_2d = aaa.layer.get_bank_bind_2d_ref( layer_marked )
		end

		local r = self:__find_ref( "color" )
		if r then ref.color	= COLOR_REF:create( self:get_name(), r ) end
		ref.mapping		=	self:__find_ref( "mapping" )
		if ref.mapping then
			ref.tex_dim =	param.get_ref( ref.mapping, "tex_use" )
		end
		ref.rendering	=	self:__find_ref( "rendering" )
		ref.model		=	self:__find_ref( "model" )
		if ref.model then
			ref.size_u 		=	param.get_ref( ref.model, "size_u" )
			ref.size_v 		=	param.get_ref( ref.model, "size_v" )
			ref.size_axe 	=	param.get_ref( ref.model, "size_axe" )
			ref.size_factor	=	param.get_ref( ref.model, "size_factor" )
		end

		ref.multiple	=	self:__find_ref( "multiple" )

		-- if ref.__layer_marked then
		-- 	ref.s_texturing	=	param.get_ref( ref.__layer_marked, 		"use_texturing" )
		-- 	ref.texturing	=	self:__find_ref( "texturing" )
		-- 	if ref.texturing then
		-- 		--self:print( "texturing" )
		-- 		for i=1,3 do
		-- 			local r = param.get_obj_attached( ref.texturing, "unit_"..i )
		-- 			if r then
		-- 				local str = "tex_unit_"..i
		-- 				--self:print( str )
		-- 				ref[str] = r
		-- 			end
		-- 		end
		-- 	end
		-- end

	if GA.b_spy then aaa.spy.pop_range() end

	--self:print_debug( "\t__build_refs() End" )
end

--
--	MODEL
--
function MEU:set_model_size_uv( u, v )
	local ref = self.ref
	local model = ref.model
	if model then
		param.set( ref.size_u,	u	)
		param.set( ref.size_v,	v	)
	end
end
function MEU:get_model_size_uv()
	local ref = self.ref
	local model = ref.model
	if model then
		return param.get( ref.size_u ), param.get( ref.size_v )
	end
end