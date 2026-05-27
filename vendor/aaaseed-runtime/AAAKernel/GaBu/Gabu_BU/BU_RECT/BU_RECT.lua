-- a BU with ui/values
if CLASS.DECLARE( "BU_RECT", SLIDER,
		{
			__color_back = BU:get_color_back_named("BUI"),
			__b_value_load_save = true,

			__b_uif_outside		= false,
		}
	) then
	BU_RECT:set_class_status_doc(	CLASS.STATUS.GABU,
									"Very useful to control a bunch of MU for now (2023 Sept)" )
end

function BU_RECT:do_action( action )
	local b_done = false
	local mus = self:__get_mus_up()
	if mus then
		b_done = true
		if     action=="delete" then	mus:remove_render_rect( self )
		elseif action=="insert" then	mus:insert_render_rect( self )
		elseif action=="move_up" or action=="move_down" then
			mus:inc_render_rect( self, action == "move_up" and 1 or -1 )		
		else
			b_done = false
		end
	end
	return b_done
end

-- --done by BU now that we have do_copy/paste
function BU_RECT:do_key(key)
	self:print_do_key( "BU_RECT", key )

 	local b_key_used = false
	if not aaa.keyboard.is_alt() and aaa.keyboard.is_ctrl() then
 		if key == 127 then
			b_key_used = self:do_action( "delete" )
		end
	end
	self:print( "BU_RECT:do_key() : b_key_used is "..b_key_used )

 	return b_key_used or oo.getsuper(BU_RECT).do_key( self, key )
end

function BU_RECT:do_key_special( key )
	self:print_do_key_special( "BU_RECT", key )

 	local b_key_used = false
	local mus = self:__get_mus_up()
	if mus then
		self:print( "BU_RECT:do_key_special() : mus_up is "..mus )
		if not aaa.keyboard.is_alt() and aaa.keyboard.is_ctrl() then
			if key == 272 or key == 273 then		-- UP DOWN
				b_key_used = self:do_action( key == 272 and "move_up" or "move_down" )
			elseif key == 276 then
				b_key_used = self:do_action( "insert" )
			end
		end
	end
	self:print( "BU_RECT:do_key_special() : b_key_used is "..b_key_used )

 	return b_key_used or oo.getsuper(BU_RECT).do_key_special( self, key )
end

-- function BU_RECT.init_instance(self)
-- 	--aaa.print_fn( "BU_RECT.init_instance" )
-- 	self:dbox_dbg( "BU_RECT.init_instance" )
-- 	oo.getsuper(BU_RECT).init_instance(self)

-- 	self:set_text_xy_fxy( 0,.9, .1,.1 )
-- 	self:set_ui_top_size(true)
-- --	self:box_debug( "init_instance()" )
-- 	return self
-- end

function BU_RECT:create( name, rect )
	local self = BU_RECT:create_instance( name, rect )
	--self:set_text_xy_fxy( 0,0, 1,1 )
	--self:set_text_xy_fxy( .5,.9, .1,.1 )
	self:set_ui_top_size( true )
	self:set_inertia( false )
	self:set_grid_xy( 1/16, 1/16 )
	self:set_sxy_min( 1/4, 1/8 )
	self:set_grid_angle( 1. )	-- disable angle
	
--	self:set_text_xy_fxy_mini( 0,.45,	.08,.6 )
--	self:set_text_nice()

	-- self:set_hook_on_value_change(
	-- 	function( bu, balue )
	-- 		if balue:get_value_as_bool() then
	-- 			self:set_dplane(-42)
	-- 		else
	-- 			self:set_dplane(42)
	-- 		end
	-- 	end )

	self.__b_glue = true
	return self
end

function BU_RECT:__build_glue_moving()
	local rx,ry = self:get_xy()
	local t = {}
	local mus = self:__get_mus_up()
	local lbrt = self:get_lbrt()
	for _, mu in PAIRS( mus.__mu_down ) do
		if mu:is_visible() and mu:is_inside_lbrt( lbrt ) then
			--self:print( mu.." is inside")
			local x,y = mu:get_xy()
			table.insert( t, { mu=mu, dx=x-rx, dy=y-ry } )
		end
	end
	self.__glue_moving = #t>0 and t or nil
end
function BU_RECT:__add_contact( blob, ui_type )
	if self.__b_glue and ui_type == "move" then
		self:__build_glue_moving()
	else
		self.__glue_moving = nil
	end
	oo.getsuper(BU_RECT).__add_contact( self, blob, ui_type )
end

function BU_RECT:__move_dxy( dx,dy, b_constraint_asked, b_store_speed )
	oo.getsuper(BU_RECT).__move_dxy( self, dx,dy, b_constraint_asked, b_store_speed )
	if self.__glue_moving then
		local x,y = self:get_xy()
		for i, t in IPAIRS( self.__glue_moving ) do
			t.mu:set_xy( x+t.dx, y+t.dy  )
			--mu:__move_dxy( dx,dy, false, false, "move" )
		end
	end
end

-- 2024 Nov Maa tried undoredo on BU_RECT
function BU_RECT:set_xy( x,y )
	aaa.print_fn( "BU_RECT:set_xy()" )

	if not self:is_contact() then
		self:__build_glue_moving()
		if self.__glue_moving then
			local cur_x, cur_y = self:get_xy()
			local dx = x - cur_x
			local dy = y - cur_y
			self:print( dx .. " " .. dy )
			for i, t in IPAIRS( self.__glue_moving ) do
				local mu = t.mu
				local x,y = mu:get_xy()
				mu:set_xy( x+dx, y+dy )
				--mu:__move_dxy( dx,dy, false, false, "move" )
			end
		end
		self.__glue_moving = nil
	end
	oo.getsuper(BU_RECT).set_xy( self, x,y )
end

-- function BU_RECT:draw_text_color()
-- 	gol.color_red()
-- end
-- BU_RECT.draw_text_color_sel = BU_RECT.draw_text_color 

-- if self:is_text_draw() then
-- 	self:__draw_text( b_special, b_state )
-- end

function BU_RECT:get_uif_zones()
	local zones = {}	--oo.getsuper(BU_RECT).get_uif_zones( self )
	local command = self.__b_glue and "UnGlue" or "Glue"
	zones[command]	=	{	angle=28.5/32,		dangle=5/32,	dist=1,	dx=.15, dy=.05,justify="left" }
	local X,Y = -5.5,-1.2
	local DY = 1.2
	zones.Delete	=	{	type="button",		angle=3/4,		dist=1,	rect={X,Y,			3}, action="delete"	}
	zones.Insert	=	{	type="button",		angle=3/4,		dist=1,	rect={X,Y+DY,		3}, action="insert"	}
	X = -2.5
	zones.Up		=	{	type="button",		angle=3/4,		dist=1,	rect={X,Y+DY,		2}, action="move_up"	}
	zones.Down		=	{	type="button",		angle=3/4,		dist=1,	rect={X,Y,			2}, action="move_down"	}

	self:add_uif_zones_base( zones, 1, -1.4 )
	return zones
end

function BU_RECT:do_uif_command( uif )
	aaa.print_fn()
	local command = BU:get_uif_name_sel( uif )
	local b_used = false
	local action = uif.zone_sel.action
	if action then
		b_used = self:do_action( action )
	else
		b_used = true
		if		command == "glue"	then	self.__b_glue = true
		elseif	command == "unglue"	then	self.__b_glue = false	
		else								b_used = false
		end
	end

	return GABU_OBJ.do_uif_command_with_super( self, b_used, uif, BU_RECT )
end

function BU_RECT:draw_back()
	self:draw_back_bind_only()
end

function BU_RECT:draw()
	local sy = self:get_sy()
	sy = (1/16)/sy

	local h = .5-sy*1.2
	local alpha = self:get_value()
	if alpha > 0 and app:get_gp():is_render() then
		self:gol_green_back()
		aaa.draw_rect( -.5,-.5, .5,h )
		--this border color bizness have to be centralized for all BU operation
		-- if self:is_bu_cur() then	gol.color_magenta()
		-- else						gol.color_green()
		-- end
		gol.color_green()
	else
		self:gol_red_back()
		aaa.draw_rect( -.5,-.5, .5,h )
		--self:draw_back_inactive()
		gol.color_red()
	end
	aaa.draw_rect_line_size()

	if self.__b_glue then		
		gol.draw_lines_2d( -.5,h, .5,h )
	end

	if alpha > 0 then
		self:gol_color_meter_def()
		aaa.draw_rect( -.5,h, -.5+alpha,.5 )
		if alpha < 1 then
			gol.color_black(.25)
			--gol.color_white( .5 )
			aaa.draw_rect( -.5+alpha,h, .5,.5 )
		end
	else
		gol.color( .5, 0,0, .25 )
		--gol.color_black(.5)
		--gol.color_white( .5 )
		aaa.draw_rect( -.5,h, .5,.5 )
	end
	
	local text = self:get_name()
	--if text then
		if alpha ~= 1 then
			if alpha ~= 0 then
		 		gol.color_red( math.sin( aaa.time.t_real * math.pi2 * 4 ) * .25 + .75 )
			else
				gol.color_red( 1 )
			end
		else
			self:draw_text_color()
		end
		--was this but two small for window with long name
		--self:draw_text_lrbt( text, -.5, .5, -.5, .5 )	
		gol.set_line_width(2)
		self:draw_text_lrbt( text..": "..aaa.format.real_dot2(alpha), -.5,.5, .5-sy,.5, "center" )
	--end
	-- if self:is_text_draw() then
	-- 	self:__draw_text()
	-- end

	-- if b_on then	gol.color_green( 1 )
	-- else			gol.color_red( 1 )
	-- end

	--todo 3d version
end	

