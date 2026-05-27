-- BIND 
-- these fns finction with the ref table but have no connection with interface
function MEU:get_bind_2d()
	local ref = self.ref
	if ref.layer and ref.bank_2d then
		return aaa.img.make_bind_2d( param.get( ref.bank_2d ), param.get( ref.bind_2d ) )
	end
end

function MEU:set_bind_2d( bind, id )
	--aaa.print_fn()
	local maps = self.ref.map
	--table.print( maps, "maps", 2 )
	if maps then
		id = id or 1
		local map = maps[id]
		if map then
			local pset = param.set
			if bind then
				local ba,bi = aaa.img.make_bank_bind_2d( bind )
				--aaa.print_method()
				pset( map.bank_2d, ba )
				pset( map.bind_2d, bi )
			else
				if id == 1 then
					pset( map.bank_2d, -1 )
					pset( map.bind_2d, -1 )
				end
			end
		else
			table.print( self.ref.map, "maps", 2 )
			self:print_error( "MEU:set_bind_2d() no ref map for id "..id )
		end
	else
		self:print_error( "MEU:set_bind_2d() no ref.map" )
	end
end


function MEU:set_texture_bind_2d( id, bind )
	if bind then
		--todo clean and optimize
		local bu_tex = self:get_bu_texture( id )
		--aaa.print_fn()
		if bu_tex then
			bu_tex:set_bind_2d(bind)
		end
	end
end
function MEU:get_texture_bind_2d( id )
	--self:print( "before" )
	local bu_tex = self:get_bu_texture( id )
	--self:print( "after bu_tex "..bu_tex )
	if bu_tex then
		--self:print( "before get_bind_2d()" )
		return bu_tex:get_bind_2d()
	end
	--self:print( "use backup" )
	return self:get_bind_2d()
end


function MEU:get_texture_size( id )
	local bind = self:get_texture_bind_2d( id )
	--self:print( "bind "..bind ) 
	if bind then return aaa.img.get_size( bind ) end
end
function MEU:get_texture_ratio_x( id )
	local sx,sy = self:get_texture_size( id )
	return sy and sx/sy or 1
end
function MEU:get_texture_ratio_y( id )
	local sx,sy = self:get_texture_size( id )
	return sx and sy/sx or 1
end
function MEU:get_texture_dim( id )
	local bu_tex = self:get_bu_texture( id )
	if bu_tex then
		return bu_tex:get_dim()
	end
end
--tmp hack for maa
--used for live debug only
function MEU:get_texture_bind_hack( id )
	local b_debug = false -- id > 100
	if id > 100 then
		id = id - 100
	end
	local bu_tex = self:get_bu_texture( id )
	if b_debug then
		aaa.print_fn()
		table.print( bu_tex, "bu_tex in MEU:get_texture_bind_hack()" )
	end
	if bu_tex then
		--aaa.print_fn()
		if bu_tex.bu.bank then
			local bank, bind = bu_tex.bu.bank:get_value(), bu_tex.bu.bind:get_value()
			--self:print( "bank "..bank.." bind "..bind )
			--self:print( aaa.img.make_bind_2d( bank, bind ) )
			return aaa.img.make_bind_2d( bank, bind )
		else
			self:print( "No bu.bank, id is "..id )
			table.print(  bu_tex, "bu_tex table is", 3 )
			aaa.debug.print_traceback()
		end
	else
		-- if id ~= 1 then
		-- 	self:print(  "No bu_tex, id is "..id )
		-- 	aaa.debug.print_traceback()	
		-- end
	end
	return self:get_bind_2d()
end
function MEU:swap_texture_bind( id_a, id_b )
	local bind_a,bind_b = self:get_texture_bind_2d(id_a),self:get_texture_bind_2d(id_b)
	self:set_texture_bind_2d( id_a, bind_b )
	self:set_texture_bind_2d( id_b, bind_a )
end
--todo now used in facetrak only
function MEU:get_texture_bank_bind( id )
	local bu_tex = self:get_bu_texture( id )
	if bu_tex then
		return bu_tex:get_bank_bind_2d()
	end
end
function MEU:bind_texture_to_unit( id, unit_index )
	gol.set_tex_unit_2d_bind( unit_index or (id - 1), self:get_texture_bind_2d( id ) )
end

-- BU_TEXTURE storage
--
--todotex we should move to bus level to store bu_texture
function MEU:free_bu_texture()
	--todo put it in BUS
	self.__bu_textures = nil
end

function MEU:__store_bu_texture( id, bu_texture )
	if not id then
		self:box_debug( "Houston no id calling MEU:__store_bu_texture()")
		id = 1
	end
	-- create new tex table when needed
	local textures = rawget( self, "__bu_textures" )	-- we don't want to confuse with instance proto 
	if not textures then
		textures = {}
		self.__bu_textures = textures
	end

	-- erase tex[id] if it exist to avoid problem when we redef
	if textures[id] then
		self:error( "MEU.__bu_textures["..id.."] already exist this should not happen" )
		-- we never pass here
	end

	textures[id] = bu_texture
	return bu_texture
end


function MEU:get_bu_texture( id )
	local tex = rawget( self, "__bu_textures" )	-- we don't want to confuse with instance proto 
	if tex then
		return tex[id or 1]
	end
end



-- function MEU:set_ui_tex_bank_bind( id, bank, bind )
-- 	--todo clean and optimize
-- 	local bu_tex = self:get_bu_texture( id )
-- 	if bu_tex then
-- 		if not bank then
-- 			bank, bind = aaa.img.make_bank_bind_2d( bind )
-- 		end
-- 		bu_tex.bu.bank:set_value( bank )
-- 		bu_tex.bu.bind:set_value( bind )
-- 	end
-- end
-- function MEU:set_ui_tex_bank_bind_save( id, b )
-- 	local bu_tex = self:get_bu_texture( id )
-- 	if bu_tex then
-- 		--todo Maa done too much here in a rush
-- 		--self:print( "value is  "..b  )
-- 		bu_tex.bu.bank:set_value_load_save( b )
-- 		bu_tex.bu.bank:set_pos_load_save(false)	--hack to be sure
-- 		bu_tex.bu.bind:set_value_load_save( b )
-- 		bu_tex.bu.bind:set_pos_load_save(false)	--hack to be sure
-- 	end
-- end

function MEU:set_bu_texture_preset_use( id, b_use )
	 local bu_tex = self:get_bu_texture( id )
	 if bu_tex then
		bu_tex:set_preset_use( b_use )
	end
end
--todo how do we deal with dim or even monitor
function MEU:set_bu_texture_save( id, b )
	local bu_tex = self:get_bu_texture( id )
	--table.print( bu_tex.bu.bank.__balues, "bu_tex.bu.bank.__balues", 3 )
	if bu_tex then
		bu_tex:set_value_load_save( b )
		self.__bu_ui.b_build_preset_needed = true	--todo force rebuild of list should be isolated later
	end
end

--todonow clean, used just for Sitem
function MEU:set_ui_tex_src_sel( id, value )
	aaa.print_fn()
	self:get_bu_texture( id ).bu.src_sel:set_value( value )
end



--TEXS prepare for hierarchy
function MEU:get_tex_named()
	return TEXS:get_tex_named()
--	local tab = MEU.__bind_tex
--	return tab and tab or MEU:__init_tex_named()
end
function MEU:get_bind_by_name( name )			return TEXS:get_bind_by_name( name )	end
function MEU:get_name_from_bind( bind )			return TEXS:get_name_from_bind( bind )	end

--[[ if use have to be updated
local function sel_tex_src_min( bu, balue )
	local value = balue:get_value()
	aaa.print( "sel_tex_src_min() value is "..value )
	local src = MEU.__src_video_bind_list[ value + 1 ];
	if src then
		bu.__target_monitor:set_texture_bind_2d( src+96 )
	end
end
--]]

function MEU:load_texture( id )
	--aaa.print_fn()
	local bind = self:get_texture_bind_2d( id )
	TEXS:load_texture( bind )
end

function MEU:__add_bu_texture( rect, name, id, b_dim_or_x_skip,	target_lua_table,	layer_or_tex_unit,	mapping )
	id = id or 1

	local ix = (rect and rect[1]) or 1
	local sx = (rect and rect[3]) or 8
	local sy = (rect and rect[4]) or 3.5
	local iy = (rect and rect[2]) or (1.2 + (id-1) * sy)
	local b_compact
	if rect then b_compact = rect.b_compact end

	name = name or "Tex"

	local bu = self:create_add_bu( BU_TEXTURE, name, {ix,iy, sx,sy}, id, b_dim_or_x_skip )
	self:__store_bu_texture( id, bu )

	if target_lua_table then
		bu:set_target_lua( target_lua_table )
	end

	if layer_or_tex_unit then
		bu:set_target_layer( layer_or_tex_unit,	mapping )
	end

	return bu
end


--we pass a layer_ref or it just like add_bu_texture
MEU.doc.add_bu_texture_target_layer = "( {x,y, sx,sy}, name, id,	b_dim_or_x_skip, layer, mapping )\n"..
									"same than add_bu_texture_target_unit() but we can pass layer and maaping explicitly"
function MEU:add_bu_texture_target_layer(	rect, name, id,	b_dim_or_x_skip,			layer, mapping )
	if layer == nil then
		self:print_error( ":add_bu_texture_target_layer() argument layer should not be nil" )
	else
		local class = aaa.obj.get_class(layer)
		if class~="layer" then
			self:print_error( ":add_bu_texture_target_layer() argument 5 should be a layer not a "..class.."\n filename "..aaa.obj.get_filename(layer) )
		end
	end
	return self:__add_bu_texture( 			rect, name, id, b_dim_or_x_skip,	nil,	layer, mapping )
end

-----------------------------------------------------------------------------------------------------------------------------------------------------
MEU.doc.add_bu_texture_target_unit = "( {x,y, sx,sy}, name, id,	b_dim_or_x_skip )\n"..
									"nil b_dim_or_x_skip will be considered as true\n"..
									"the corresponding texture unit () going thru the MEU ref table will be used as target\n"..
									"so most of the time it will be from layer A/1\n"..
									"use set/get_bind_2d() to access it"
function MEU:add_bu_texture_target_unit(	rect, name, id,	b_dim_or_x_skip,			arg5	)
	if arg5~=nil then
		self:print_error( "MEU:add_bu_texture_target_unit() argument 5 ignored: wrong method called ?")
	end
	id = id or 1
	if b_dim_or_x_skip==nil then b_dim_or_x_skip = true end
	local ref = self.ref
	if id==1 then	--we do it with layer and mapping
		return self:add_bu_texture_target_layer( rect, name, id,	b_dim_or_x_skip,	ref.__layer_marked,		self.ref.mapping	)
	else			-- or we use the texturing part of layer
		--table.print( ref, "ref", 2 )
		local ref_tex_unit = aaa.layer.get_tex_unit( ref.__layer_marked, id-1 )
		--local ref_tex_unit = ref["tex_unit_"..(id-1)]
		--self:print( "tex_unit_"..id.." -> "..ref_tex_unit )
		if not ref_tex_unit then
			--todotex		
			self:print_error( "No ref_tex_unit for id "..id.." solve what we do here" )
		end
		return self:__add_bu_texture( 		rect, name, id, b_dim_or_x_skip,	nil,	ref_tex_unit )
	end
end
MEU.doc.add_bu_texture_target_unit_nb = "( {x,y, sx,sy}, pre, nb_u,nb_v )"
function MEU:add_bu_texture_target_unit_nb(		rect,	pre,	nb_u,nb_v )
	local x,y, sx,sy = rect[1],rect[2], rect[3],rect[4]
	rect.b_compact = true
	sx = sx/nb_u
	rect[3] = sx
	sy = sy/nb_v
	rect[4] = sy
	local id = 1
	for iv=1,nb_v do
		rect[1] = x	
		for iu=1,nb_u do
			self:add_bu_texture_target_unit(	rect, pre..id, id, false )
			rect[1] = rect[1] + sx	
			id = id + 1
		end
		rect[2] = rect[2] + sy
	end
end
-----------------------------------------------------------------------------------------------------------------------------------------------------
MEU.doc.add_bu_texture = "( {x,y, sx,sy}, name, id, b_dim_or_x_skip )\n"..
						"nil b_dim_or_x_skip will be considered as false\n"..
						"just add a standard texture interface with no other automatism\n"..
						"use set/get_bind_2d() to access it"
function MEU:add_bu_texture(		rect, name, id, b_dim_or_x_skip,	arg5	)
	if arg5~=nil then
		self:print_error( "MEU:add_bu_texture() argument 5 ignored: wrong method called ?")
	end
	return self:__add_bu_texture(	rect, name, id, b_dim_or_x_skip )
end

-----------------------------------------------------------------------------------------------------------------------------------------------------
MEU.doc.add_texture_button = "( {x,y, sx,sy}, name )\n"..
						"just add a button with the same Shortcut selector than a BU_TEXTURE\n"..
						"use get_bind_2d() get the bind_2d of the selected one"
function MEU:add_texture_button( rect, name )
	local bu = self:add_button( rect, name, nil, nil, 3 ) -- 3 is value
	TEXS:attach_src_menu( bu )
	bu:set_value_load_save( false )
	bu.get_bind_2d = bu.get_item_data
 	return bu
end
MEU.doc.add_texture_selector = "( {x,y, sx,sy}, name )\n"..
						"just add a selector with some predefined choice for bind\n"..
						"use get_bind_2d() get the bind_2d of the selected one"
function MEU:add_texture_selector( rect, name )
	local row_nb = 3

	local channel_name  = TEXS._t_dst_bind_text
	local channel_symbo = TEXS._t_dst_bind_symbo
	
	local bu
	local nb = #channel_name
	if true then
		bu = self:add_button( rect, name ) --nil, nil, 3 )
		bu:set_menu( channel_name, name.."_menu" )
			bu:get_selector():set_nb_min_0( nb/row_nb, row_nb ):set_item_symbo( 1, unpack(channel_symbo) )
			bu:set_text_selector(true)
			--local par = param.get_ref( ref, "blend_equation" )
			--bu:set_target_param( par )
			
	else
		bu = self:add_selector( rect, name )	
		bu:set_nb_min_0( nb/row_nb, row_nb )


		bu:set_item_text( 1, unpack(channel_name) )
		-- for i=1,nb do		
		-- 	--self:print( name.." - "..TEXS:get_bind_by_name( name ) )
		-- 	bu:set_item_data( i, self:get_bind_by_name( channel_symbo[i] ) )
		-- end

	end
	
	bu.__get_bind_2d = function(bu) return self:get_bind_by_name( channel_symbo[bu:get_value()+1] ) end
	if true then	--one way to do it
		bu.get_bind_2d = bu.__get_bind_2d
	else			--a more optimize for time way
		bu:set_hook_on_value_change( function( bu, balue ) bu.__sel_bind_2d = bu:__get_bind_2d() end )
		bu.get_bind_2d = function(bu) return bu.__sel_bind_2d end
	end

	bu:set_preset_use( false )	--todo we should do a mecanism to load when not in contradiction with bind and bank
	return bu
end
