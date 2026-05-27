
--hack
local	b_no_rewind = true


function BU:set_draw_lua( b )	self.__b_draw_lua = b			return self		end
function BU:is_draw_lua()		return self.__b_draw_lua		end

function BU:set_draw_min( b )	self.__b_draw_min = b			return self		end
function BU:is_draw_min()		return self.__b_draw_min		end

--function BU:set_draw_skip_for_speed(b)	self.__b_draw_skip_for_speed = b		return self		end
--function BU:is_draw_skip_for_speed()		return self.__b_draw_skip_for_speed		end

function BU:set_gdebug( b )		self.__b_gdebug = b 	return self 	end
function BU:is_gdebug()			return self.__b_gdebug	end

function BU:define_gdebug_ui( rect, name )
	local b_window_call = false
	local bus
	local S = .175
	if b_window_call then
		bus = BUS:begin_window( name )
	else
		bus = BUS:create( name )
		bus:init_begin()

		local side = S/2
		bus:add_bup( {side, side, (rect.sx-side)*2, rect.sy*2-side*2} ):set_sxy_col( S,S, 5 )
	end
	bus:set_bu_pos_load_save( true )
	bus:set_fname( "AAA_Bu" )

	local function add_blt( name, tab, key, b )
		local bu = bus:add_but_target_lua( name, nil, tab, key, b )
			:set_gdebug( false )
			:set_pos_load_save( false )
			:set_text_rect_ratio(5)
		return bu
	end
	local function add_slider( name, tab, key, val, min,max )
		local bu = bus:add_slider_target( name, nil, tab,key, val, min,max 	)
			:set_gdebug( false )
			:set_pos_load_save( false )
			:set_show_value( true )
		return bu
	end

	local DX_INDENT = .6
	local DY_INDENT = .75
	local DY = DY_INDENT * .5
	local bup = bus:get_bup()
	bup:move_y( .25 )
	--todo	move the b_gdebug to bus too
	local bu
		bu = add_blt(	"Draw Regular",			self,	"__b_draw_regular_ui",			true	)	--todo 2023 July do not work anymore
		bu = add_blt(	"Draw in Lua",			self,	"__b_draw_lua"					)
		bu = add_blt(	"Coordonate",			BUS,	"__b_gdebug_coor",				false	)
		bup:move_y( DY_INDENT )

		bu = add_blt(	"Text",					TEXT,	"__b_active",					true	)
			bu:set_text_draw_always( true )

			bup:move_x( DX_INDENT )
			bu = bus:add_selector_target_lua( "Text Nice", {nil,nil,4, nil},	BU,	"__s_text_type", 1	)
				:set_nb( 3,1 )
				:set_item_text( 1, "Glut", "Maa", "Nice" )
				:set_text_draw( false )
				:set_pos_load_save( false )

			bu = add_slider( "TEXT Line width",	self,	"__draw_text_line_width",		2,		1,6	):set_text( "Line Width" )
				:add_values_def( 2,3,4,5 )
		--bup:set_col_nb( col_nb )
			bup:move_x( -DX_INDENT )
		bu = add_blt( 	"Text Rect", 			GA, 	"__b_visu_text_draw_rect",		false	)
		bup:move_y( DY_INDENT )

		bu = add_blt( 	"Draw 3D", 				GA, 	"__b_3d",						false	)
		bu = add_blt(   "Texture Custom", 		GA, 	"__b_tex_draw_custom",			false	)
		bu = add_blt( 	"Main Rect Only", 		self,	"__b_inside_main_rect_only",	false	)
		bu = add_blt( 	"Draw all", 			self,	"__b_draw_all_ui_inv",			true	)
			bu:set_trig()	--tmp try to solve Fred remark (careful the bool is inverted)
			--todo can't change at this stage 
			--bu:set_x( BU:get_x() + 2*S )
			--bu:set_sx( 4*S )
			--bu:set_text_inside( true )
			bus:move_x( DX_INDENT )
			bu = add_blt( 	"draw Before", 		self,	"__b_draw_before_ui",			true	):set_text( "Before" )
			bu = add_blt( 	"draw Back", 		self,	"__b_draw_back_ui",				true	):set_text( "Back" )
			bu = add_blt( 	"Draw", 			self,	"__b_draw_ui_inv",				true	)
				bu:set_trig()	--tmp try to solve Fred remark (careful the bool is inverted)
			bu = add_blt( 	"draw Fore", 		self,	"__b_draw_fore_ui",				true	):set_text( "Fore" )
			bu = add_blt( 	"draw After", 		self,	"__b_draw_after_ui",			true	):set_text( "After" )
			bus:move_x( -DX_INDENT )

		--bus:move_next_col()
		bup:move_y( 0.5 )
		bu = add_blt(		"gdebug",					self,	"__b_gdebug",						false	):set_text( "Graphic Debug" )
		bup:move_x( DX_INDENT )
		--col_nb = bup:get_col_nb()
		--bup:set_col_nb( col_nb - DX_INDENT )
			bu = add_blt(	"Finish by BU",				self,	"__b_gdebug_finish_by_bu",			false	):set_text( "Finish by BU" )
			bu = add_blt(	"gdebug Plain",				self,	"__b_gdebug_draw_plain",			false	):set_text( "Plain" )
				bup:move_x( DX_INDENT )
				bu = add_slider("gdebug Plain Alpha",	self,	"__gdebug_plain_alpha",				.125,	0,1):set_text( "Alpha" )
				bup:move_x( -DX_INDENT )
			bu = add_blt(	"gdebug Border",			self,	"__b_gdebug_draw_line",				true	):set_text( "Border" )
				bup:move_x( DX_INDENT )
				bu = add_slider("gdebug Line Alpha",	self,	"__gdebug_line_alpha",				1,		0,1):set_text( "Alpha" )
				bup:move_x( -DX_INDENT )
			bus:move_y( DY )
			bu = add_blt(	"gdebug Class",				self,	"__b_gdebug_draw_class",			false	):set_text( "Class" )
			bu = add_blt(	"gdebug Name",				self,	"__b_gdebug_draw_name",				false	):set_text( "Name" )
			bu = add_blt(	"gdebug Contact Nb",		self,	"__b_gdebug_draw_contact_nb",		false	):set_text( "Contact Nb" )
			bus:move_y( DY )
			bu = add_blt(	"gdebug Link",				self,	"__b_gdebug_draw_link",				false	):set_text( "Link" )
		--	bu = add_blt(	"link down",				self,	"__b_gdebug_draw_link_down",		false	)
			bu = add_blt(	"gdebug Invisible",			self,	"__b_gdebug_draw_invisible",		false	):set_text( "Invisible" )
			bu = add_blt(	"gdebug Inactive_ui",		self,	"__b_gdebug_draw_inactive_ui",		false	):set_text( "Inactive_ui" )
			bu = add_blt(	"gdebug Inactive",			self,	"__b_gdebug_draw_inactive",			false	):set_text( "Inactive" )

			bus:move_y( DY )
			bu = add_blt(	"gdebug Focus",				self,	"__b_gdebug_bu_contact_set_focus",	false	):set_text( "Focus" )
			bu = add_blt(	"gdebug Dump",				self,	"__b_gdebug_dump",					false	):set_text( "Dump" )
				bup:move_x( DX_INDENT )
				bu = add_slider("gdebug Dump Level",	self,	"__gdebug_dump_level",				4,	0,8	):set_text( "Dump Level" )
					:set_value_type_integer( true )
				bup:move_x( -DX_INDENT )
			bup:move_x( -DX_INDENT )

		--bup:move_next()

	if b_window_call then
		bus:end_window()
	else
		bus:init_end()
	end

	return bus
end
function BU:__update_draw_stuff()
	self.__b_draw_regular	= self.__b_draw_regular_ui -- or not self.__b_gdebug
	self.__b_draw_before	= self.__b_draw_before_ui  -- and not self.__b_gdebug
	self.__b_draw_back		= self.__b_draw_back_ui    -- and not self.__b_gdebug
	self.__b_draw_fore		= self.__b_draw_fore_ui    -- and not self.__b_gdebug
	self.__b_draw_after		= self.__b_draw_after_ui   -- and not self.__b_gdebug
end
function BU:draw_3d_on_top( l,b, r,t  )
	local sy = t-b
	local _,_,sz = self:get_size_draw()
	sz = sz * sy
	aaa.draw_box_top_line( r-l, sy, sz,	(l+r)*.5, 0, sz*.5 )
end
function BU:draw_3d_on_top_size( size )
	size = size or 1
	local _,_,sz = self:get_size_draw()
	sz = sz * size
	aaa.draw_box_top_line( size, size, sz,	0, 0, sz*.5 )
end
function BU:draw_rect_uv_lbrt( l,b, r,t  )
	aaa.draw_rect_uv( l,b, r,t )
	--aaa.draw_rect_uv_axe_z( (l+r)/2,(b+t)/2,0 ,r-l,t-b )
	--todo3d
	if ga:is_3d() then self:draw_3d_on_top( l, b, r, t  ) end
end
function BU:draw_rect_uv_size( size )
	aaa.draw_rect_uv_size( size )
	--aaa.draw_rect_uv_axe_z( (l+r)/2,(b+t)/2,0 ,r-l,t-b )
	--todo3d
	if ga:is_3d() then self:draw_3d_on_top_size( size  ) end
end
function BU:draw_rect_lbrt( l,b, r,t )
	aaa.draw_rect( l,b, r,t )
	--todo3d
	if ga:is_3d() then self:draw_3d_on_top( l,b, r,t  ) end
end
function BU:draw_rect_array_lbrt( a )
	aaa.draw_rect( a[1],a[2], a[3],a[4] )
	--todo3d
	if ga:is_3d() then self:draw_3d_on_top( a[1],a[2], a[3],a[4] ) end
end
function BU:draw_rect_size( size )
	if size then 
		aaa.draw_rect_size( size )
	else
		aaa.draw_rect_cano()
	end
	--todo3d
	if ga:is_3d() then
		self:draw_3d_on_top_size( size )
	end
end

function BU:__draw_gdebug_color( alpha )
	--	SET COLOR
	if self:is_active() then
		if self:is_visible() then
			if self:is_contact() then	gol.color_green( alpha )
			else						gol.color_cyan ( alpha )
			end
		else
			if self:is_contact() then	gol.color_green( alpha )
			else						gol.color_blue ( alpha )
			end
		end
	else
		if self:is_visible() then		gol.color_magenta( alpha )
		else							gol.color_red    ( alpha )
		end
	end
end

function BU:__draw_gdebug_begin()
	gol.set_default()
	gol.set_line_width( 1 )
--	gol.set_blend_add()
--	gol.set_blend_max()
end

function BU:__draw_gdebug_plain( s )
	s = s or .95
--	if not self:is_ui_active() and not self.b_gdebug_draw_inactive_ui then return end

	self:__draw_gdebug_begin()
	--	PLAIN
	self:__draw_gdebug_color( self.__gdebug_plain_alpha )
	self:gol_draw_rect_plain_size( s )
end

function BU:__draw_gdebug_after()
	if not self:is_ui_active() and not self.__b_gdebug_draw_inactive_ui then return end

	self:__draw_gdebug_begin()
	self:__draw_gdebug_color( self.__gdebug_line_alpha )

	--	LINE
	if self.__b_gdebug_draw_line then
		if self:is_active() then
			if self:is_contact() then
				self:gol_draw_rect_line_size( 1.02 )
			else
				self:gol_draw_rect_line()
			end
		else
			self:gol_draw_octo_line( .45 )
		end
	end
	--	CLASS
	if self.__b_gdebug_draw_class or self.__b_gdebug_draw_name then
		local str = self.__b_gdebug_draw_class and self:get_class_name() or nil
		str = self.__b_gdebug_draw_name and aaa.__build_table_str( self ) or str
		if str then
			gol.color_cyan()
			self:draw_text_lrbt( str, -.5,4.5, .3,.5 )
		end
	end
	if self.__b_gdebug_draw_contact_nb then
		local nb = self:get_contact_nb()
		if nb > 0 then
			self:draw_text_lrbt( tostring(nb), -.5,.5, .5,1 )
		end
	end
end

function BU:__draw_gdebug_link()
	if not self:is_ui_active() and not self.__b_gdebug_draw_inactive_ui then return end

	--	LINK
	--aaa.show( self.__b_gdebug_draw_link, "__b_gdebug_draw_link" )
	if not self.__b_gdebug_draw_link then return end
	local up = self:get_up()
	if not up then return end

	-- Draw up
	self:__draw_gdebug_begin()

	local a = self:is_active()
	if a then
		gol.color_cyan( .5 )
	else
		if self.__b_gdebug_draw_inactive then
			gol.color_red( .5 )
			a = true
		end
	end
	if a then
		local x,y = self:get_xy()
		aaa.draw_line( 0,0, x,y )
	end
end

--	unused
function BU:__draw_gdebug_link_down()
	if not self:is_ui_active() and not self.__b_gdebug_draw_inactive_ui then return end

	--	LINK
	if not self.__b_gdebug_draw_link_down then return end
	local bus_down = self:get_bus_down()
	if not bus_down then return end

	-- Draw down
	self:__draw_gdebug_begin()

	for i = 1, bus_down:get_bu_nb() do
		local bu = bus_down:get_bu( i )
		local a = bu:is_active()
		if a then
			gol.color_cyan( .5 )
		else
			if self.__b_gdebug_draw_inactive then
				gol.color_red( .5 )
				a = true
			end
		end
		if a then
			local x, y = bu:get_xy()
			x, y = bus_down:do_transfo_inverse( x, y )
			aaa.draw_line( 0, 0, x, y )
		end
	end
end


--
--	TEX
--
-- this the old stuff implemented to have image and video still in use but rewrote un 2024 August 27th
function BU:set_img( img )
	if img then
		--self:print_debug( "BU:set_img( "..img )
		local tex = self:get_table_always( "__texture" )
		tex.img = img
		tex.bind = nil
	else
		local tex = self:get_table( "__texture" )
		if tex then
			tex.img = nil
			tex.bind = nil
		end
	end
end
function BU:set_texture_bind_2d( bind )
	if bind then
		local tex = self:get_table_always( "__texture" )
		tex.img = nil
		tex.bind = bind
	else
		local tex = self:get_table( "__texture" )
		if tex then
			tex.img = nil
			tex.bind = nil
		end
	end
end
function BU:get_texture_bind_2d()
	local t = self:get_table( "__texture" )
	if not t then return end

	local bind = t.bind
	if bind then return bind end

	local img = t.img
	-- try to recussitate Vulcania APP
	-- if type(img) ~= "table" then
	-- 	aaa.show( img, "Hack binded" )
	-- 	--table.print( t, "Maa we got a problem", 2 )
	-- 	bind = img
	-- else
		bind = img and img:get_bind() or nil
	--end
	return bind
end
function BU:set_texture_bind_on(bind)
	local tex = self:get_table_always( "__texture" )
	tex.bind_on = bind
end
function BU:get_texture_bind_on()
	local tex = self:get_table( "__texture" )
	return tex and tex.bind_on
end

function BU:set_tex_draw( b )			self.__b_tex_draw = b			end
function BU:is_tex_draw()
	local tex = self.__texture
	return (tex and (tex.bind or tex.img)) or self.__b_tex_draw
end

function BU:get_texture_bind_to_draw()
	local bind
	if self:is_contact() then
		bind = self:get_texture_bind_on()
		if bind then
			return bind
		end
	end
	bind = self:get_texture_bind_2d()
	--self:print( "tex_bind "..self.__img )
	return bind
end
function BU:set_texture_flip_u( b )
	local tex = self:get_table_always( "__texture" )
	tex.b_flip_u = b
end
function BU:set_texture_flip_v( b )
	local tex = self:get_table_always( "__texture" )
	tex.b_flip_v = b
end
function BU:set_texture_lbtr( lbrt )
	local tex = self:get_table_always( "__texture" )	
	tex.lbrt = lbrt
end

function BU:__draw_frame_highlight( lbrt )
	gol.set_line_width( BU.__draw_text_line_width * 1.5 )
	if lbrt then
		aaa.draw_rect_line(	unpack(lbrt) )
	else
		self:gol_draw_rect_line()
	end
end
function BU:__set_color_contact_highlight()
	gol.color_green()
end
function BU:__draw_contact_highlight( lbrt )
	self:__set_color_contact_highlight()
	self:__draw_frame_highlight( lbrt )	
end
function BU:__set_color_bu_cur()
	gol.color_magenta()
end
function BU:__draw_bu_cur( lbrt )
	self:__set_color_bu_cur()
	self:__draw_frame_highlight( lbrt )	
end
function BU:__set_color_highlight()
	local b
	if		self:is_contact()	then	b = true 	self:__set_color_contact_highlight()
	elseif	self:is_bu_cur()	then	b = true	self:__set_color_bu_cur()
	else								b = false
	end
	return b
end
function BU:__draw_highlight( lbrt )
	if		self:is_contact()	then	self:__draw_contact_highlight( lbrt )
	elseif	self:is_bu_cur()	then	self:__draw_bu_cur( lbrt )
	end
end
function BU:__draw_text_on_top( alpha )
	--local b_lua = self:is_draw_lua()
	--if b_lua then

	local text = self:get_text()
	--todo fix this : unify
	if self:is_contact() then
		gol.color_green(alpha)
	else
		self:draw_text_color()
		--gol.color_white(alpha)
	end

	if self:is_mini() then
		--was this but two small for window with long name
		--self:draw_text_lrbt( text, -.5, 2.5, -.5, .5 )
		self:draw_text_lrbt( text, -.5, 12, -.5, .5 )
	else
		--was this but two small for window with long name
		--self:draw_text_lrbt( text, -.5, .5, -.5, .5 )
		self:draw_text_lrbt( text, -.5, 8., -.5, .5 )	-- was displaying big parasite text on Mac
	end
end

function BU:__set_draw_marked( b )
	self.__b_marked = b
end
function BU:draw_marked( lbrt )
	local ph = aaa.time.t*2. + self.__bu_id * .25
	ph = triangle_01( ph )
	--local sy = math.sin(aaa.time.t*10.)*.15+.35
	--gol.draw_triangles_2d( -.4,0, -.6,sy, -.6,-sy )
	--gol.draw_triangles_2d( .4,0, .6,-sy, .6,sy )
	gol.set_line_width( BU.__draw_text_line_width * 2  )
	local fn_color = gol.color_red
	--fn_color( .75 + ph * .25 )
	--aaa.draw_rect_line( -i,-i, i,i )
	--fn_color( .5 + ph * .5 )
	fn_color()
	
	local x = -.5 + ph
	--gol.draw_lines_2d( x,-.5, x,.5 )
	gol.draw_lines_2d( -.5,x, .5,x )
	-- local s = .5 - ph * .5
	-- local i = 0.5
	-- gol.draw_lines_2d( -s,-s, -i,-i,
	-- 					-s,s, -i,i,
	-- 					s,-s, i,-i,
	-- 					s,s, i,i )

end
function BU:draw_fore( lbrt )
	--todo add global flag
	self:__draw_highlight(lbrt)
	self:draw_led(lbrt)
	if self.__b_marked then
		self:draw_marked( lbrt )
	end

	--if self:is_text_draw() then
	--	self:__draw_text_on_top( 1 )	
	--end
end

BU.doc.__draw_all = "() this is the complete draw method for all BU."
function BU:__draw_all()
	--if true then return end
	--self:print( "BU:__draw_all()" )

	local b_gdebug = self:is_gdebug()
	local b_regular = self.__b_draw_regular or not b_gdebug

	local fn_table = b_regular and self.__fn_table
--BEFORE
	if fn_table and BU.__b_draw_before then self:do_fn( "draw_before" ) end

--BACK
	if b_regular then
		if BU.__b_draw_back then self:draw_back() end
	end
	if b_gdebug and self.__b_gdebug_draw_plain then
		--	if self.__b_gdebug_draw_plain then
		self:__draw_gdebug_plain()
	end


--CENTRAL
	if b_regular then --todo 2023 July do not work anymore
		if fn_table and self:do_fn( "draw_custom" ) then
		else
			local obj = self.obj
			if obj then	--	there is an object attached to this BU
				self:draw_obj()
			end
			if not BU.__b_draw_ui_inv then
				self:draw()
			end
		end
	end


--DOWN
	local bus_down = self:get_bus_down()
	if bus_down and (not self:is_mini() or self.__b_draw_inside_mini) then
		if bus_down:is_active() then
			bus_down:draw( false )
		end
	end

--FORE
	if b_regular then
		if BU.__b_draw_fore then self:draw_fore() end
	end

--AFTER
	if fn_table and BU.__b_draw_after then self:do_fn( "draw_after" ) end



	if self:is_ui_top() and aaa.keyboard.is_alt_only() then
		self:__draw_ui_top()
	end

end

function BU:__transform_and_draw_gdebug()
	--was GABU.transfo_push(pos.x + self.bus.__x_offset_exp, pos.y + self.bus.__y_offset_exp, sx, sy,  )
	self:__draw_gdebug_link()

	self:push_transfo()
		if self.__b_gdebug_draw_plain then
			self:__draw_gdebug_plain()
		end
		self:__draw_gdebug_after()
	self:pop_transfo()
end
-- this where draw is done 
function BU:__transform_and_draw()
	--if true then return end
	--table.print( self, self )
	self.__draw_ui_render_index = BU.__render_index_cur

	if self.verbose >= 3 then aaa.print_method() end

	local b_gdebug = self:is_gdebug()
	if b_gdebug then	--and self.__b_gdebug_draw_before then
		self:__draw_gdebug_link()
	end

	if not self.__b_draw_all_ui_inv then
		if GA.b_spy then aaa.spy.push_range( self..". tr_dr()", 2 ) end
		--was GABU.transfo_push(pos.x + self.bus.__x_offset_exp, pos.y + self.bus.__y_offset_exp, sx, sy,  )
		self:push_transfo()
			--gol.translate( math.sin( aaa.time._real * 10.) * .1 , 0 )
				--gol.scale( .9, .8 )
				--gol.translate( 0, -.2 )
			--if BUS.get_stack_size() > 3 then return end
			--self:print( "BUS stack level ------------------ "..BUS.get_stack_size() )
			--self:print( "debugging" )
			self:__draw_all()
			if b_gdebug then
				self:__draw_gdebug_after()
			end
		self:pop_transfo()
		if GA.b_spy then aaa.spy.pop_range() end
	end
end

--	GOL
--
--[[
function BU:gol_begin()
--	gol.set_depth( false )
--	gol.color_white()
end
function BU:gol_end()
end
--]]

--
--  ALPHA
--
--todo deal with alpha really
--todo check back_alpha
function BU:set_alpha_bu( a )		self.__alpha_bu = (type(a)=="boolean") and (a and 1 or .4) or a		return self	end
function BU:get_alpha_bu()			return self.__alpha_bu								end
function BU:get_alpha_bu_to_draw()	return self.__alpha_bu * BU.__class_alpha_factor	end

function BU:interpolate_alpha_bu( a, speed )
	if type(a)=="boolean" then
		a = a and 1 or .4
	end
	--self:print( "a is "..a) 
	a = interpolate( self.__alpha_bu, a, aaa.time.dt_real * (speed or 3) )
	self.__alpha_bu = a
	return a
end

--
--	BACK and COLOR
--
--todo refine back strategy
function BU:set_back_alpha( a )						self.__back_alpha = a				end

function BU:__set_color_back( color )				self.__color_back = color			end
function BU:__set_color_back_named( name )			self.__color_back = self:get_color_back_named( name )			return self	end
function BU:set_color_back_named_no_error( name )	self.__color_back = self:get_color_back_named_no_error( name ) 	return self	end

function BU:push_color_back()
	self.__pushed_color_back = pack( self:get_color_back() )
end
function BU:pop_color_back()
	self:set_color_back( self.__pushed_color_back )
	self.__pushed_color_back = nil
end
function BU:push_color_back_busy()
	self:push_color_back()
	--aaa.print_fn()
	self:__set_color_back_named( "busy" )
end


function BU:set_color_back_lua()					self:__set_color_back_named( "lua" )							return self end
function BU:set_color_back_edit()					self:__set_color_back_named( "edit" )							return self end

function BU:set_color_back( color )
	if color then
		if type(color) == "string" then self:__set_color_back_named( color )
		else							self.__color_back = color
		end
	else
		--aaa.print_fn()
		self:__set_color_back_named( self:get_text() )
	end
	return self
end

function BU:get_color_back()
	local col = self.__color_back
	local alpha = self.__alpha_bu * self.__back_alpha * BU.__class_alpha_factor
	if col then
		return col[1],col[2],col[3], col[4]*alpha
	else
		return 1,0,0, .8*alpha
	end
end
function BU:gol_rgba_back( r,g,b, a )
	gol.color( r,g,b, a * self.__alpha_bu * self.__back_alpha * BU.__class_alpha_factor )
end
function BU:gol_red_back()
	gol.color( 1,0,0, .25 * self.__alpha_bu * self.__back_alpha * BU.__class_alpha_factor )
end
function BU:gol_green_back()
	gol.color( 0,1,0, .25 * self.__alpha_bu * self.__back_alpha * BU.__class_alpha_factor )
end

function BU:gol_color_back()
	--gol.color_white()
	gol.color( self:get_color_back() )
end

function BU:gol_draw_rect_back()
	self:gol_color_back()
	self:draw_rect_size()
end

function BU:set_back_bind( bind )
	self.__back_bind = bind
end
function BU:draw_back_bind_only()
	local back_alpha = self.__back_alpha
	if back_alpha > 0 then		
		--self:print( "back" )
		local back_bind = self.__back_bind
		if back_bind then
			self:draw_bind_full( back_bind, back_alpha )
		end
	end
end
function BU:draw_back()
	local back_alpha = self.__back_alpha
	if back_alpha > 0 then		
		--self:print( "back" )
		local back_bind = self.__back_bind
		if back_bind then
			--gol.color_white( back_alpha )
			--aaa.draw_bind_rect( self.__back_bind, -.5,-sy, sx,sy )
			self:draw_bind_full( back_bind, back_alpha )
		else
			self:gol_color_back()
			self:gol_draw_rect_plain()
		end
	end
end
function BU:draw_back_inactive( val )
	gol.color( .5,0,0, (.8+ math.sin(aaa.time.t*math.pi2)*.1) * (val or 1) )
	--self:print( "back_inactive" )
	self:gol_draw_rect_plain()
end

function BU:__draw_back_by_value( b_equal )
	local draw_by_value = self.__draw_by_value
	local b_mode
	if draw_by_value then
		b_mode = draw_by_value.mode
		if b_equal == nil then
			local i_sel = self:get_value()
			b_equal = i_sel == draw_by_value.value
		end
	else
		b_mode = "true"
	end

	local col
	if		b_mode == "false"	then
		col = BU:get_color_back_named( b_equal and "bu_off" or "bu_on" )
	elseif	b_mode == "true"	then
		col = BU:get_color_back_named( b_equal and "bu_on" or "bu_off" )
	elseif	b_mode == "notice"	then
		if b_equal then
			col = self:get_color_back_named( "bu_on" )
		else
			col = pack( self:get_color_back() )
			--self:print( col )
			local f = wrap_01( aaa.time.t_real * 2 ) * .2
			--if col == "table" then
				--self:gol_rgba_back( f*col[1],f*col[2],f*col[3], col[4] )
				self:gol_rgba_back( 1,1,1, f )
			--end
			--col[1],col[2],col[3] = f,f*col[2],f*col[3]
			col[1] = col[1] + f
			col[2] = col[2] - f
		end
	end
	
	self:gol_rgba_back( col[1],col[2],col[3], col[4] )
	self:draw_rect_size()
end
--
-- COLOR
--
function BU:set_color_sel( color )		self.__color_sel = color			end
function BU:get_color_sel()
	local col = self.__color_sel
	if col then
		return col[1],col[2],col[3], col[4] * self:get_alpha_bu_to_draw()
	else
		return 1,1,1, .5 * self:get_alpha_bu_to_draw()
	end
end
function BU:gol_color_sel()
	gol.color( self:get_color_sel() )
end

function BU:set_border_line_color( color )	self.__border_line_color = color		end

--
--	DRAW UTIL
--
function BU:gol_draw_mul_line()				aaa.draw_mul_line(    0,   0, .5, .5 )	end
function BU:gol_draw_plus_line()			aaa.draw_plus_line(   0,   0, .5, .5 )	end
--todo3d
--function BU:gol_draw_rect_plain()			aaa.draw_rect(      -.5, -.5, .5, .5 )	end
--function BU:gol_draw_rect_uv()			aaa.draw_rect_uv(   -.5, -.5, .5, .5 )	end

function BU:gol_draw_rect_line()			aaa.draw_rect_line_cano()	end
function BU:gol_draw_rect_plain()			aaa.draw_rect_cano()		end
function BU:gol_draw_rect_uv()				aaa.draw_rect_uv_cano()		end

function BU:gol_draw_rect_line_size(  s )	aaa.draw_rect_line_size(s )	end
function BU:gol_draw_rect_plain_size( s )	aaa.draw_rect_size(		s )	end
function BU:gol_draw_rect_uv_size(    s )	aaa.draw_rect_uv_size(	s )	end


function BU:draw_border_line( size )
	gol.set_line_width( size )
	aaa.draw_rect_line_size()
	--todo3d
	if ga:is_3d() then
		local _,_,s = self:get_size_draw()
		aaa.draw_box_top_line( 1,1,s,	0,0,s*0.5 )
	end
end

--[[
function BU:gol_draw_rose()
	--todo make a C fn
	local d = .2
	gol.begin_triangle_fan()
		gol.vertex2( 0., 0. )
		gol.vertex2( -.5, -.5 )
		gol.vertex2( 0, -d )
		gol.vertex2( .5, -.5 )
		gol.vertex2( d, 0. )
		gol.vertex2( .5, .5 )
		gol.vertex2( 0, d )
		gol.vertex2( -.5, .5 )
		gol.vertex2( -d, .0 )
		gol.vertex2( -.5, -.5 )
	gol.do_end()
end
function BU:gol_draw_hexagone()
	--todo make a C fn
	gol.begin_triangle_fan()
		gol.vertex2( 0., -.5 )
		gol.vertex2( .433, -.25 )
		gol.vertex2( .433, .25 )
		gol.vertex2( 0., .5 )
		gol.vertex2( -.433, .25 )
		gol.vertex2( -.433, -.25 )
	gol.do_end()
end
function BU:gol_draw_triangle()
	--todo make a C fn
	gol.begin_triangle_fan()
		gol.vertex2( 0., -.5 )
		gol.vertex2( .5, .5 )
		gol.vertex2( -.5, .5 )
	gol.do_end()
end
function BU:gol_draw_rect_rotated()
	--todo make a C fn
	gol.begin_triangle_fan()
		gol.vertex2( -.5, 0 )
		gol.vertex2( 0., -.5 )
		gol.vertex2( .5, 0 )
		gol.vertex2( 0., .5 )
	gol.do_end()
end
function BU:gol_draw_octogone_low()
	gol.vertex2( -.5, .16666 )
	gol.vertex2( -.5, -.16666 )
	gol.vertex2( -.16666, -.5 )
	gol.vertex2( .16666, -.5 )
	gol.vertex2( .5, -.16666 )
	gol.vertex2( .5, .16666 )
	gol.vertex2( .16666, .5 )
	gol.vertex2( -.16666, .5 )
end
function BU:gol_draw_octogone()
	--todo make a C fn
	gol.begin_triangle_fan()
		self:gol_draw_octogone_low()
	gol.do_end()
end
function BU:gol_draw_octogone_line()
	--todo make a C fn
	gol.begin_line_loop()
		self:gol_draw_octogone_low()
	gol.do_end()
end
--]]



function BU:gol_draw_octo_line( s )
	--todo make a C fn
	gol.draw_line_loop_2d(	-.5, s,
							-.5, -s,
							-s, -.5,
							s, -.5,
							.5, -s,
							.5, s,
							s, .5,
							-s, .5 )
end


function BU:gol_draw_frame_rect()
	aaa.draw_rect_line_size()
end
function BU:gol_draw_frame_cross( ox, oy )
	gol.draw_lines_2d(	-.5-ox, -.5, .5+ox, -.5,
						-.5-ox, .5, .5+ox, .5,
						-.5, -.5-oy, -.5, .5+oy,
						.5, -.5-oy, .5, .5+oy
					)
end
function BU:gol_draw_frame_cross_light( ox, oy )
	gol.draw_line_strip_2d(	-.5-ox, 	-.5,	-.5, 	-.5,	-.5,	-.5-oy	)
	gol.draw_line_strip_2d(	.5+ox,		-.5,	.5,		-.5,	.5, 	-.5-oy 	)
	gol.draw_line_strip_2d(	-.5-ox, 	.5,		-.5, 	.5,		-.5,	.5+oy	)
	gol.draw_line_strip_2d(	.5+ox,		.5,		.5, 	.5,		.5, 	.5+oy	)
end
function BU:gol_draw_frame_cross_spe_light( inter )
--	gol.begin_lines()
	local ox	=	inter*.25
	local oy	=	inter
	gol.draw_line_strip_2d(	-.5-ox,		-.5,	-.5,	-.5,	-.5,	-.5-oy )
	gol.draw_line_strip_2d(	.5+ox,		-.5,	.5,		-.5,	.5,		-.5-oy )

	ox	=	(1-inter)*.25
	oy	=	(1-inter)
	gol.draw_line_strip_2d(	-.5-ox,		.5,		-.5,	.5,		-.5,	.5+oy )
	gol.draw_line_strip_2d(	.5+ox,		.5,		.5,		.5,		.5,		.5+oy )

--[[
	, .5+oxb, -.5 )
	aaa.draw_line(	-.5-oxb,			.5, .5+ox, .5 )
	aaa.draw_line(	-.5, -.5-oyb, -.5, .5+oy )
	aaa.draw_line(	.5, -.5-oy, .5, .5+oyb)
--]]
end
function BU:gol_draw_frame_cross_spe( inter )
	gol.draw_lines_2d(	-.5-inter*.25,	-.5,	.5+(1-inter)*.25, -.5,
						-.5-(1-inter)*.25, .5, 	.5+inter*.25, .5,
						-.5, -.5-(1-inter), 	-.5, .5+inter,
						.5, -.5-inter, 			.5, .5+(1-inter)
					)
end

--
--	UI
--
local __array_draw_ui_top_lines_pos =
{
	BU.__UI_TOP_LEFT,	-.5,				BU.__UI_TOP_RIGHT,	-.5,
	-.5,				BU.__UI_TOP_DOWN,	-.5,				BU.__UI_TOP_UP,
	BU.__UI_TOP_LEFT,	.5,					BU.__UI_TOP_RIGHT,	.5,
	.5,					BU.__UI_TOP_DOWN,	.5,					BU.__UI_TOP_UP
}
function BU:__draw_ui_top()
	--gol.set_depth( false )
	gol.set_texture_dim( 0 )
	local ph = aaa.time.t_real * 3 --+ self.__bu_id*.07
	ph = ( math.sin( ph * math.pi2 ) + 1 ) * .5
	gol.color( 0, .5+ph*.5, 1, 1 )
	--gol.color( 0, wrap_01( ph ) * 1.5 + .25 , 1, 1UPO
	gol.set_line_smooth( false )
	gol.set_logic_op_none()
	local lw = gol.get_line_width()
		if self:is_ui_top_size() then
			gol.set_line_width( 1 )
			aaa.draw_rects_line(	-.5,				-.5,				BU.__UI_TOP_LEFT,	BU.__UI_TOP_DOWN,
									BU.__UI_TOP_RIGHT,	-.5,				.5,					BU.__UI_TOP_DOWN,
									-.5,				BU.__UI_TOP_UP,		BU.__UI_TOP_LEFT,	.5,
									BU.__UI_TOP_RIGHT,	BU.__UI_TOP_UP,		.5,					.5
								)
			gol.draw_lines_2d(	__array_draw_ui_top_lines_pos	)
		end
		gol.set_line_width( 2 )
		gol.color( 0, 1-ph*.5, 1, 1 )
		aaa.draw_rect_line(	BU.__UI_TOP_LEFT,	BU.__UI_TOP_DOWN,	BU.__UI_TOP_RIGHT,	BU.__UI_TOP_UP	)

	gol.set_line_width( lw )
end

function BU.init_bind_video_image()
	BU.bind_video_border_off	= 0
	BU.bind_video_border_on		= 1
	BU.bind_image_border_off	= 2
	BU.bind_image_border_on		= 3
end
function BU.set_bind_border_video_to_image()
	BU.init_bind_video_image()
	BU.bind_video_border_off	=	BU.bind_image_border_off
	BU.bind_video_border_on		=	BU.bind_image_border_on
end
function BU.set_bind_border_image_to_video()
	BU.init_bind_video_image()
	BU.bind_image_border_off	=	BU.bind_video_border_off
	BU.bind_image_border_on		=	BU.bind_video_border_on
end

--BU.__carre = nil
function BU.carre_init()
	if BU.__carre then return end

	BU.__carre = {}
	BU.__carre.ref = {}
	local ref = BU.__carre.ref
	local layers = aaa.obj.get( "GaBu/GaBu_BU/BU_draw/fx.layers_param" )
	--ref.layers = aaa.obj.get_by_name_symbo( "BU_Carre" )
	ref.layers = layers

	ref.img_layer		= aaa.obj.get_branch_by_name_symbo( layers, "BU_layer_image" )
		ref.img_active		= param.get_ref( ref.img_layer, "active" )
		--ref.img_bank		= param.get_ref( ref.img_layer, "bank_2d" )
		ref.img_bind_offset	= param.get_ref( ref.img_layer, "bind_2d_offset" )
		ref.img_texturing	= param.get_ref( ref.img_layer,  "use_texturing" )
		ref.img_shading		= param.get_ref( ref.img_layer,  "use_shading" )

	ref.frame_layer		= aaa.obj.get_branch_by_name_symbo( layers, "BU_layer_frame" )
		ref.frame_active	= param.get_ref( ref.frame_layer, "active" )
		ref.back_color		= aaa.layer.create_color( ref.frame_layer, "frame" )
--	ref.frame_rendering	= aaa.layer.get_rendering( ref.frame_layer )
--	ref.border_red			= param.get_ref( ref.frame_rendering, "top_line_red" )
--	ref.border_green		= param.get_ref( ref.frame_rendering, "top_line_green" )
--	ref.border_blue			= param.get_ref( ref.frame_rendering, "top_line_blue" )

	ref.mapping			= aaa.layer.get_mapping( ref.img_layer )
		ref.tex_left		= param.get_ref( ref.mapping, "tex_left" )
		ref.tex_right		= param.get_ref( ref.mapping, "tex_right" )
		ref.tex_top			= param.get_ref( ref.mapping, "tex_top" )
		ref.tex_bottom		= param.get_ref( ref.mapping, "tex_bottom" )
		ref.blend_src		= param.get_ref( ref.mapping, "blend_src" )
		ref.blend_dst		= param.get_ref( ref.mapping, "blend_dst" )

		ref.blend_alpha		= param.get_ref( ref.mapping, "blend_color_alpha" )

		ref.tex_u_ori		= param.get_ref( ref.mapping, "tex_u_ori" )
		ref.tex_v_ori		= param.get_ref( ref.mapping, "tex_v_ori" )
		ref.tex_u			= param.get_ref( ref.mapping, "tex_u" )
		ref.tex_v			= param.get_ref( ref.mapping, "tex_v" )
		ref.tex_by_side		= param.get_ref( ref.mapping, "tex_by_side" )
		ref.tex_u_wrap		= param.get_ref( ref.mapping, "tex_wrap_u" )
		ref.tex_v_wrap		= param.get_ref( ref.mapping, "tex_wrap_v" )

	ref.color			= aaa.layer.create_color( ref.img_layer, "image" )

--	ref.rendering		= aaa.layer.get_rendering( ref.img_layer )
	ref.frame_rendering	= aaa.layer.get_rendering( ref.frame_layer )
--	ref.top_line			= param.get_ref( ref.frame_rendering, "top_line" )
--	ref.line_size			= param.get_ref( ref.frame_rendering, "line_size" )
--	ref.line_red			= param.get_ref( ref.frame_rendering, "top_red" )
--	ref.line_green			= param.get_ref( ref.frame_rendering, "top_green" )
--	ref.line_blue			= param.get_ref( ref.frame_rendering, "top_blue" )

	ref.shading			= aaa.layer.get_shading( ref.img_layer )
		--chanel generalize or move or remove
		ref.shading_inter	= param.get_ref( ref.shading, "fu_float_03" )
		ref.shading_scale_x	= param.get_ref( ref.shading, "fu_float_04" )
		ref.shading_scale_y	= param.get_ref( ref.shading, "fu_float_05" )
--	BORDER
	ref.border_layer	= aaa.obj.get_branch_by_name_symbo( layers, "BU_layer_border" )
		ref.border_active		= param.get_ref( ref.border_layer, "active" )
		ref.border_bind_offset	= param.get_ref( ref.border_layer, "bind_2d_offset" )
--	BORDER LINE
	ref.border_color		= aaa.layer.create_color( ref.border_layer, "border" )

	ref.border_line	=	{}
	ref.border_line.layer	= aaa.obj.get_branch_by_name_symbo( layers, "BU_layer_border_line" )
		ref.border_line.active		= param.get_ref( ref.border_line.layer, "active" )
	ref.border_line.rendering	= aaa.layer.get_rendering( ref.border_line.layer )
		ref.border_line.size		= param.get_ref( ref.frame_rendering, "line_size" )
	ref.border_line.color		= aaa.layer.create_color( ref.border_line.layer, "border_line" )

	ref.but = {}
	ref.but.layer		= aaa.obj.get_branch_by_name_symbo( layers, "BU_video_but" )
	ref.but.model		= aaa.layer.get_model( ref.but.layer )
	ref.but.size_u			= param.get_ref( ref.but.model, "size_u" )

	ref.play = {}
	ref.play.layer		= aaa.obj.get_branch_by_name_symbo( layers, "bu_video_play" )
		ref.play.active		= param.get_ref( ref.play.layer, "active" )
	ref.play.transfo	= aaa.obj.get_branch_by_class( ref.play.layer, "transfo_trs" )
		ref.play.x			= param.get_ref( ref.play.transfo, "translate_x" )
		ref.play.y			= param.get_ref( ref.play.transfo, "translate_y" )

	ref.stop = {}
	ref.stop.layer		= aaa.obj.get_branch_by_name_symbo( layers, "bu_video_stop" )
		ref.stop.active		= param.get_ref( ref.stop.layer, "active" )
	ref.stop.transfo	= aaa.obj.get_branch_by_class( ref.stop.layer, "transfo_trs" )
		ref.stop.x			= param.get_ref( ref.stop.transfo, "translate_x" )
		ref.stop.y			= param.get_ref( ref.stop.transfo, "translate_y" )

	ref.rewind = {}
	ref.rewind.layer	= aaa.obj.get_branch_by_name_symbo( layers, "bu_video_rewind" )
		ref.rewind.active	= param.get_ref( ref.rewind.layer, "active" )
	ref.rewind.transfo	= aaa.obj.get_branch_by_class( ref.rewind.layer, "transfo_trs" )
		ref.rewind.x		= param.get_ref( ref.rewind.transfo, "translate_x" )
		ref.rewind.y		= param.get_ref( ref.rewind.transfo, "translate_y" )

	--was the old way
---[[ -- still useful (e.g. chanel)
	ref.text = {}

	ref.text.layer		= aaa.obj.get_branch_by_name_symbo( layers, "bu_video_text" )
		ref.text.active 	= param.get_ref( ref.text.layer, "active" )
		ref.text.bdd		= aaa.obj.get_branch_by_class( ref.text.layer, "bdd_text" )
			ref.text.x			= param.get_ref( ref.text.bdd, "origin_x" )
			ref.text.y			= param.get_ref( ref.text.bdd, "origin_y" )
			ref.text.text		= param.get_ref( ref.text.bdd, "text" )
		ref.text.model		= aaa.layer.get_model( ref.text.layer )
			ref.text.su			= param.get_ref( ref.text.model, "size_u" )
			ref.text.sv			= param.get_ref( ref.text.model, "size_v" )
		ref.text.color_ref	= aaa.layer.create_color( ref.text.layer, "text" )
--]]

	BU.__carre.but = {}
		BU.__carre.but[1]	=	{	ref = ref.rewind,	name = "rewind", 	pos = -.35	}
		BU.__carre.but[2]	=	{	ref = ref.stop,		name = "stop", 		pos = .42	}	--pos=-.15	}
		BU.__carre.but[3]	=	{	ref = ref.play,		name = "play",	 	pos = .42	}	--pos=-.15	}

	BU.init_bind_video_image()
end

BU.carre_init()
BU.__carre.but[2].pos = -.42
BU.__carre.but[3].pos = -.42
function BU.c_init_draw()
	BU.init_bind_video_image()
end
function BU:get_but_in( x, y )
	for i=1,2 do
		local but = BU.__carre.but[i]
		--self:print( "but is "..but )
		--table.print( but, "but", 2 )
		if not b_no_rewind or but.name ~= "rewind" then
			local l2 = math.get_length_squared( x-but.x, y-but.y )
			--self:print( i.." "..l2 )
			if l2 < but.size2 then
				if self.verbose >= 1 then self:print(  "button found "..i ) end
				return i
			end
		end
	end
	return nil
end

function BU.place_video_but()
	if not BU.__carre then return end
	local y = -.35
	for i = 1, 3 do
		local but = BU.__carre.but[i]
		--aaa.print( "i "..i.." ii "..ii-1 )
		but.x = but.pos
		but.y = y
		but.size = .06
		but.size2 = but.size * but.size
		param.set( but.ref.x, but.x )
		param.set( but.ref.y, but.y )
	end
end
BU.place_video_but()

function BU:__prepare_video_but( b )
	local car = BU.__carre
	--aaa.print( "toto" )
	--local b_last = car.b_video_but_last
	--if b_in == b then return end
	local but = car.but
	if b then
		param.set( car.ref.but.size_u, self:get_ratio_y() )
		local video = self:get_video()
		but[1].__b_active = not b_no_rewind
		local bp		= video:is_playing()
		local bs	= video:is_scrub()
		but[2].__b_active = not bs and bp
		but[3].__b_active = not bs and not bp
	else
		but[1].__b_active = false
		but[2].__b_active = false
		but[3].__b_active = false
	end
	--car.b_video_but_last = b_last
end
function BU:__set_video_but()
	local buts = BU.__carre.but
	for i=1,3 do
		local but = buts[i]
		param.set( but.ref.active, but:is_active() )
	end

end
function BU:__clear_video_but()
	local buts = BU.__carre.but
	param.set( buts[1].ref.active, false )
	param.set( buts[2].ref.active, false )
	param.set( buts[3].ref.active, false )
end

--doit in C++
function BU:draw_bind_full( bind, alpha )
	gol.color_white( alpha )
	gol.set_texture_dim( 2 )
	gol.bind_texture( bind )
	gol.set_quad_uv()
	self:gol_draw_rect_uv()
	gol.set_texture_dim( 0 )
end

function BU:draw_bind( bind, le,bo, ri,to, alpha )
	--if true then return end
	if aaa.img.get_size( bind ) then
		-- if self.color then
		-- 	--self:print( "coloring")
		-- 	gol.color( self.color.r, self.color.g, self.color.b, alpha )
		-- 	--table.print( self.color, "color")
		-- else
			gol.color_white( alpha )
		--end
		--gol.color_red( alpha )
		--self:print( "bind "..bind  )
		--gol.set_blend_max()
		--gol.reset()
		--gol.set_matrix_texture()
		--	gol.set_matrix_identity()
		--gol.set_matrix_modelview()
		gol.set_texture_dim( 2 )
		gol.bind_texture( bind )
		--gol.bind_texture( 33 )

		gol.set_quad_uv( le,bo, ri,to )
		--self:print( "Bind "..bind.." lbrt "..le.." "..bo.." "..ri.." "..to )
		--self:print( "Toto" )
		--gol.set_cull_front()
		self:gol_draw_rect_uv()
		--local suv = 1
		--gol.set_quad_uv( 0,0, suv,suv )
		--local s = .4
		--aaa.draw_rect_uv(	-s,-s,	s,s )
		--gol.begin_quads()
	-- 	gol.begin_triangles()
	-- 		gol.draw_uv_xy(	0,0, -s,-s )
	-- 		gol.draw_uv_xy(	.9,.9, s,s )
	-- 		gol.draw_uv_xy(	.9,0, s,-s )

	-- --		gol.draw_uv_xy(	0,.9, -s,s )
	-- 	gol.do_end()
		--aaa.draw_rect_uv(  -.2,-.5, .2,.5 )
		--	self:print( "render bind "..bind.." at { "..le..","..bo..","..ri..","..to.." }" )
		gol.set_texture_dim( 0 )
	else
		gol.color( {.5,.25,.25,.5} )
		aaa.draw_rect_size()
		gol.color_orange( alpha )
		aaa.draw_mul_line( 0,0, .1,.1 )
	end
end

function BU:draw_border()
	local alpha = self:get_alpha_bu_to_draw()
	--if not (ga.b_border_texture and not self:is_border_line()) then
		local b_contact = self:is_contact()
		local color = self.__line_color_forced
		if not color then
			color = self.__border_line_color
			if not color then
				--todo centralize these color
				if b_contact then
					color = BU.__border_line_color_contact
				else
					color = BU.__border_line_color_contact_no
				end
			end
		end
		gol.color( color[1],color[2],color[3], (color[4] or 1) * alpha )
		self:draw_border_line( (b_contact and 2 or .5 ) * self.__draw_text_line_width )
	--else
		-- Huh
	--end
end

function BU:draw_border_tex( b_video )
	local alpha = self:get_alpha_bu_to_draw()
	if ga.b_border_texture and not self:is_border_line() then
		local b_contact = self:is_contact()
		local ref = BU.__carre.ref
		local border_bind_offset
		--todo chanel use image border for video border
		--todo		si add an option and remove chanel code to eliminate boder
		if b_video then
			border_bind_offset = b_contact and self.bind_video_border_on or self.bind_video_border_off
		else
			border_bind_offset = b_contact and self.bind_image_border_on or self.bind_image_border_off
		end
		--param_set( ref.border_active, 0 )
		--param_set( ref.top_line, 0 )
		--param_set( ref.top_line, 1 )

		ref.border_color:set_alpha( alpha )

		param.set( ref.border_active,	true		)
		if border_bind_offset then
			param.set( ref.border_bind_offset, border_bind_offset )
		end

		--param_set( ref.border_line.active,	false	)
	end
end

function BU:draw()
	--if true then return end
	if self.verbose >= 3 then aaa.print_method() end

	--opt this should be faster with caching

	if GA.b_spy then aaa.spy.push_range( "BU:draw", 3 ) end

		--	PASS 1 we prepare all the info
		--
		local b_video = self:is_video()
		local b_video_button

		local bind
		-- TEXTURE COORS
		local le, ri
		local bo, to

		local b_tex_by_side = true

		local b_img_active
		local b_frame_active

		--	TEXTURE
		if self:is_tex_draw() then
			bind = self:get_texture_bind_to_draw()
			--self:print( "bind "..bind )
			--bind = math.random( 0, 8 )
			-- TEXTURE COORS
			--todo why a special case
			if b_video then
				--	crop a little bit to avoid black video on the side
				local c = .1
				le, ri = c, 1-c
				bo, to = c, 1-c
			else
				le, ri = 0, 1
				bo, to = 0, 1
			end

			--local tex = self.__texture
			local tex = rawget( self, "__texture" )
			if tex then
				local lbrt = tex.lbrt
				if lbrt then
					le = lbrt.l or le
					ri = lbrt.r or ri
					bo = lbrt.b or bo
					to = lbrt.t or to
				end		
				--	flip if needed
				if tex.b_flip_u then le,ri = ri,le	end
				if tex.b_flip_v then to,bo = bo,to	end
			end

			-- if self.b_tex_anim then
			-- 	-- texture must not be by side in case of tex animation
			-- 	b_tex_by_side = false
			-- end

			b_img_active	= true
			b_frame_active	= false
		else
			b_img_active	= false
			b_frame_active	= true	--todo use in the lua case
		end	--if self:is_tex_draw()

		local video_but_sy_limit  =	2
	--	if false then
		if b_video then
			--n5	to generalize
			local sy = self.__sy_rt
			if not sy then
				sy = self:get_sy()
			end
			b_video_button = self:is_video_button()
			if b_video_button then
				--n5	to generalize
				if sy > video_but_sy_limit then
					if self:is_video_playing() then
						if not self.__time_when_play then
							self.__time_when_play = aaa.time.t_real
						else
							if self.__time_when_play + 3. <  aaa.time.t_real then
								self:set_video_button( false )
								--hack
								b_video_button = false
							end
						end
					else
						self.__time_when_play = nil
					end
				else
					self.__time_when_play = nil
				end
			else
				--n5	to generalize
				if sy < video_but_sy_limit then
					self:set_video_button( true )
					b_video_button = true
				end
				self.__time_when_play = nil
			end
			self:__prepare_video_but( b_video_button )
		end	-- if video


	if GA.b_spy then
		aaa.spy.pop_range()
		aaa.spy.push_range( "BU:draw", 4 )
	end

		-- BLEND
		local b_blend = self.blend_spe

		--	back alpha
		--	back color

		--	alpha
		local alpha = self:get_alpha_bu_to_draw()
		--
		--	PASS 2 we DRAW NOW
		--	
		local b_lua = self:is_draw_lua()
		if b_lua then
			if bind then
				if not b_img_active then
					self:print( "tex : we should not pass here" )
				end
				--self:print( "zob")
				--self:draw_bind( bind, le,bo, ri,to, alpha )
				--gol.translate_x( .12 )
				self:draw_bind( bind, le,bo, ri,to, alpha )
			else
				--if b_img_active then
					--hack
					--self:print( "no bind but b_image_active true : we should not pass here" )
				--end
				--self:draw_back()
			end

			if self:is_text_draw() then
				self:__draw_text_on_top( alpha )
			end

			-- BORDER
			if self:is_border() then
				self:draw_border()
			end
		else -- if b_lua
			local param_set = param.set
			local ref = BU.__carre.ref

			if bind then
				--self:print( bind )
				param_set( ref.img_bind_offset, bind )
					--self:print( bind )
				param_set( ref.tex_left,	le	)
				param_set( ref.tex_right,	ri	)
				param_set( ref.tex_top,		to	)
				param_set( ref.tex_bottom,	bo	)

				param_set( ref.tex_by_side, b_tex_by_side )

				-- defaut wrap
				local wrap_u = 3
				local wrap_v = 3

				if b_tex_by_side then
					-- repeat
					wrap_u = 2
					wrap_v = 2
				end

				local u_ori = self.tex_u_ori or .0
				local v_ori = self.tex_v_ori or .0

				param_set( ref.tex_u_ori, u_ori )
				param_set( ref.tex_v_ori, v_ori )

				local tex_u = self.tex_u or 1.
				local tex_v = self.tex_v or 1.

				param_set( ref.tex_u, tex_u )
				param_set( ref.tex_v, tex_v )

				param_set( ref.tex_u_wrap, wrap_u )
				param_set( ref.tex_v_wrap, wrap_v )

				-- if self.color then
				-- 	ref.color:set_rgba( self.color.r, self.color.g, self.color.b, alpha )
				-- else
					ref.color:set_rgba( 1,1,1, 1 )
				-- end
			--	ref.color:set_rgba( 1.0, .0, 1., .1 )
			--	ref.color:set_rgba_t(  )

			--	was vulcania	param_set( ref.img_texturing, self.circle_mask and 2 or 0 )
				--	because chanel was able change this
				--	opt
				param_set( ref.img_texturing,	1	)	--	1 is none
				param_set( ref.img_shading,		1	)	--	1 is none
			else
			end
			param_set( ref.img_active,		b_img_active	)
			param_set( ref.frame_active,	b_frame_active	)

			if b_video then
				self:__set_video_but()
			end

			ref.back_color:set_rgba( 0,0,0, self.__back_alpha )
			ref.color:set_alpha( alpha )


			local r = ref.text
			local text = self:get_text() and false -- self:is_text_draw()
			--if text_x then
			if text then
				param_set( r.active, true )
				param_set( r.text, text )
				--param_set( r.su, text_sx  )
				--param_set( r.sv, text_sy )
				--param_set( r.x, text_x )
				--param_set( r.y, text_y )

				local t = self:get_text_color_table()
				local cref = r.color_ref
				if t then
					--self:print( t[1].." "..t[2].." "..t[3].." "..t[4] )
					cref:set_rgba( t[1], t[2], t[3], t[4] or 1. )
				else
					cref:set_rgba( 0, 1, self:is_contact() and 0 or 1, 1. )
				end
			else
				param_set( r.active, false )
			end

			if b_blend then
				--self:print( "true" )
				param_set( ref.blend_src, 2 )	-- SRC_COLOR
				param_set( ref.blend_dst, 1 )	-- GL_ONE
			else
				--self:print( "false" )
				param_set( ref.blend_src, 6 )	-- SRC_ALPHA
				param_set( ref.blend_dst, 7	)	-- ONE_MINUS_SRC_ALPHA
			end

			-- BORDER
			if self:is_border() then
				self:draw_border_tex( b_video )
			end

			if true then
				aaa.layers.draw_layer_all( ref.layers )
			else
				local function draw( par, layer )
					param.set( par, true )
					aaa.obj.update_then_draw( layer )
				end
				if layer_to_draw.img			then	draw( ref.img_active,			ref.img_layer )			end
				if layer_to_draw.frame			then	draw( ref.frame_active,  		ref.frame_layer )		end
				if layer_to_draw.border			then	draw( ref.border_active, 		ref.border_layer )		end
				if layer_to_draw.border_line	then	draw( ref.border_line.active, 	ref.border_line.layer )	end
			end
			if b_video then
				self:__clear_video_but()
			end
		end	-- if b_lua


		--self:print( b_video_button.." -- "..self:is_video_scrub() )
	--[[	was
		if b_video_button and self:is_video_scrub() then
			local video = self:get_video()
			--hack
			local b_play = video:is_playing()
			local s = self.__video_scrub_sy
			self:print( "scrib y "..s )
			if s > 0 then
				gol.set_default()
				gol.color( .5, .5 )
				local x = video:get_time_cano() - .5
				aaa.draw_rect( -.5, -.5, x, -.5 + s * .08 )
			end
		end
	--]]
		-- BORDER
		if self:is_border() then
			self:draw_border()
		end

		if b_video_button then
			local video = self:get_video()
			gol.set_default()
			gol.color( .5, .25 )
			local l = -.48
			local b = -.41
			local r = -.41
			local t = -.3
			local sx = r-l
			local sy = t-b
			local cx = (l+r)*.5
			local cy = (b+t)*.5
			--aaa.draw_rect( l, b, r, t	)
			gol.set_blend_add_alpha_one()
			--draw_video_button proceduraly
			if video:is_playing() then
				aaa.draw_rect( cx-sx*.25-sx*.125, b+sy*.125,  cx-sx*.25+sx*.125, t-sy*.125	)
				aaa.draw_rect( cx+sx*.25-sx*.125, b+sy*.125,  cx+sx*.25+sx*.125, t-sy*.125	)
			else
				gol.draw_triangles_2d(	cx-sx*.25-sx*.125, t-sy*.125,
										cx-sx*.25-sx*.125, b+sy*.125,
										cx+sx*.25+sx*.125, cy )
			end
			--hack
			local b_play = video:is_playing()
			if self:is_video_scrub() then
				local s = self.__video_scrub_sy
				--self:print( "scrib y "..s )
				if s > 0 then

					gol.color( .5, .25 )
					local x = video:get_time_cano() - .5
					aaa.draw_rect( -.5, -.5, x, -.5 + s * .08 )
				end
			end
		end	--if b_video_button


	if GA.b_spy then aaa.spy.pop_range() end
end


-- BLINK
function BU:set_blink( b )			self.__b_top_blink = b 			return self end
function BU:is_blink()				return self.__b_top_blink 					end

--todo there is a problem here
function BU:set_blink_period( p )	self.__top_blink_period = p		return self end
function BU:get_blink_period()		return self.__top_blink_period or .25		end

function BU:set_tex_led( bind_led_on, bind_led_off )
	self.__b_tex_draw = true
	self.__tex_led_on = bind_led_on
	self.__tex_led_off = bind_led_off
end
function BU:get_tex_led()		return self.__tex_led_on, self.__tex_led_off	end

function BU:__get_blink_phase_bool()
	local t = aaa.time.t_real
	local period = self:get_blink_period()
	t = math.fmod( t, period )
	return t, t < period * .5
end
function BU:__draw_led( lbrt, b_out, b_change )
	--if true then return end
	lbrt = lbrt or {-.5,-.5, .5,.5}
	--self:set_blink_period( 1 )
	local _,b = self:__get_blink_phase_bool()
	if b then
		self.__b_top_led_on		= true
		self.__b_top_led_off	= false
		self.__b_top_led_active = true
	else
		self.__b_top_led_on		= false
		self.__b_top_led_off	= true
		self.__b_top_led_active = false
	end

	local tex_led_on, tex_led_off = self:get_tex_led()
	local alpha = self:get_alpha_bu_to_draw()

	if tex_led_on and self:is_tex_draw() then
		local size = .15
		local x = .5 - size / 2
		local y = 0
		local ratio = self:get_ratio_x()
		gol.push_scale_2d( 1, ratio )
			size = size * 0.5
			gol.color_white( alpha )
			local bind = self.__b_top_led_on and tex_led_on or tex_led_off
			aaa.draw_rect_uv( bind, x - size, y - size, x + size, y + size )
		gol.pop_matrix()
	else
		gol.set_texture_dim( 0 )
		if b_change then
			gol.color_red( alpha )
		else
			if self.__b_top_led_on then
				gol.color_black( alpha )
			else
				gol.color_green( alpha )
			end
		end
		local size_y = .5
		local size_x = size_y * self:get_ratio_y()
		local x,y
		if b_out then
			x = lbrt[1] + size_x / 2
			y = .5 - size_y / 2
		else
			x = lbrt[3] - size_x / 1.4	-- visually done depend onm sx of BU ideally
			y = -.5 + size_y / 2
		end
		aaa.draw_ellipse_axe_z(	x,y,0, size_x,size_y, 3 )
	end

end
function BU:__draw_led_for_trax( gp, bu_trax, lbrt )
	local t_bu = bu_trax[self]
	if t_bu then
		--table.print( t, "t for "..self, 3  )
		local t_by_meu = t_bu.t_out
		if t_by_meu then
			local b = false
			for _,t in pairs(t_by_meu) do
				b = b or t.b_changed
			end
			if not self.__b_no_led then
				self:__draw_led( lbrt, true, b )
			end
			self.__b_change_out = b
		end

		t_by_meu = t_bu.t_in
		if t_by_meu then
			local b = false
			for _,t in pairs(t_by_meu) do
				b = b or t.b_changed
			end
			if not self.__b_no_led then
				self:__draw_led( lbrt, false, b )
			end
			self.__b_change_in = b
		end
		
		t_bu.b_drawn = true
	end
end

function BU:draw_led( lbrt )
	if app then
		local gp = app:get_gp()
		if gp then
			local bu_trax = gp.__bu_trax
			if bu_trax then
				self:__draw_led_for_trax( gp, bu_trax, lbrt )
			end
		end
	end
	--if b == nil then
		if self.read_midi_control and self:read_midi_control() then
	 		self:__draw_led( lbrt, true, true )
		end
	--end
end
