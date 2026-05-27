
function MEU:get_shading_ref_from_layer( layer_id_or_ref )
	if not aaa.obj.is_ref_no_error( layer_id_or_ref ) then
		layer_id_or_ref = self:get_layer( layer_id_or_ref )
	end
	if layer_id_or_ref then
		return aaa.layer.get_shading( layer_id_or_ref )
	end
end
function MEU:create_shading_from_layer( name, layer_id_or_ref )
	--aaa.print_fn()
	local sha_ref = self:get_shading_ref_from_layer( layer_id_or_ref )
	if sha_ref then
		return SHADING:create( name, sha_ref )
	end
end

function MEU:__add_shading_one( layer_id_or_ref, name, b_do_use_shading )
	--aaa.print_fn()
	local b_error = b_do_use_shading
	layer_id_or_ref = layer_id_or_ref or 1 --should we parse in some case

	local sha = self:create_shading_from_layer( name, layer_id_or_ref )
	if not sha then
		local str = "add_shading_one( "..layer_id_or_ref..", "..name.." )"
		self:print_error( str )
		if b_error then
			self:box_error( str.."\n\tNo shading\n\tthis MEU need to be corrected" )
		end
		return
	end
	
	local ss = self.ref.shading_stuff
	local info = { sha=sha }
	local id_now = ss.nb + 1
	ss.nb = id_now
	ss.infos[ id_now ] = info

	--todo this bad because it don't work with ref
	if not aaa.obj.is_ref_no_error( layer_id_or_ref ) then
		-- in this casse we have the layer index
		info.layer_id = layer_id_or_ref
		local layer_ref_table = self:get_layer_ref_table( layer_id_or_ref )
		if layer_ref_table then
			info.layer_ref = layer_ref_table.obj
			if b_do_use_shading then
				self.ref.use_shading = layer_ref_table.use_shading
			end
		else
			local str = "add_shading_one( "..layer_id_or_ref..", "..name.." )"
			self:print_error( str )
			self:box_error(  str.."\n\tNo layer_ref_table\n\tthis MEU need to be corrected" )
		end
	end
	return sha
end

function MEU:add_shading( layer_id_or_ref_or_table, name )
	name = name or self:get_name()

 	local ss = { nb=0, id_cur=1, infos={} }
	self.ref.shading_stuff = ss

	if type(layer_id_or_ref_or_table)=="table" then
		local t = layer_id_or_ref_or_table
		for i=1,#t do
			local sha_ref = self:get_shading_ref_from_layer( t[i] )
			if sha_ref then
				self:__add_shading_one( t[i], name.."_"..i, i==1 )
			else
				if i~=1 then
					self:print( "did not found shader in layer "..t[i] )
				end
			end
		end
	else
		self:__add_shading_one( layer_id_or_ref_or_table, name, true )
	end
	return self:set_shading_cur( 1 )
end
function MEU:set_shading_cur( id )
	local ss = self.ref.shading_stuff
	local info_cur = ss.infos[id]
	if info_cur then
		ss.info_cur = info_cur
		local sha = ss.info_cur.sha
		ss.sha_cur = sha
		return sha
	else
		self:print_error( "MEU:set_shading_cur( "..id.." ) Failed to find info_cur" )
		aaa.debug.print_traceback()
	end
end
function MEU:get_shading( id )
	local ss = self.ref.shading_stuff
	if ss then
		if id then
			local info = ss.infos[id]
			if info then
				return info.sha
			else
				self:print_error( "MEU:get_shading( "..id.." ) Failed to find info" )
				aaa.debug.print_traceback()
			end
		else
			return ss.sha_cur
		end
	end
end
function MEU:get_shading_layer_id()
	local ss = self.ref.shading_stuff
	if ss then
		if ss.info_cur then
			return ss.info_cur.layer_id
		else
			self:print_error( "MEU:get_shading_layer_id() Failed to find info_cur" )
			aaa.debug.print_traceback()
		end
	else
		self:print_error( "MEU:get_shading_layer_id() Failed to find shading_stuff" )
		aaa.debug.print_traceback()
	end
end


--these finction act as a level of indirection, so we can switch between several shader
-- function MEU:__trig_shading_reload()
-- 	self:get_shading():trig_reload()
-- end
-- function MEU:__trig_shading_edit()
-- 	self:get_shading():edit_shader()
-- end
-- function MEU:__set_shading_focus()
-- 	self:get_shading():set_focus()
-- end
-- function MEU:__update_ui_shading( sha )
-- 	local bu = self.ui.bu_shader_active
-- 	if bu then
-- 		local sha = self:get_shading()
-- 		if sha then
-- 			bu:set_text_color_problem_white( sha:is_valid() )
-- 		else
-- 			self:print_error( "no shader, bad MEU init" )
-- 		end
-- 	else
-- 		self:print_error( "no bu for shading, bad MEU init" )
-- 	end
-- end
function MEU:__change_shading_sel_value( bu )
	local id = bu:get_value()
	local sha = self:set_shading_cur( id )
	self.ref.shading_stuff.bu_shading:attach_shading( sha )
end

function MEU:__add_button_shading(	irect,	name, sha_or_sha_older	) --,	tab_or_ref, name_field, val )
	--table.print( rect, "add_button() rect" )
	local rect = self:make_rect_from_irect( irect )

--	name = self:__do_before_add_bu( name )
	local bu = bus_cur:create_add_bu( BU_SHADING, name, rect, sha_or_sha_older )
--	self:__do_after_add_bu( bu )
	return bu
end
function MEU:__add_shading_ui( rect, b_sel, names )
	local sha = self:get_shading() or self:add_shading()
	if not sha then return end

	local ui = {}
	sha.ui = ui

	local ref = sha.ref
	local bu

	local ix,iy, sx,sy = self:unpack_rect_using_def( rect, {9,2.5, 8,1} )

	--todo this a problem to deal with the active nature of several shader
	--todo no indirection here
	bu = self:__add_button_shading(	{ix,iy,		2,sy},		"Shader",		sha	)
--		bu:set_text_rect_ratio(3)
--		bu:set_method_on_click( self, "__trig_shading_reload" )
--		self.ui.bu_shader_active = bu


	local bu_click
	local IX = ix+2
	if b_sel then
		local ss = self.ref.shading_stuff
		ss.bu_shading = bu
		local nb = ss.nb
		if nb > 1 then
			local bu_sel = self:add_selector(	{IX,iy,		4,sy}, "Shader Cur" )
				:set_nb( ss.nb )
			for i=1,nb do
				bu_sel:set_item_text( i, names and names[i] or (i==1 and "Def" or string.char(64+i-1) ) )
			end
			bu_sel:set_method_on_value_change( self, "__change_shading_sel_value", bu_sel )
			--bu_click = bu_sel
		end
	end

	-- if not bu_click then
	-- 	bu_click = self:add_trig(		{IX,iy,	1.5,sy},	"Shader.Edit"	)
	-- 		:set_text( "Edit" )
	-- 		bu_click:set_color_back( "edit" )
	-- end

	-- if bu_click then
	-- 	bu_click:set_method_on_click(			self, "__set_shading_focus" )
	-- 	bu_click:set_method_on_click_double(	self, "__trig_shading_edit" )
	-- end

	--self:register_update_ui( "__update_ui_shading" )

	return sha
end
function MEU:add_shading_ui( rect, sha )
	if sha then
		--todo this was done before and should perhaps be redone ....
		self:box_error( "second arg should not append anymore\nperhaps you want to use add_shading_ui_multiple()" )
	end
	return self:__add_shading_ui( rect, false )
end
function MEU:add_shading_ui_multiple( rect, names )
	return self:__add_shading_ui( rect, true, names )
end

function MEU:add_shading_sliders_type( rect, sha, shader_type, uniform_type, first_elt, last_element, names )
	local ix,iy, sx,sy = unpack(rect)
	local internal_name = shader_type .. "_" .. uniform_type .. "_"

	local short_shad_type = {	vertex = "vert",	geometry = "geom",	fragment = "frag", compute = "comp"	}
	local post = short_shad_type[shader_type] .. "_" .. uniform_type

	local getter = sha["get_ref_"..post ]
	--self:print_error("get_ref_"..post )

	local elt_count = last_element - first_elt + 1
	local dy = sy / (elt_count)

	local coordinates = { "x", "y", "z", "w" }
	local name

	local res = {}
	local bu

	for i = first_elt, last_element do
		if names and i - first_elt + 1 <= #names then
			name = names[i - first_elt + 1]
		else
			name = ""
		end
		if uniform_type == "vec4" then
			for k = 1, 4 do
				local name_k = name .. coordinates[k] .. " " .. i
				bu = self:add_slider(	{ix + sx * .25 * (k - 1),iy,	sx * .25,dy}, internal_name..name_k, getter(sha, i, k), nil, 0, 0, 16)
					:set_text(name_k)
					:set_meter( true )
					:set_color_back(coordinates[k])
				res[name_k] = bu
			end
		else
			if name == "" then name = uniform_type.." "..i end
			bu = self:add_slider(	{ix,iy,	sx,dy}, internal_name..i, getter(sha,i), nil, 0, 0, 16 )
				:set_text( name )
				:set_meter( true )
				if uniform_type == "int" then
					bu:set_color_back("grey")
				end
			res[name] = bu
		end
		iy = iy + dy
	end
	--self:print_error( "set_save_"..post )
	sha["set_save_"..post]( sha, false, first_elt, last_element )
	return res
end

local shader_name_to_name_long = { vert="vertex", geom="geometry", frag="fragment", comp="compute" }

function MEU:add_shading_sliders( sha, name, iy,sy, ix,sxa,sxb,sxc )
	ix = ix or 1
	sxa = sxa or 4
	sxb = sxb or 4
	sxc = sxc or 8

	local name_new = shader_name_to_name_long[name]
	if name_new then name = name_new end

	local tab_nb = SHADING[name.."_nb"]
	-- we split in 2 colums eventually
	local nb = tab_nb.float
	local limit = 14
	local nb1 = math.min( nb, limit )
	self:add_shading_sliders_type(		{ix,     		iy, 	sxa,	nb1*sy},		 sha, name, "float",	1, nb1, {"time"} )
	if nb > limit then
		nb1 = nb - limit
		--self: print_error( nb.." "..nb1 )
		local oiy = (limit-nb1)*sy
		self:add_shading_sliders_type(	{ix+sxa,		iy+oiy,	sxb,	nb1*sy},		 sha, name, "float",	limit+1, limit+nb1 ) 
	end
	self:add_shading_sliders_type(		{ix+sxa,		iy,		sxb, 	tab_nb.int*sy},  sha, name, "int",		1, tab_nb.int)
	self:add_shading_sliders_type(		{ix+sxa+sxb,	iy, 	sxc, 	tab_nb.vec4*sy}, sha, name, "vec4",		1, tab_nb.vec4)		
end

function MEU:add_shading_sliders_tab( sha, name, iy,sy, ... )
	if type(name)=="table" then
		for _,elt_name in ipairs(name) do
			self:add_shading_sliders_tab( sha, elt_name, iy,sy, ... )
		end
		return
	end
	self:set_tab_key( name )
	self:add_shading_sliders( sha, string.lower(name),  iy,sy, ... )
end
