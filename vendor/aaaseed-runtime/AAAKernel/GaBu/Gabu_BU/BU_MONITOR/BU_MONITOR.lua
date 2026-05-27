local f_def = 1
if CLASS.DECLARE( "BU_MONITOR", BU_WINDOW,  {
	ref = {},
	__text_info_size_fx	= f_def,
	__text_info_size_fy	= f_def,
	__sx_min			= 1/8,
	__sy_min			= 1/32,
	__b_info_draw		= false,
	__b_size_draw		= false,
	__b_channel_draw	= true,
	__channel_colors	= { { 1,0,0, 1 }, { 0,1,0, 1 }, { 0,0,1, 1 }, { 1,1,1, 1 } },
	__b_undo_redo_using_values = true,
	__b_ui_change_bind	= false,
	} ) then

	BU_MONITOR:set_class_status_doc(	CLASS.STATUS.GABU,
										"Encapsulate an image monitor, visualization of a texture",
										"with its associated UI" )

	BU_MONITOR.__b_draw_inside_mini = true
	BU_MONITOR:set_size_ratio_fix( true )

	local ref = BU_MONITOR.ref
	local o = aaa.get_caller()
	o = aaa.obj.get_root( o )
	ref.layers = o
	ref.layer_a = aaa.layers.get_layer( o, 1 )
	ref.bank_2d, ref.bind_2d = aaa.layer.get_bank_bind_2d_ref( ref.layer_a, 0 )

	local sha = aaa.layer.create_shading( ref.layer_a, "BU_MONITOR" )
	ref.shading = sha
	ref.mapping = aaa.layer.get_mapping( ref.layer_a )
		ref.tex_left 	=	param.get_ref( ref.mapping, "tex_left" 		)
		ref.tex_right 	=	param.get_ref( ref.mapping, "tex_right" 	)
		ref.tex_bottom 	=	param.get_ref( ref.mapping, "tex_bottom"	)
		ref.tex_top 	=	param.get_ref( ref.mapping, "tex_top" 		)

	param.set_save( ref.mapping, "tex_u",		false )
	param.set_save( ref.mapping, "tex_u_ori",	false )
	param.set_save( ref.mapping, "tex_v",		false )
	param.set_save( ref.mapping, "tex_v_ori",	false )

	param.set_save( ref.mapping, "tex_left",	false )
	param.set_save( ref.mapping, "tex_right",	false )
	param.set_save( ref.mapping, "tex_bottom",	false )
	param.set_save( ref.mapping, "tex_top",		false )

	sha:set_save_frag_int(		false,	1,2 )
	sha:set_save_frag_float(	false,	1,2 )
	sha:set_save_frag_vec4(		false,	1,2 )

	param.set_save( ref.layer_a, "bank_2d", false )
	param.set_save( ref.layer_a, "bind_2d", false )
	--aaa.obj.update_then_draw( ref.layers )	--make sure it is updated so we have a color object
	--ref.crosshair			= aaa.obj.get_by_name_symbo( "gabu_bu_text_crosshair" )
	--	ref.crosshair_active	= param.get_ref( ref.crosshair, "active" )
end

--
-- SET/GET
--
function BU_MONITOR:get_values()
	local ui = self.__ui
	local tab = { bind=self:get_texture_bind_2d() }
	for k,v in pairs(ui) do
		tab[k] = v:get_value()
	end
	--table.print( tab )
	return tab
end
function BU_MONITOR:set_values( value )
	--table.print( value )
	local ui = self.__ui
	if type(value)=="table" then
		for k,v in pairs(value) do
			if k == "bind" then
				self:set_texture_bind_2d( v )
			else
				local bu = ui[k]
				if bu then
					bu:set_value( v )
				end
			end
		end
	else
		self:set_texture_bind_2d( value )
	end
end

--todo no undo yet
function BU_MONITOR:set_values_ui( value )
	--table.print( value, "set_values_ui" )
	local ui = self.__ui
	if type(value)=="table" then
		for k,v in pairs(value) do
			if k == "bind" then
				self:__ui_set_texture_bind_2d( v )
			else
				local bu = ui[k]
				if bu then
					bu:set_value( v )
				end
			end
		end
	else
		self:__ui_set_texture_bind_2d( value )
	end
end

function BU_MONITOR:is_ui_change_bind()		return self.__b_ui_change_bind					end
function BU_MONITOR:set_ui_change_bind(b)	self.__b_ui_change_bind = b		return self		end

function BU_MONITOR:do_paste()
	local bu_bank = self.__window_bank 
	if bu_bank then
		local clip = BUI.__clipboard
		table.print( clip, "clip" )
		local bind_src = clip.bind
		if bind_src then
			local filename = aaa.img.get_bind_filename(bind_src)
			local bind = self:get_texture_bind_2d()
			aaa.img.destroy( bind )
			aaa.img.set_bind_filename( bind, filename )
			return true
		end		
	elseif self:is_ui_change_bind() or self:__is_delegate_get_set_bind_2d() then
		return oo.getsuper(BU_MONITOR).do_paste( self )
	end
end

function BU_MONITOR:get_uif_zones( ox,oy )
	local zones = {}	--oo.getsuper(BU_RECT).get_uif_zones( self )
	local sx,sy = 3,1
	local dy = .2
	ox, oy = ox or 0, oy or 0
	local x,y = ox,oy
	local dx = .2

	zones.Open			= {	type="button",		rect={x,y,		sx,sy}	}
	y = y - sy - dy
	zones.Save			= {	type="button",		rect={x,y,		sx,sy}	}
	y = y - sy - dy
	zones["Save As"]	= {	type="button",		rect={x+.6,y,	sx,sy}	}
	y = y - sy - dy
	zones.Print			= {	type="button",		rect={x,y,		sx,sy}	}

	y = oy
	x = x + sx + dx
	local f = 2
	local sxf = sx*f
	local xf = x + sx * (f-1)/2
	zones["Clear Filename"]	= {	type="button",	rect={xf,y,	sxf,sy}	}
	x = x + .6 
	y = y - sy - dy
	zones.Destroy		= {	type="button",		rect={x,y,			sx,sy}	}
	y = y - sy - dy
	local bu_bank = self.__window_bank 
	if bu_bank then
		zones.Dec		= {	type="button",		rect={x,y,			sx,sy}	}
		zones.Inc		= {	type="button",		rect={x+sx+dx,y,	sx,sy}	}
		y = y - sy - dy
	else
		local bind_def = self.__bind_at_creation
		if bind_def and bind_def ~= self.__infos.bind then
			zones["Reset bind"] =  { type="button",		rect={x+sx*.5,y,	sx*1.5,sy}	}
		end
		y = y - sy - dy
	end


	
	self:add_uif_zones_base( zones, 2.6-oy, 2 )
	return zones
end

function BU_MONITOR:load_image()
	local bind = self:get_texture_bind_2d()
	if bind then
		TEXS:load_texture( bind )
	end
end


function BU_MONITOR:do_command_for_bind( command, bind )
	local function save_image( bind, b_dialog )
		local ret
		if b_dialog then
			ret = aaa.img.save_dialog( bind, b_dialog )
		else
			local ymd = aaa.time.get_str_ymd_underscore()
			local hms = aaa.time.get_str_hms()
			local name = aaa.dir.get_dir_snapshot().."/Monitor_Bind_"..bind.."_at_"..ymd.."_"..hms
			aaa.img.move_from_gpu( bind )
			aaa.img.save( bind, name )
			ret = true
		end
		return ret
	end

	aaa.print_fn()
	local b_used = false
	if bind then
		b_used = true
		if		command == "open"				then	TEXS:load_texture( bind )
		elseif	command == "save"				then	save_image( bind )
		elseif	command == "save_as"			then	save_image( bind, true )
		elseif	command == "print"				then	aaa.img.print( bind )
		elseif	command == "clear_filename"		then	aaa.img.set_bind_filename( bind )
		elseif	command == "destroy"			then	aaa.img.destroy( bind )
		else									b_used = false
		end
	else
		self:print_error( "need a bind for command "..command )
	end
	return b_used
end

function BU_MONITOR:do_uif_command( uif )
	aaa.print_fn()
	local command = BU:get_uif_name_sel( uif )
	local b_used = self:do_command_for_bind( command, self:get_texture_bind_2d()  )
	if not b_used then
		b_used = true
		if		command == "dec"		then	self:__move_image(-1)
		elseif	command == "inc"		then	self:__move_image( 1)
		elseif  command == "reset_bind"	then	self:__ui_set_texture_bind_2d(self.__bind_at_creation)
		else									b_used = false
		end
	end
	
	return GABU_OBJ.do_uif_command_with_super( self, b_used, uif, BU_RECT )
end

function BU_MONITOR:__move_image( inc )
	local bu_bank = self.__window_bank 
	if bu_bank then
		local id_mon = bu_bank:get_monitor_cur_id()
		bu_bank:__swap_image( id_mon, inc )
	end
end

function BU_MONITOR:get_texture_bind_2d()
	local bu = self.__bu_image
	return bu and bu:get_texture_bind_2d()
end

function BU_MONITOR:__get_texture_bind_2d_special()
	local t = self.__t_method_get_set_bind_2d
	if t then
		local bind
		--get_texture_bind_2d for MEU
		local obj, get, arg = t.obj, t.get, t.arg 
		--self:print(  t.obj.." for "..t.id.." bind will be set to : " .. bind )
		if arg then
			bind = obj[get]( obj, arg )
		else
			bind = obj[get]( obj )
		end
		--aaa.print_fn( true )
		return bind
	end
end

function BU_MONITOR:__is_delegate_get_set_bind_2d()
	return self.__t_method_get_set_bind_2d ~= nil
end
function BU_MONITOR:__delegate_get_set_bind_2d( obj, get, set, arg )
	if obj then
		self.__t_method_get_set_bind_2d = {obj=obj, get=get, set=set, arg=arg }
	else
		self.__t_method_get_set_bind_2d = nil
	end
end

function BU_MONITOR:__set_bind_2d_special( bind )
	local t = self.__t_method_get_set_bind_2d
	if t then
		aaa.print_fn( true )
		--set_texture_bind for MEU
		local obj, set, arg = t.obj, t.set, t.arg 
		--self:print(  t.obj.." for "..t.id.." bind will be set to : " .. bind )
		if arg then
			obj[set]( obj, arg, bind )
		else
			obj[set]( obj, bind )
		end
		return true
	end
end

--todo remove this hack
function BU_MONITOR:__ui_set_texture_bind_2d( bind )
	if not self:__set_bind_2d_special( bind ) then
		self:set_texture_bind_2d( bind )
	end
end

function BU_MONITOR:__set_bind_2d_low( bind )
	local infos	= self.__infos

	local bind_reference = self.__bind_at_creation
	if bind_reference then
		self:set_text_color( bind_reference == bind and "white" or "red" )
	end

	self.__bu_image:set_texture_bind_2d( bind )
	infos.bind = bind
	infos.name_to_use = TEXS:get_name_from_bind( bind )

	self:__update_infos()
end

function BU_MONITOR:set_texture_bind_2d( bind )
	--todola
	if not bind or bind < 0 then return end

	local infos	= self.__infos
	if bind and (infos.bind ~= bind) then
		self:__set_bind_2d_low( bind )
	else
		self:__update_infos()
	end
end

function BU_MONITOR:__set_texture_bind_2d_ui( bind_next )
	local bind_prev = self:get_texture_bind_2d()
	self:set_texture_bind_2d( bind_next )
	if bind_prev ~= bind_next then
		local undo = self:make_table_to_call_method( self, "set_texture_bind_2d", bind_prev	)
		local redo = self:make_table_to_call_method( self, "set_texture_bind_2d", bind_next	)
		ga:add_undo_redo( undo,redo )
	end
end

function BU_MONITOR:__dec_texture_bind_2d_ui( nb )	self:__set_texture_bind_2d_ui( self:get_texture_bind_2d() - (nb or 1) )		end
function BU_MONITOR:__inc_texture_bind_2d_ui( nb )	self:__set_texture_bind_2d_ui( self:get_texture_bind_2d() + (nb or 1) )		end

function BU_MONITOR:set_draw_channel( b )	self.__b_channel_draw = b 							end

-- deal with the info field
function BU_MONITOR:set_info_draw(	b )		self.__b_info_draw = b 								end
-- deal with the size field
function BU_MONITOR:set_size_draw(	b )		self.__b_size_draw = b 								end
-- deal with both
function BU_MONITOR:set_text_info_size_fxy( fx,fy )
	self.__text_info_size_fx = fx
	self.__text_info_size_fy = fy or fx
end

function BU_MONITOR:set_texture_flip_u( b )		self.__bu_image:set_texture_flip_u( b )			end
function BU_MONITOR:set_texture_flip_v( b )		self.__bu_image:set_texture_flip_v( b )			end

function BU_MONITOR:do_key(key)
	self:print_do_key( "BU_MONITOR", key )

	local b_key_used = true
	if		key == 32 then
		local mon = self.__b_fix and self:get_gp_monitor_associated() or self
		mon:set_window_state( aaa.keyboard.is_ctrl() and "flip_mini" or "flip_full" )
	--todo d is in conflict with depth
	-- elseif	key == 100 then -- d like display
	-- 	local b = not self.__b_info_draw 
	-- 	self:set_info_draw( b )
	-- 	self:set_size_draw( b )
	-- elseif		key == 116 then	--t
	-- 	self.__b_draw_top = not self.__b_draw_top
	else
		if not aaa.keyboard.is_alt() then
			if		key==43 then	-- +
				self:inc_render_mode_alpha()
			elseif	key==45 then	-- -
				self:dec_render_mode_alpha()
			else
				b_key_used = false
			end
		else
			b_key_used = false
		end
	-- if (key==43 or key==45) and aaa.keyboard.is_alt() then
	-- 	if not self:is_mini() and not self.__b_fix then
	-- 		local sx,sy = self:get_sxy()
	-- 		local f = key==43 and 1.1 or 1/1.1
	-- 		self:set_sxy( sx*f,sy*f )
	-- 	end
	-- else
	end
 	return b_key_used or oo.getsuper(BU_MONITOR).do_key( self, key )
end

function BU_MONITOR:do_key_special(key)
	self:print_do_key_special( "BU_MONITOR", key )

	local b_key_used = false
	if not self:is_mini() and not self.__b_fix then
		if aaa.keyboard.is_alt() then
		elseif aaa.keyboard.is_ctrl() then
			if self:is_ui_change_bind() then
				if inside( key, 272, 273 ) then
					local nb = aaa.keyboard.is_shift() and 32 or 1
					if	    key == 273 then	self:__dec_texture_bind_2d_ui( nb )	--down
					elseif	key == 272 then	self:__inc_texture_bind_2d_ui( nb )	--up
					end
					b_key_used = true
				end
			end
		else

		end
	end

	return b_key_used or oo.getsuper(BU_MONITOR).do_key_special( self, key )
end
--	SIZE
--
function BU_MONITOR:__adjust_size()
	local infos	= self.__infos
	local bind = infos.bind

	local sx,sy
	if self.__b_fix then
		sx,sy = infos.bind_sx, infos.bind_sy
		--this version provoke successive reduction until it get too small
		--sx,sy = self:fit_sxy_in( sx,sy, infos.sx,infos.sy )		
		local up_sx,up_sy = self:get_bus_down():get_transfo_sxy()
		if self.__b_texture_fill then
			sx,sy = up_sx,up_sy
			--self:print( "sx/sy "..sx.."/"..sy )
			--sx,sy = 1,up_sy/up_sx
			sy = sx*up_sy/up_sx
		else
			sx,sy = self:fit_sxy_in( sx,sy, up_sx,up_sy )
			--	self:print( "__adjust_size() _________________ result "..sx.." "..sy )		
		end
		self.__bu_image:set_sxy_direct( sx,sy )	
	else
		if self:is_full_page() then
		elseif self:is_mini() then
		else
			sx,sy = self:get_sxy()
			if infos.bind_sx then
				sy = sx * infos.bind_sy / infos.bind_sx
			else
				sy = sx
			end
			--self:print( "__adjust_size() "..sx.." "..sy )
			self:set_sxy( sx,sy )
		end
	end
end

--todoq we have to call it too often
function BU_MONITOR:__update_infos()
	--local bind_old = self:get_texture_bind_2d()
	--todo do it also when the content of a texture change
	local infos	= self.__infos
	local bind = infos.bind

	--self:print( "__update_infos() bind is " .. bind )
	--self:print ( aaa.img.get_format_name( bind ) )
	--todo don't call every frame
	local bind_sx, bind_sy, bind_ch_nb = aaa.img.get_size_channel( bind )
	--self:print( bind.." "..bind_sx )
	if bind_sx and bind_sx ~= 0 then
		infos.bind_ch_nb = bind_ch_nb
		infos.bind_format_name = aaa.img.get_format_name( bind )
		--self:print( "BU_MONITOR:__update_infos() "..infos.bind_format_name ) 
		if infos.bind_sx ~= bind_sx or infos.bind_sy ~= bind_sy then
			infos.bind_sx = bind_sx
			infos.bind_sy = bind_sy
			self:__adjust_size()
		end
	else
		infos.bind_sx = nil
		infos.bind_sy = nil
		infos.bind_ch_nb, infos.bind_format_name = nil, nil
	end
end

BU_MONITOR.doc.set_texture_fill = "( bool ) set an attribute so the texture will use all the monitor space"
function BU_MONITOR:set_texture_fill( b )
	self.__b_texture_fill = b
	self:__adjust_size()
end

-- function BU_MONITOR:set_mini( b )
-- 	if self.__bu_more		then self.__bu_more:set_active( false ) 	end
-- 	if self.__bu_draw_top	then self.__bu_draw_top:set_active( false )	end
-- 	oo.getsuper(BU_MONITOR).set_mini( self, b )
-- end
function BU_MONITOR:set_info_src_bind( bind )
	self.__infos.src_name = TEXS:get_name_or_bind( bind )
end

function BU_MONITOR:get_info_draw_str( b_line_several )
	local infos	= self.__infos
	local name	= infos.name_to_use

	local sep = b_line_several and "\n@\n" or "@"
	local str = infos.title
	local src_name = infos.src_name
	if src_name then
		str = str.."("..src_name.."):"
	else
		str = str..":"
	end
	
	if name then str = str..name end
	str = str..sep..infos.bind
	return str
end
function BU_MONITOR:get_size_draw_str( b_line_several )
	local infos	=	self.__infos
	local bind_sx = infos.bind_sx
	if bind_sx == nil then
		return "No Size info"
	end
	local sep = b_line_several and "\nx\n" or "x"
	local sep2 = b_line_several and "\n" or " "
--	return bind_sx..sep..infos.bind_sy..sep2..infos.bind_ch_nb.."ch "..infos.bind_format_name
	return bind_sx..sep..infos.bind_sy..sep2..infos.bind_format_name
end
function BU_MONITOR:get_info_size_draw_str( b_line_several )
	return self:get_info_draw_str( b_line_several ).." "..self:get_size_draw_str( b_line_several )
end

function BU_MONITOR:draw_back()	-- no colors in the back if mobile
	--if true then return end
	if not BU_MONITOR.__b_draw then	return end
	---self:print( "BU_MONITOR:draw_back()"..self.__back_alpha ) 
	--if self.__b_fix then

--	local bind = self:get_texture_bind_to_draw()
--	local sx = aaa.img.get_size( bind )
	 local infos = self.__infos
	if not infos.bind_sx then
		self:draw_back_inactive( self.__alpha_updated )
	else
		oo.getsuper(BU_MONITOR).draw_back( self )
	end
end

--BU_MONITOR.draw = nil
-- function BU_MONITOR:draw()
-- end
function BU_MONITOR:__transform_and_draw()
	if not BU_MONITOR.__b_draw then	return end
	oo.getsuper(BU_MONITOR).__transform_and_draw(self)
end
--function BU_MONITOR:draw_central()	end -- we want to draw after BUs down
local color_info = { 0,1,1, 1 }
local color_size = { 1,1,0, 1 }
function BU_MONITOR:draw_fore()
	--if true then return end
	if not BU_MONITOR.__b_draw then return end

	oo.getsuper(BU_MONITOR).draw_fore( self )

	--if true then return end
	--aaa.debug.print_traceback()

	local alpha = self.__alpha_updated

	local infos	= self.__infos
	local b_tex_present = infos.bind_sx ~= nil
	local b_contact = self:is_contact()
	local b_channel_draw = b_tex_present and (self.__b_channel_draw or b_contact or self.__s_render_alpha==2)
	local b_size_draw = b_tex_present and (self.__b_size_draw or b_contact)
	local b_info_draw = (self.__b_info_draw or b_contact)

	--todo understand and comment
	-- if not self.__b_fix then
	-- 	self:set_text_draw( true )
	-- 	--self:set_text_draw( self:is_mini() or (not self.__b_info_draw) )
	-- end
	if self:is_text_draw() then
		self:__draw_text_on_top( alpha )
	end
	
	if not self:is_mini() then
		local b_draw_top = self.__b_draw_top and (self.__b_grid or self.__b_center or self.__b_frame)
		if b_draw_top then
			gol.color_white( self.__top_alpha * alpha )
			if self.__b_grid then
				for i=1,self.__grid_nb_u-1 do
					local x = i/(self.__grid_nb_u) - .5
					gol.draw_lines_2d(	x, -.5,		x, .5	)
				end
				for i=1,self.__grid_nb_v-1 do
					local y = i/(self.__grid_nb_v) - .5
					gol.draw_lines_2d(	-.5, y,		.5,	y	)
				end
			end
			if self.__b_center then
				aaa.draw_null_2d(	0, 0, 0, .1 )
			end
			if self.__b_frame then
				aaa.draw_rect_line_size( self.__frame_size )
			end
		end

		if b_channel_draw then
			local ch_nb = infos.bind_ch_nb
			if ch_nb then
				local cols = BU_MONITOR.__channel_colors
				local s = .06
				local sx = s / 1.77
				local y = -.48
				local x
				for i = 1,ch_nb do
					local tc = cols[i]
					gol.color( tc[1], tc[2], tc[3], tc[4]*alpha )
					x = .48 + (-ch_nb+i-1)*sx
					if i==4 then
						local s_alpha = self.__s_render_alpha
						--self:print( "s_alpha "..s_alpha )
						if s_alpha == 2 then
							gol.set_line_width(3)
							aaa.draw_rect_line(	x,y, x+sx, y+s )
						else
							aaa.draw_rect(	x,y, x+sx, y+s )
							if s_alpha==0 then
								gol.set_line_width(3)
								gol.color_red( alpha )
								sx,s = sx*.5,s*.5
								aaa.draw_mul_line(	x+sx, y+s, sx*.9, s*.9 )
							end
						end
					else
						aaa.draw_rect(	x,y, x+sx, y+s )
					end
				end
			end
		end

		if b_info_draw or b_size_draw then		--	fix case
			--gol.set_line_width( 1 )
			--self:print( alpha )

			local rx = self:get_ratio_x()
			local fsx,fsy = self.__text_info_size_fx,self.__text_info_size_fy
			if rx >= 1 then
				fsy = fsy * .1
				fsx = fsy / rx
			else
				fsx = fsx * .1
				fsy = fsx * rx
			end
 	
		 	--if self.__b_fix then
				--if self.__b_fix then fsx = fsx*.8 end
		 		if b_contact then	fsx,fsy = fsx*2,fsy*2
		-- 		else				tsx, tsy = tsx*.8,tsy*.8
				end
			--end

			--local tt_pushed = rawget( self, "__text_type" )
			--self:set_text_nice()

			local to = self.__text_obj
			local function draw_text( txt, x,y )		
				to:set_next_shadow()
				to.__shadow_du = .8 / rx
				to.__shadow_dv = -.8
				to:__draw_for_bu( self, txt, x,y,0, fsx,fsy, "left","left" )
			end

			local y_info,y_size 
			if rx >= 1 then
				y_info,y_size = .5-fsy*.85, -.5+fsy*.08
			else
				y_info,y_size = .5-fsy*.85, -.5+fsy*3.08
			end

			if b_info_draw then
				gol.color_cyan( alpha )
				draw_text( self:get_info_draw_str(false), -.5,y_info )
			end
			if b_size_draw then
				gol.color_yellow( alpha )
				draw_text( self:get_size_draw_str(false), -.5,y_size )
			end
			--self:__set_text_type( tt_pushed )
		end
		
		if b_contact and not aaa.keyboard.is_alt() then
			gol.color_white( .5 )
			gol.set_line_width(4)

			if aaa.keyboard.is_ctrl() then
				gol.draw_lines_2d(	0,	-.5,		0,	.5	)
			else
				local x = (self.__UI_TOP_RIGHT+.5)*.5
				gol.draw_lines_2d(
					-.5,					self.__UI_TOP_DOWN,		self.__UI_TOP_LEFT,		self.__UI_TOP_DOWN,
					self.__UI_TOP_RIGHT,	self.__UI_TOP_DOWN,		.5,						self.__UI_TOP_DOWN,
					-.5,					self.__UI_TOP_UP,		self.__UI_TOP_LEFT,		self.__UI_TOP_UP,
					self.__UI_TOP_RIGHT,	self.__UI_TOP_UP,		.5,						self.__UI_TOP_UP,
					self.__UI_TOP_LEFT,		-.5,					self.__UI_TOP_LEFT,		.5,
					self.__UI_TOP_RIGHT,	-.5,					self.__UI_TOP_RIGHT,	.5,
					x,						self.__UI_TOP_UP,		x,						.5
					)
			end
		end
	end
end

--todo re
function BU_MONITOR:update()
--	self.__bu_image:set_texture_bind_2d( bind )
--	self.__bu_image.draw_bind = BU_MONITOR.draw_bind_monitor

	local bind = self:__get_texture_bind_2d_special()
	if bind then
		self:set_texture_bind_2d( bind )
	else
		self:__update_infos()
	end

	self.__alpha_updated = self:get_alpha_bu_to_draw()	--* self.__back_alpha

	oo.getsuper(BU_MONITOR).update( self )
end

function BU_MONITOR:__define_ui_more( name, sx,sy, ui )
	local bus = BUS:create( name.."_more" )
	bus:init_begin()

		bus:set_constraint( "edge" )
		bus:set_bu_pos_load_save( false )

		local bu
		local my = sy*.05
		local SY = sy / 16.
		local DY = SY * .2
		local to = sy*.5 - my

		local mx = my
		local SX = SY
		local SXT = sx - 2*mx
		local le = -sx*.5 + mx
		local ri = sx*.5 - mx

		local y = to - SY

		bu = bus:add_selector( "alpha", {le+SXT*.5,y+SY*.5,	SXT,SY} )
			bu:set_nb_min_0( 3, 1 )
			bu:set_item_text( 1, "No", nil, "-" )
			bu:set_target_lua( self, "__s_render_alpha" )
			--bu:set_value( 1 )
			--bu:set_ui_active( false )
			ui.alpha = bu
		y = y -SY -DY

		ui.hsv		= bus:add_button( "Hsv", 	{le+SY*.5,y+SY*.5, SY,SY} ):set_text_rect_ratio( 2. )

		local nb_y = 2
		y = y - SY * nb_y - DY
		bu = bus:add_selector(  "how",	{le+SXT*.5, y+SY*(nb_y/2),	SXT,SY*nb_y} )
			bu:set_nb_min_0( 3, nb_y )
			bu:set_item_text( 1,	"RGB",	"Gr",	"Alpha",
									"R",	"G",	"B"
	--								"H",	"S",	"V",
	--								"Hsv",	"2",	"3",
	--								"-1",	"-2",	"-3"
							)
			bu:set_value( 0 )
			ui.mode = bu

		y = y -SY -DY
		ui.invert	= bus:add_button( "Invert",	{le+SY,			y+SY*.5, SY*2, SY} )
		--	:set_text_rect_ratio( 2. )
		ui.abs		= bus:add_button( "Abs",	{le+SY + SY*2,	y+SY*.5, SY*2, SY} )

		y = y - SY - DY
		bu = bus:add_selector( "out",			{le+SXT*.5,y+SY*.5,		SXT,SY} )
			bu:set_nb_min_0( 3 )
			bu:set_item_text( 1,	"Direct",	"White",	"Spectrum"  ) --"Fake" )
			bu:set_value( 0 )
			--bu:set_ui_active( false )
			ui.out = bu

		y = y - SY*2 - DY
		bu = bus:add_selector( "Orientation",	{le+SXT*.25,y+SY,		SXT*.5,SY*2} )
			bu:set_nb( 2, 2 )
			bu:set_item_text( 1,	"LT",	"RT",	"LB",	"RB"  )
			bu:set_value(3)
			--bu:set_ui_active( false )
			ui.orientation = bu

		y = y - SY - DY
		local ox = .25
		bu = bus:add_selector( "level_use",		{le+SXT*.5,y+SY*.5,		SXT,SY} )
			bu:set_nb_min_0( 4 )
			bu:set_item_text( 1,	"no",	"Level",	"Triangle", "Loop" )
			bu:set_value(0)
			bu:set_active( false )
			ui.level_use = bu

		y = y - SY
		bu = bus:add_slider_two( "Level", 		{le+SXT*.25,y+SY*.5,	SXT,SY}	)
				bu:set_target_lua( self, "__level_min", nil, 1 )
				bu:set_min_max( 0, 2, 1 )
				bu:set_min_max( 0, 2, 2 )
				bu:set_value( 0, 1 )
				bu:set_target_lua( self, "__level_max", nil, 2 )
				bu:set_value( 1, 2 )
				bu:set_active( false )
				ui.level = bu

		local ssx = SXT / 5
		local ssy = SY*2
		y = -sy*.5+my+ssy*.5
		local function add( table, pre, ox,	name, color_back, val )
			local name_low = string.lower(name)
			local bu = bus:add_slider( pre.."_"..name_low,	{le+(ox+.5)*ssx,y, ssx,ssy} )
				bu:set_text( name )
				bu:set_meter_color( color_back )
				--self:box_error( "  ".. key )
				bu:set_target_lua( table, name_low )
				if pre == "offset" then		bu:set_meter( false ):set_min_max( -1, 1 )
				else						bu:set_meter( true ):set_min_max( 0, 4 ):add_values_def( .5, 1, 2 )
				end
				bu:set_value( val )
			return bu
		end
		local t = {}
		add(	t,	"factor", 0, "Red",		{1, 0, 0, 1},		1	)
		add(	t,	"factor", 1, "Green",	{0, 1, 0, 1},		1	)
		add(	t,	"factor", 2, "Blue",	{0, 0, 1, 1},		1	)
		add(	t,	"factor", 3, "Grey",	{.5, .5, .5, 1},	1	)
		add(	t,	"factor", 4, "Alpha",	{1, 1, 1, 1},		1	)
		self.__color_factor = t
		y = y + ssy + DY*.5
		t = {}
		add(	t,	"offset", 0, "Red",		{1, 0, 0, 1},		1	)
		add(	t,	"offset", 1, "Green",	{0, 1, 0, 1},		1	)
		add(	t,	"offset", 2, "Blue",	{0, 0, 1, 1},		1	)
		add(	t,	"offset", 3, "Grey",	{.5, .5, .5, 1},	0	)
		add(	t,	"offset", 4, "Alpha",	{1, 1, 1, 1},		0	)
		self.__color_offset = t

		ssx = ssx * 2.

		--self:set_tab_key( "Top" )
		--y = to - SY
		--bu = bus:add_slider( "test", {	le+SXT*.25, y+SY*.5, SXT, SY } )

	bus:init_end()

	return bus
end

function BU_MONITOR:__define_ui_draw_top( name, sx, sy, ui )
	local bus = BUS:create( name.."_draw_top" )
	bus:init_begin()

		bus:set_constraint( "edge" )
		bus:set_bu_pos_load_save( false )

		local bu
		local my = sy*.05
		local SY = sy / 16.
		local DY = SY * .5
		local to = sy*.5 - my

		local mx = my
		local SX = SY
		local SXT = sx - 2*mx
		local le = -sx*.5 + mx
		local ri = sx*.5 - mx

		local y = to - SY*.5

		bu = bus:add_button(  "Draw Top",	{le+SY*.5,y,	SY,SY} )
			bu:set_target_lua( self, "__b_draw_top", false )
			ui.draw_top = bu
		y = y - SY
		bu = bus:add_slider( "top alpha", 	{le+SXT*.5,y,	SXT,SY} )
			bu:set_min_max( 0, 1 )
			bu:set_target_lua( self, "__top_alpha", .5 )
			bu:set_show_value(true)
			ui.top_alpha = bu
		y = y - SY - DY

		bu = bus:add_button(  "Grid",		{le+SY*.5,y,	SY,SY} )
			bu:set_target_lua( self, "__b_grid", false )
			ui.grid = bu
		y = y - SY
		bu = bus:add_slider( "grid_u", 		{le+SXT*.5,y,	SXT,SY} )
			bu:set_value_type_integer( true )
			bu:set_min_max( 2, 64 )
			bu:set_target_lua( self, "__grid_nb_u", 2 )
			bu:set_show_value(true)
			ui.grid_u = bu
		y = y - SY
		bu = bus:add_slider( "grid_v",		{le+SXT*.5,y,	SXT,SY} )
			bu:set_value_type_integer( true )
			bu:set_min_max( 2, 64 )
			bu:set_target_lua( self, "__grid_nb_v" ,2  )
			bu:set_show_value(true)
			ui.grid_v = bu
		y = y - SY - DY

		bu = bus:add_button(  "Center",		{le+SY*.5,y,	SY,SY} )
			bu:set_target_lua( self, "__b_center", false )
			ui.center = bu
		y = y - SY - DY

		bu = bus:add_button(  "Frame",		{le+SY*.5,y,	SY,SY} )
			bu:set_target_lua( self, "__b_frame", false )
			ui.frame = bu
		y = y - SY
		bu = bus:add_slider( "frame size",	{le+SXT*.5,y,	SXT,SY} )
			bu:set_min_max( 0, 1 )
			bu:set_value(.9)
			bu:set_target_lua( self, "__frame_size" )
			bu:set_show_value(true)
			ui.frame_size = bu
		y = y - SY - DY

	bus:init_end()

	return bus
end

function BU_MONITOR:set_render_mode_alpha( mode )
	--aaa.print_method()
	mode = mode % 3
	-- 0 No alpha, 1 Alpha, 2 Alpha Inverse
	self.__s_render_alpha = mode
	if mode == 0 then	self:set_color_back( { 0,.2,.2,	1 } )
	else				self:set_color_back( { 0,.4, 0,	1 } )
	end
	return self
end
function BU_MONITOR:inc_render_mode_alpha()
	self:set_render_mode_alpha( self.__s_render_alpha + 1 )
end
function BU_MONITOR:dec_render_mode_alpha()
	self:set_render_mode_alpha( self.__s_render_alpha - 1 )
end

--generalize to BU
--generalize to several
function BU_MONITOR:suscribe_to_click( suscriber )
	self.__intercept_click_dst = suscriber
end

function BU_MONITOR:get_gp_monitor_associated()
	local bind = self:get_texture_bind_2d()
	if bind then
		self:print( "we want to see tex bind "..bind )
		local gp = app:get_gp()
		local mon = gp:get_monitor_by_bind( bind )
		if mon then
			self:print( "found "..mon.." to see tex bind "..bind )
		else
			mon = gp:get_monitor( "Free" )
			if mon then
				mon:set_texture_bind_2d( bind )
			end
			--todo open only if close
			--todo only if the bind is an img valid valid
		end
		return mon
	end
end

function BU_MONITOR:do_click_double( x,y )
--	aaa.debug.print_traceback()
--	aaa.print_fn()
--	self:print( "y is "..y 
	
	if self.__b_fix then
		local mon = self:get_gp_monitor_associated()
		if mon then
			mon:set_window_state( "full" )
			mon:move_to_front()
		end
	else
		oo.getsuper(BU_MONITOR).do_click_double( self, x,y )
	end

	

-- if self.__intercept_click_dst then
-- 	self:print( "BU_MONITOR:do_click_double()" )
-- 	return true
-- end
-- oo.getsuper(BU_MONITOR).do_click_double( self, x,y )
end

function BU_MONITOR:do_click_up( x,y )
	if not self:is_mini() and not self:is_uif_running() then

		local b_used = false
		if aaa.keyboard.is_ctrl() then
			if self:is_ui_change_bind() then
		--		local monitor	=	self.__infos
		--		aaa.img.read( monitor.bind, aaa.img.get_bind_filename( monitor.bind ) )
				local nb = aaa.keyboard.is_shift() and 32 or 1
				if x < 0 then	self:__dec_texture_bind_2d_ui( nb )
				else			self:__inc_texture_bind_2d_ui( nb )
				end
				b_used = true
			end
		else
			if self.__intercept_click_dst then
				--self:print( x.." "..y )
				self.__intercept_click_dst:intercept_click( x,y )
				b_used = true
			end
			if not b_used then
				local lx,ly, sub_x,sub_y = self:__get_ui_top_zone( x,y )
				if lx and ly then
					b_used = true
					if lx==1 then
						if		ly==3		then
							self:set_info_draw( not self.__b_info_draw )
						elseif	ly==2		then
							self:set_info_draw( not self.__b_info_draw )
							self:set_size_draw( not self.__b_size_draw )
						else
							--self:print( "size_draw "..x.." / "..y.." "..self.__b_size_draw )
							self:set_size_draw( not self.__b_size_draw )
						end
					elseif lx==3 then
						if		ly==3		then
							--self:print( "sub_x is "..sub_x )
							local bu = (sub_x <.5) and self.__bu_more or self.__bu_draw_top
							if bu then
								bu:flip_active()
								if bu:is_active() then
									local x,y = bu:get_xy()
									bu:__set_rect_inertia( {x,y} )
								end
							end
						elseif	ly==2		then
							self:inc_render_mode_alpha()
						else
							self:set_draw_channel( not self.__b_channel_draw )
						end
					else
						b_used = false
					end
				end
			end
		end
		if b_used then
			self:clear_click_prev()
		end
	end
	--if true then return end
	oo.getsuper(BU_MONITOR).do_click_up( self, x,y )
--	self:__adjust_size( )
end

function BU_MONITOR:do_click_down( x,y )
	--aaa.debug.print_traceback()
	--if true then return end
	BU_MONITOR.__last_bind = self:get_texture_bind_2d()
	oo.getsuper(BU_MONITOR).do_click_down( self, x,y )
end

--[[
function BU_MONITOR:__set_helper( mode, alpha, out, level_use )
	local ui = self.__ui
	ui.mode:set_value(		mode 		)
	ui.alpha:set_value(		alpha 		)
	ui.out:set_value(		out 		)
	ui.level_use:set_value(	level_use 	)
	ui.invert:set_value(	0 			)
end
function BU_MONITOR:adapt_to_bind( bind )
	if not bind then return end

	local sx, sy, ch_nb = aaa.img.get_size_channel( bind )
	--we should resize here too
	if ch_nb then
		local ch_nb_last = self.__ch_nb_last
		if ch_nb_last ~= ch_nb then
			if ch_nb == 1 then
				--hack come from next function
				BU_MONITOR.__set_helper( self, 2, 0, 0, 0 )
			elseif ch_nb == 3 then
				BU_MONITOR.__set_helper( self, 0, 0, 0, 0 )
			elseif ch_nb == 4 then
				BU_MONITOR.__set_helper( self, 0, 1, 0, 0 )
			end
			self.__ch_nb_last = ch_nb
		end
	end
end
--]]
function BU_MONITOR:cpy_bu_flip( bu_src )
	local ui = self.__ui
	local src_ui = bu_src.__ui
	ui.flip_u = src_ui.flip_u
	ui.flip_v = src_ui.flip_v

	return self
end

function BU_MONITOR:__get_helper()
	--todonownow look strange: check

	--self:print( "__s_render_alpha "..src.__s_render_alpha )
	--aaa.print_fn()
	local ui = self.__ui
	if ui and ui.mode then
		return	ui.mode			:get_value()
				,self.__s_render_alpha
				,ui.out			:get_value()
				,ui.level_use	:get_value() + 1 + ui.invert:get_value_cano()*8 + ui.hsv:get_value_cano()*16
				,ui.orientation	:get_value()
				,ui.abs			:get_value()
	end

	return 0,self.__s_render_alpha,0,0,3,0
--	return 0,1,1,1,0,0
end

function BU_MONITOR:draw_tex_invalid( alpha )
	--self:draw_back_inactive()
	--gol.color( 1,0,0, alpha or 1 )
	--self:gol_draw_rect_plain()
	gol.color_blue( alpha or 1 )
	gol.set_line_width( 3 )
	aaa.draw_line( -.4,-.4, .4,.4 )
	--aaa.draw_mul_line( 0, 0, .1, .1 )
end

function BU_MONITOR:__draw_bind_spe( bind, le,bo, ri,to, alpha )
--	aaa.img.draw_lbz_size( bind, -.5,-.5,0, 1, false, true )++
--	if true then return end

	if not BU_MONITOR.__b_draw then return end
	--if true then return end

	--aaa.print_method()
	local bu_mon = self.__bu_mon_owner
	local alpha = bu_mon.__alpha_updated

	local sx = aaa.img.get_size( bind )
	if sx==nil or sx==0 then
		--todo draw_back have to much asymmetry
		--gol.color( .5,.25,.25, .5 * self:get_alpha_bu_to_draw() )
		--self:draw_rect_size()	
		if not bu_mon:is_mini() then
			bu_mon:draw_tex_invalid( alpha )
		end
		return
	end

	--if bind == 187 then
	--	aaa.print_fn()
	--end
	--BU.draw_bind( self, bind, le,bo, ri,to )
	--if true then return end

--[[
	self:print( "yehh "..self.mode )
	if not self.__bu_render_mode then
		BU.draw_bind( self, bind, le,bo, ri,to, alpha )
		return
	end
--]]

	--hack correct
--	BU_MONITOR.adapt_to_bind( self, bind )
	--table.print( self, "self in __draw_bind_spe()", 1 )

	local s_mode, s_alpha, s_out, s_level_use, s_ori, s_abs = bu_mon:__get_helper()

	-- if self:is_name( "Dif _image" ) then
 	-- 	self:print(  "mode "..s_mode.." alpha "..s_alpha.." out "..s_out.." level_use "..s_level_use.." ori "..s_ori.." abs "..s_abs )
	-- end
	
	--todo this is dirty, generalize : delegate ?
	--bu_mon.__s_alpha = s_alpha --pass it to BU_MONITOR

	local b
	--self:print( src )
	--if src then le = le - 2. end
	--todonownow too much here ?
--	b = src:is_flip_u() or self:is_flip_u()
--	self:print( "bu_mon is "..bu_mon )
	b = bu_mon:is_flip_u()
	if b then le, ri = ri, le end
	
--	b = src:is_flip_v() or self:is_flip_v()
	b = bu_mon:is_flip_v()
	if b then bo, to = to, bo end

	if s_alpha > 0 and not self.__b_fix then
		--todo why we can use the standard draw_back ?
		gol.set_texture_dim( 0 )
		self:gol_rgba_back( 0,.4,0,	alpha )
		self:draw_rect_size()
	end
	--self:print( s_alpha )
	if s_mode==0 and s_alpha==1 and s_out==0 and s_level_use==0 and s_abs==0 then
		--self:print( "s_alpha is "..s_alpha )
		--we process these cases this way to be faster
		if GA.b_spy then aaa.spy.push_range( "draw_bind", 3 ) end
		
			--if s_alpha == 0 then
			--	gol.disable_blend()
			--end
			--if s_alpha == 0 then gol.set_alpha_test( false ) end
			--self:print( "lbrt : "..le..","..bo.." "..ri..","..to )
			--gol.set_cull_back()
			--gol.set_front_line()
			-- if self:is_name( "f15_image" ) then
			-- 	self:print( "alpha : ".. self.__alpha_bu )
			-- end
			BU.draw_bind( self, bind, le,bo, ri,to, alpha )
--			if s_alpha == 0 then
--				gol.enable_blend()
--			end

		if GA.b_spy then aaa.spy.pop_range() end
	else
		if GA.b_spy then aaa.spy.push_range( "draw with layer", 3 ) end

			local ref = BU_MONITOR.ref
			local pset = param.set

			local ba, bi = aaa.img.make_bank_bind_2d( bind )
			pset( ref.bind_2d, bi )
			pset( ref.bank_2d, ba )

			if( s_ori == 1 or s_ori==3 )	then	pset(  ref.tex_left, 	le )	pset(  ref.tex_right,	ri )
			else									pset(  ref.tex_left, 	ri )	pset(  ref.tex_right,	le )
			end
			if( s_ori > 2 )					then	pset(  ref.tex_bottom,	bo )	pset(  ref.tex_top, 	to )
			else									pset(  ref.tex_bottom,	to )	pset(  ref.tex_top, 	bo )
			end

			local shading  = ref.shading
			--self:print( "toto "..bu_mon.red )

			local t = bu_mon.__color_factor
			if t and t.red then
				local grey = t.grey
				--self:print( grey )
				shading:set_frag_vec4( 2, t.red * grey, t.green * grey, t.blue * grey, t.alpha * alpha )
				t = bu_mon.__color_offset
				grey = t.grey
				shading:set_frag_vec4( 1, t.red * grey, t.green * grey, t.blue * grey, t.alpha * alpha )
				--shading:set_frag_vec4( 2, 1, 1, 1, 1 )
				--shading:set_frag_vec4( 1, 0, 0, 0, 0 )
			else
				shading:set_frag_vec4( 2, 1, 1, 1, alpha )
				shading:set_frag_vec4( 1, 0, 0, 0, 0 )
			end

			shading:set_frag_float_1_2( bu_mon.__level_min, bu_mon.__level_max )
			--todo self:print( "in draw_bind_monitor() "..bu_mon.__level_min.." / "..bu_mon.__level_max )

			--pass several b_info_size_on_top
			--self:print( s_alpha )
			local v = s_mode + 64*( s_alpha + 64*( s_out + 64*( s_level_use  + 64*s_abs ) ) )
			--self:print( v )
			--v  = 4
			local ch_nb = aaa.img.get_channel( bind )
			shading:set_frag_int_1_2( v, ch_nb )
			--shading:set_frag_int( 1, 20480 )
			--shading:set_frag_vec4( 2, 1, 1, 1, 1 )

			aaa.obj.update_then_draw( ref.layer_a )
			gol.set_default()

		if GA.b_spy then aaa.spy.pop_range() end
	--gol.set_texture_dim( 2 )
	--gol.bind_texture( bind )
	--gol.set_quad_uv( le, bo, ri, to )
	--self:gol_draw_rect_uv()
	end
end

function BU_MONITOR:create_more( name, rect, bus_down )
	local sx,sy = rect[3],rect[4]
	local s = math.min( sx,sy ) * .5
	local bu = BU:create_window_center( name, rect, bus_down )
		s = s * .2
		bu:set_pos_mini( {rect[1] - s*.5,rect[2] - s*.5, s,s} )

		bu:set_text_color( 0, 1, 1, .5 )
		bu:set_border_line_color( {0, 1, 1, .5} )
		bu:set_back_alpha( .35 )
	return bu
end
function BU_MONITOR:define_monitor( name, rect, bind, b_ui_more )
	local bus = BUS:create( name )

	bus:init_begin()
		bus:set_bu_pos_load_save( false )
		bus:set_constraint( "edge" )
		bus:set_transfo( rect[3],rect[4] )

		if b_ui_more then
			local function add_window( method_define, name, x )
				local SX,SY = .2,.5
				local bus_down = self[method_define]( self, name, SX,SY, self.__ui )
				local fx,fy = self:get_sxy()
				local bu = self:create_more( name, {x,0, SX*fy,SY*fy}, bus_down )
				bus_down:set_transfo( SX,SY )
				bu:set_active( false )
				bus:add_bu( bu )
				bu:set_dplane(42)
				return bu
			end
			self.__bu_more		= add_window( "__define_ui_more", 		"More",		.15	)
			self.__bu_draw_top	= add_window( "__define_ui_draw_top",	"Draw Top",	.3	)
		end

		local bu = bus:add_image( name.."_image", rect, bind )
			bu:set_text_visible(	false )
			bu:set_mobile(			false )
			bu:set_ui_active(		false )
			bu:set_border(			false )
			bu:set_border_line(		false )

			bu:set_back_alpha(0)
			
			--hack the draw is done by a bu_image under
			bu.__bu_mon_owner	=	self
			--todonownow this was not such a bad idea
			--bu.__ui				=	self.__ui
			--encapsulation in function make us able to proppagate change in __draw_bind_spe() on the fly
			bu.draw_bind		=	function(...) return BU_MONITOR.__draw_bind_spe( ... ) end
			bu:set_dplane(-42)
		self.__bu_image = bu

	bus:init_end()

	return bus
end

function BU_MONITOR:set_text( text, text_size_factor )
	if self.__infos then	-- __infos is not defined yet when called from BU_MONITOR:create_instance
		self.__infos.title = text
	end
	return oo.getsuper(BU_MONITOR).set_text( self, text, text_size_factor )
end

function BU_MONITOR:create( name, rect, bind, b_fix )
	if b_fix == nil then
		b_fix = false
	end
	
	local b_store_bind
	if type(bind)=="string" then
		bind = TEXS:get_bind_by_name(bind)
		b_store_bind = true
	else
		if bind then
			b_store_bind = true
		else
			bind = TEXS:get_bind_by_name("PB")
		end
	end
	--aaa.print_fn()

	--aaa.print_fn()
	local self = BU_MONITOR:create_instance( name, rect )
	if b_store_bind then
		self.__bind_at_creation = bind
	end
	if b_fix then
		self:set_sxy_min( .01,.01 )
	end
	self.__infos = {}
	local infos = self.__infos
	infos.title	= name
	--monitor.bind_last	= -1
	--self:print( "monitor asked ".. pos.sx.." x ".. pos.sy )
	--infos.sx	= rect[3]	--todo unclear we store pixel size here
	--infos.sy	= rect[4]
	self.__ui = {}

	self.__s_render_alpha = 1
	--table.print({0,0, pos.sx,pos.sy}, "pos" )
	rect[1] = 0
	rect[2] = 0
	local bus_down = self:define_monitor( name, rect, bind, not b_fix )
	self:transform_in_window( bus_down )

	self.__b_fix = b_fix
--	self:set_mobile( not b_fix )
	self:set_mobile( false )

	self:set_ratio_max( b_fix and 4 or 5 )
	self:set_method_on_click_double( self, "do_click_double" )
	self:set_render_mode_alpha( b_fix and 0 or 1 )	-- compensate for the fact that moving monitor don;t change back color with chamge of the BU controling their alpha mode
	--self:set_render_mode_alpha( b_fix and 1 or 0 )
	--self:set_back_alpha( b_fix and 1 or 0 )
	self:set_text_draw( not b_fix )
	--border
	self:set_border( not b_fix )
	self:set_border_line( not b_fix )
	self:set_border_line_color( b_fix and {0,.5,.5, .5} or { 0,1,1, .5} )
	--ui
	self:set_ui_active( not b_fix )
	if b_fix then
		--ui
		self:set_ui_top( false )
		
		self:set_draw_channel( true )
	else
		--text
		self:set_text( name, .1 )
		self:set_text_xy( .1,.1 )
		self:set_text_color( {0,1,1, 1} )
		--ui
		self:set_ui_top_size( true )

		self:set_value_load_save( true )
	end
	self:set_texture_bind_2d( bind )

	return self
end

function BU_MONITOR:add_buttons_below( meu )
	local ui = self.__ui
	local l,b, sx,sy = self:get_lb_sxy_cur()
	local bu
	sy = sx / 16
	b = b - sy
	local y = b+sy/2
	local sxb = sy
	meu:begin_bu_group( self:get_name() )
		bu = bus_cur:add_button( "Monitor", {l+sxb/2,y, sxb,sy} )
	--		:set_text( "Monitor")
			--:set_text_inside(false)
			:set_text_rect_ratio(3)
			:set_value( true )
		ui.active = bu
		sxb = sy * 3
		bu = bus_cur:add_trig(	"Center", {l+sy*6,y,	 sxb,sy} )
			bu:set_method_on_click( self, "center_target_monitor", bu )
		
		ui.flip_u = bus_cur:add_button( "Flip U", {l+sy*9,y,  sy} ):set_text_rect_ratio(3):set_value( false )
		ui.flip_v = bus_cur:add_button( "Flip V", {l+sy*13,y, sy} ):set_text_rect_ratio(3):set_value( false )
	--	self:set_bu_flip( bu_u, bu_v )
	meu:end_bu_group()

	local gp = GP.cur
	if gp then
		gp:__register_monitor_with_button( meu, self )
	end
	return self
end

function BU_MONITOR:__get_bu_mon_buttons()
	local bu_mon
	if self.__b_gp_mon then
		--self:print( "BU_MONITOR:is_active()" )
		local name = self:get_name()
		local gp = GP.cur
		if gp then
			bu_mon = gp:__get_monitor_with_button( name )
			-- if bu_mon then 
			-- 	--self:print( "                      got "..bu_mon )
			-- 	if string.lower(name) == "f2" then
			-- 		for key,bu in PAIRS(bu_mon.__ui) do
			-- 			self:print( key.." "..bu:get_value() )
			-- 		end
			-- 	--	table.print( bu_mon.__ui, "bu_mon", 2 )
			-- 	end
			-- end
		end
	end
	return bu_mon
end
function BU_MONITOR:__do_delegate_is( key )
	local bu_mon = self:__get_bu_mon_buttons()
	if not bu_mon and key ~= "active" then
		bu_mon = self
	end
	if bu_mon then
		--self:print( "key is "..key )
		--table.print( bu_mon.__ui, "__ui" )
		local bu = bu_mon.__ui[key]
		return bu and bu:get_value_as_bool()
	end
end
function BU_MONITOR:is_active()
	--aaa.print_fn()
	local b = self:__do_delegate_is( "active" )
	if b~=nil then return b else return oo.getsuper(BU_MONITOR).is_active(self) end
end
function BU_MONITOR:is_visible()
	local b = self:__do_delegate_is( "active" )
	if b~=nil then return b else return oo.getsuper(BU_MONITOR).is_visible(self) end
end
--todonownow merge with set_texture_flip_u and monitor dialog 
function BU_MONITOR:is_flip_u()
	local b = self:__do_delegate_is( "flip_u" )
	if b~=nil then return b else return false end
end
function BU_MONITOR:is_flip_v()
	local b = self:__do_delegate_is( "flip_v" )
	if b~=nil then return b else return false end
end

function BU_MONITOR:center_target_monitor( bu )
	if bu then
		local bu_mon = self:get_gp_monitor_associated()	--bu.__bu_monitor_target
		if bu_mon then
			bu_mon:set_window_state( "normal" )
			bu_mon:set_xy( 0,0 )
		else
			self:print_error( "no __bu_monitor_target field" )
		end
	end
end

if false then
	local meu = mu:get_meu()
		bu_mon.__bu_active_master = meu:get_bu_by_key( "monitor active" )
		if not bu_mon.__bu_active_master then
			self:box_error( "No __bu_active_master" )
		end
		bu_mon.__bu_flip_u = meu:get_bu_by_key( "Flip U" )
		bu_mon.__bu_flip_v = meu:get_bu_by_key( "Flip V" )

		local bu_center = meu:get_bu_by_key( "center" )
		bu_center.__bu_monitor_target = bu_mon
		--table.print( bu_center, "bu_center", 3 )
		--self:box_error( bu_center.." center use "..bu_mon )

		--replace is_active of monitor itself by state of this button
		bu_mon.is_active	= monitor_is_active
		bu_mon.is_visible	= monitor_is_active
end