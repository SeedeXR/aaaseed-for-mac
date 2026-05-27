--require('mobdebug').start()

local UIF_DY					= 1/8
local UIF_MANUAL_SIZE_MIN		= UIF_DY * 2.
local UIF_MANUAL_SIZE_ACTIVE	= UIF_DY * 4.
local UIF_NEUTRAL				= UIF_DY * .5

local UIF_COLOR_DEF_VALUE		= { .25,.75,1,	1 }
local UIF_COLOR_SELECT			= { 1,0,1,		1 }
local UIF_COLOR_CURRENT			= { 1,0,0,		1 }

--[[
local function draw_star( x, y, nb, r )
	r = r * 1
	local rm = r * .4
	--local ri = r * .25
	local da = math.pi * 2 / nb
	local sin = math.sin
	local cos = math.cos
	local a = math.pi * .5
	local dda = da * .5
	for i=1,nb-1 do
		gol.draw_line_strip_2d(	x + rm*cos(a-dda),	y + rm*sin(a-dda),
								x + r *cos(a),		y + r *sin(a),
								x + rm*cos(a+dda),	y + rm*sin(a+dda)
								--x + ri*sin(a),		y + ri*cos(a),
								)
		a = a + da
	end
end
--]]

function BU:add_dialog_message( str )
	local x,y = self:convert_xy_local_to_top( -1,0 )
	ga:add_dialog_message( self.."\n"..str, {x,y} )
end

function BU:get_uif_verbose()	return self.__uif_verbose		end
function BU:set_uif_verbose(s) 	self.__uif_verbose = s 			end

--	uif -> User Interface Fast
BU.doc.is_uif_enabled = "() return true if UIF is enabled"
function BU:is_uif_enabled()	return self.__b_uif_enabled		end
function BU:enable_uif()
	--self:box_debug( "enable_uif()" )
	self.__b_uif_enabled = true
end
function BU:disable_uif()
	--self:box_debug( "disable_uif()" )
	self.__b_uif_enabled = false
end

BU.doc.is_uif = "() return true if UIF is to be used"
function BU:is_uif()
	--aaa.print_fn()
	if not self.__b_uif_enabled then
		if self:get_uif_verbose() > 0 then
			self:print_debug( "UIF is_uif(): disabled" )
		end
		return false
	end

	--self:print( "uif testing shift "..BU.__b_uif_on_shift )
	--self:print( "BU:is_uif() aaa.keyboard.is_shift() "..BU.__b_uif_on_shift.." "..aaa.keyboard.is_shift() )
	if BU.__b_uif_on_shift then
		return aaa.keyboard.is_shift()
	end

	--self:print( "uif ok")
	return true
end


function BU:get_uif_bui()
	self:print( "BU have no BUI" )
end

-- Single source of truth for the Slide / Min-Max (or True-False) zones used
-- by sliders, BUIs and MU. Callers pass their preferred dists; the angle and
-- dangle stay shared so all UIF menus look the same. Each zone is set only
-- if not already present so a caller can pre-fill its own variant first.
function BU:add_uif_zones_slide( zones, b_bool, dist_min_max, dist_slide )
	dist_min_max = dist_min_max or .3
	dist_slide   = dist_slide   or .5
	if not (self.is_trig and self:is_trig()) then
		-- justifcation seem to be not working
		local y = 0
		if b_bool then
			zones.False	= zones.False	or {	angle=1/2-1/32,	dangle=3/16,	dist=dist_min_max,	rect={0,y,2,1},		justify="right" }
			zones.True	= zones.True	or {	angle=0+1/32,	dangle=3/16,	dist=dist_min_max,	rect={0.8,y,1.8,1},	justify="left" }
		else
			zones.Min	= zones.Min		or {	angle=1/2-1/32,	dangle=3/16,	dist=dist_min_max,	rect={0,y,1.8,1},	justify="right" }
			zones.Max	= zones.Max		or {	angle=0+1/32,	dangle=3/16,	dist=dist_min_max,	rect={0.8,y,1.8,1},	justify="left" }
		end
	end
	zones.Slide	= zones.Slide	or {	angle=1/4,		dangle=10/32,	dist=dist_slide,	b_text_no_when_selected=true }
end
function BU:add_uif_zones_base( zones, dist, x,y )
	local SX,SY = 1.5,.8
	local DX = .1
	local SXT = SX*4 + DX*3
	x = x or -SXT/2
	y = y or 0
	x = x + SX/2
	DX = DX + SX

	zones.Name		=	{	type="button",		angle=3/4,		dist=dist,	rect={x,y,	SX,SY}	}
	x = x + DX
	zones.Values	=	{	type="button",		angle=3/4,		dist=dist,	rect={x,y,	SX,SY}	}
	x = x + DX
	zones.Dump		=	{	type="button",		angle=3/4,		dist=dist,	rect={x,y,	SX,SY}	}
	x = x + DX
	if ga.bu_doc then
		zones.Doc	=	{	type="button",		angle=3/4,		dist=dist,	rect={x,y,	SX,SY}	}
	end

end
function BU:get_uif_zones()
	--aaa.print_fn()
	if true then
		local zones = {}
		self:add_uif_zones_base( zones, 2 )
		--zones.ZO	= {	angle=3/4,			dangle=1/4,	dist=.7,	}
		return zones
	else
		-- 	self:print( "BU have no get_uif_zones()" )
	end
end

function BU:__process_uif_name( name )
	if name then
		-- force name to lower case
		name = string.lower(name)
		-- replace space by _
		name = string.gsub( name, " ", "_" )
		return name
	end
end
function BU:get_uif_name_sel( uif )
	-- when the mouse is on a sub zone, the command-level selection is the
	-- parent main zone that opened the sub (so do_uif_command receives the
	-- parent name, and the sub name remains in uif.name_sel for the handler)
	if uif.b_sub_sel and uif.sub_parent_name then
		return uif.sub_parent_name
	end
	return uif.name_sel
end
function BU:is_uif_name_sel( uif, name, b_sub )
	-- highlight matches the actually hovered zone, scoped: a sub zone only
	-- highlights when the hover is in sub, and a main zone only when in main
	return self:__process_uif_name(name) == uif.name_sel
	   and ((b_sub == true) == (uif.b_sub_sel == true))
end

function BU:do_uif_command( uif )
	local command = BU:get_uif_name_sel( uif )
	aaa.print_fn()

	-- local b_used = true 
	-- if	command == "dump"	then	self:dump( 2 )
	-- else							b_used = false 
	-- end

	local b_used = false
	return GABU_OBJ.do_uif_command_with_super( self, b_used, uif, BU )
end


function BU:begin_uif( x,y,	origin )
	--self:set_uif_verbose( 3 )
	--aaa.print_fn()
	--self:set_uif_verbose(1)
	--todouif add ui to control this
	if not self:is_uif() then
		return false
	end
	-- if not aaa.keyboard.is_shift() then
	-- 	return false
	-- end
	
	local time = self.__uif_time_end
	if time then
		if aaa.time.t_real > time + 1. then -- 0.2 sec had passed
			self.__uif_time_end = nil
			--self:print_error( "end blocking with tempo" )
		else
			--self:print_error( "Too soon to start UIF again" )
			if self:get_uif_verbose() > 0 then
				self:print_debug( "UIF begin_uif(): Too soon to start UIF again" )
			end
			return false
		end
	end

	if self:get_uif_verbose() > 0 then
		self:print_debug( "BU:begin_uif() from "..origin )
		--table.print( self, "BU" , 1 )
	end

	if self:get_uif_verbose() > 0 then
		self:print_debug( "UIF treat case __b_uif_outside "..self.__b_uif_outside )
	end

	if self.__b_uif_outside then	
		if origin ~= "mouse_move" then
			self.__uif_time_begin = aaa.time.t_real
			if self:get_uif_verbose() > 0 then
				self:print_debug( "origin "..origin.." not a move so can't begin uif" )
			end
			return false
		end

		if self:is_inside_xy_local( x,y ) then
			if self:get_uif_verbose() > 0 then
				self:print_debug( "inside BU so can't begin uif" )
			end
			return false
		end

		-- self.__uif_time_begin happeedn to be nil once
		local delay = self.__uif_time_begin and (aaa.time.t_real - self.__uif_time_begin) or 0
		if delay > .2 then
			if self:get_uif_verbose() > 0 then
				self:print_debug( "too long ( "..delay.." ) before going out " )
			end
			return false
		end
	else
		if origin ~= "action_down" then
			if self:get_uif_verbose() > 0 then
				self:print_debug( "not a down so can't begin uif" )
			end
			return false
		end
	end 

	local zones = self:get_uif_zones()
	--table.print( zones )
	if not zones then
		if self:get_uif_verbose() > 0 then
			self:print_debug( "UIF begin_uif(): No Zones" )
		end
		return false
	end
	
	--aaa.print_fn()
	if self:get_uif_verbose() > 0 then
		if origin then
			self:print( "BU:begin_uif() called for "..origin )
		else
			self:print( "BU:begin_uif()" )
		end
		if self:get_uif_verbose() > 1 then
			self:print_debug( "got zones Zones" )
			table.print( zones, "zones", 1 )
		end
	end

	local uif = { zones=zones }
	local bui = self:get_uif_bui()
	if bui then
		if bui.get_id_interacting then	--todo this a way to validate again that this a bui, we could do better: is derived from
			local id = bui:get_id_interacting()
			--self:print( "BU:begin_uif() with id interacting "..id  )
			bui.__uif_val_begin = { val_cano = bui:get_value_cano_clamped(id), val = bui:get_value(id) }
			uif.bui = bui
		else
			self:print_error( "BU:begin_uif() bui is not a BUI here" )
		end
	else
		self:print( "BU:begin_uif() no BUI" )
	end
	
	uif.time_begin = aaa.time.t_real
	self:set_uif_data( uif )
	ga:register_uif( self )	
	self:__make_uif_data( x,y, true )
	return true
end

function BU:end_uif( b_block_tempo, origin ) 
	if self:get_uif_verbose() > 0 then
 		if origin then
			self:print( "end_uif() called from "..origin )
		else
			self:print( "end_uif()" )
		end
	end

	local uif = self:get_uif_data()
	if uif then 
		local bui = uif.bui
		if bui then
			uif.bui = nil
			bui.__uif_val_begin = nil
		end
		self:set_uif_data( nil )
		ga:unregister_uif( self )
	else
		self:print_error( "in end_uif() but uif is not runningfor this BU" ) 
	end

	if b_block_tempo then
		self.__uif_time_end = aaa.time.t_real
	end
end

function BU:__find_zone_sel( zones, x,y, ac )
	local name_sel, zone_sel
	-- Build a sorted hit-test order: smallest rect zones tested first so when
	-- two rect zones overlap, the smaller one (which is drawn on top) wins.
	-- Panel-type zones are pure backdrops and are never reported as found.
	local order = {}
	for name, zone in PAIRS( zones ) do
		if zone.type ~= "panel" then
			local rect = zone.rect
			local area = (rect and rect[3] and rect[4]) and (rect[3] * rect[4]) or 0
			table.insert( order, { name=name, zone=zone, area=area } )
		end
	end
	table.sort( order, function(a, b) return a.area < b.area end )

	for _, entry in ipairs( order ) do
		local name, zone = entry.name, entry.zone
		local a = zone.angle
		if zone.dangle then
			local da = zone.dangle * .5
			local min = a - da
			local max = a + da
			if inside( ac, min,max ) or ( min < 0 and inside( ac, min+1, max+1 ) ) then
				name_sel, zone_sel = name, zone
				--self:print( "uif found by angle "..name )
			end
		else
			local lrbt = zone.lrbt
			if lrbt then
				--self:print( name.." "..x.." in ["..lrbt.l..", "..lrbt.r.." ]" )
				if inside( x, lrbt.l, lrbt.r ) and inside( y, lrbt.b, lrbt.t ) then
					name_sel, zone_sel = name, zone
					--self:print( "uif found by button "..name.." at "..x.." "..y )
					break
				end
			end
		end
	end
	return name_sel, zone_sel
end

function BU:__make_uif_data( x,y, b_begin )
--	aaa.print_fn()
	local uif = self:get_uif_data()
	uif.b_in_main = true
	uif.b_sub_sel = false

	x,y = self:convert_xy_local_to_top( x,y )
	local x_begin,y_begin
	if b_begin then
		x_begin,y_begin = x,y
		uif.x_begin,uif.y_begin = x_begin,y_begin
	else
		x_begin,y_begin = uif.x_begin,uif.y_begin
	end
	uif.x,uif.y = x,y

	--table.print( uif, "uif", 2 )

	local dx = x - x_begin
	local dy = y - y_begin
	--aaa.print( "UIF dx, dy : "..dx..", "..dy )
	local d = math.sqrt( dx*dx + dy*dy )

	-- Whether the mouse currently sits inside the sub bbox (xy_sxy). Computed
	-- once here and reused below: the search prefers the sub when in bbox, the
	-- post-search keep-alive uses it to decide whether to drop a transient sub.
	local sub = uif.sub
	local b_in_sub_bbox = false
	if sub and sub.xy_sxy then
		local DY = UIF_DY
		local sub_x, sub_y, sub_sx, sub_sy = sub.xy_sxy[1], sub.xy_sxy[2], sub.xy_sxy[3], sub.xy_sxy[4]
		local sub_l = uif.x_begin + (sub_x - sub_sx*.5) * DY
		local sub_r = uif.x_begin + (sub_x + sub_sx*.5) * DY
		local sub_b = uif.y_begin + (sub_y - sub_sy*.5) * DY
		local sub_t = uif.y_begin + (sub_y + sub_sy*.5) * DY
		if x >= sub_l and x <= sub_r and y >= sub_b and y <= sub_t then
			b_in_sub_bbox = true
		end
	end

	local zones = uif.zones
	local name_sel, zone_sel
	if d < UIF_NEUTRAL and (zones.Slide or (zones.Max and zones.Min) or (zones.True and zones.False) ) then
		zone_sel = nil
		name_sel = nil
	else
		local ac = math.atan2( dy, dx ) * math.pi2_over
		if ac < 0 then ac = ac + 1. end
		--aaa.show( ac, "angle" )
		--self:print( name.." "..x.." "..y )

		-- search sub first so the panel wins over any wide-arc main zone
		if sub then
			name_sel, zone_sel = BU:__find_zone_sel( sub.zones, x,y, ac )
			if zone_sel then
				uif.b_in_main = false
				uif.b_sub_sel = true
			end
		end

		-- only search main zones when the mouse is outside the sub bbox so wide-arc
		-- main zones (Focus, Lua, Min, Max, Slide ...) cannot steal a click that
		-- belongs to the panel area (gaps between sub buttons)
		if not zone_sel and not b_in_sub_bbox then
			if dy > UIF_DY and zones.Slide then	--hackuif this not generic
				name_sel, zone_sel = "slide", zones.Slide
			else
				name_sel, zone_sel = BU:__find_zone_sel( zones, x,y, ac )
			end
		end

		-- mouse inside sub bbox but on no sub button: keep sub alive in the
		-- "in sub" state so the overlay stays and main is dimmed
		if b_in_sub_bbox and not zone_sel then
			uif.b_in_main = false
		end
	end
	if zone_sel then
		local sub_def = zone_sel.sub_def
		if sub_def and self.define_uif_sub then
			self:define_uif_sub( sub_def )
			uif.sub_parent_name = self:__process_uif_name( name_sel )
		elseif uif.sub and uif.sub.b_transient and uif.b_in_main then
			-- transient sub: hovering a main zone without sub_def closes it
			uif.sub = nil
			uif.sub_parent_name = nil
		end
	else
		-- nothing hit: transient subs auto-close when the mouse leaves their bbox.
		-- Persistent subs (e.g. BU_TEXTURE grid) stay alive until UIF ends or
		-- another sub_def is hovered.
		if uif.sub and uif.sub.b_transient and uif.sub.xy_sxy then
			if b_in_sub_bbox then
				uif.b_in_main = false
			else
				uif.sub = nil
				uif.sub_parent_name = nil
			end
		end
	end
	uif.zone_sel	= zone_sel
	name_sel = self:__process_uif_name(name_sel)
	uif.name_sel	= name_sel

	local bui = uif.bui
	if bui then
		local id = bui:get_id_interacting()
		if name_sel then
			--self:print( "uif found "..name_sel )
			if name_sel=="slide" then
				local du	
				dy = dy - UIF_DY
				uif.d_manual = math.sqrt( dx*dx + dy*dy )
				local v = uif.d_manual - UIF_MANUAL_SIZE_MIN
				if v > 0 then
					local ac = math.atan2( dy, dx ) * math.pi2_over
					if ac < 0 then ac = ac + 1. end

					v = v / ( UIF_MANUAL_SIZE_ACTIVE - UIF_MANUAL_SIZE_MIN )
					if v > 1 then
						v = 1
					end
					du = interpolate( uif.du_last or 0, -ac + .25, v )
					--if uif.d_manual > UIF_MANUAL_SIZE_MIN then
					--	du = -ac + .25
				else
					du = 0
				end
				--aaa.show( v, "v is ", nil, 0 )
				if aaa.keyboard.is_ctrl() then
					du = du * .125
					if aaa.keyboard.is_alt() then
						du = du * .125
					end
				end
				uif.__du = du
			end

			if		name_sel=="min"	or name_sel=="false" then
				--self:print( "--- command Min/false" )
				bui:set_value_cano( 0, id )
			elseif	name_sel=="max"	or name_sel=="true" then
				--self:print( "--- command Max/true" ) 
				bui:set_value_cano( 1, id )
			elseif name_sel=="slide"	then
				if bui.__get_uif_u then
					local u = bui:__get_uif_u( uif )
					bui:__set_from_u( u, id )
				end
			end
		elseif not zone_sel then	-- when no zone no button we set itback to value at uif begin
			local val_begin = bui.__uif_val_begin
			if val_begin then
				bui:set_value_cano( val_begin.val_cano, id )
			else
				self:print_error( "BU:__make_uif_data() no val_begin in bui for reset" )
			end
		end
	end

	--todo remove when not used
	--todo reuse table
end

--todo deal with double interaction
function BU:draw_uif_slide( ox,oy )
	--if true then return end
	if not self.__blob_cur then return end	--in case of interuption by bug

	local uif = self:get_uif_data()
	local bui = uif.bui
	if not bui then
		self:print_error( "BU:draw_uif_slide no BUI target" )
		return
	end

	--local bx,by = self.__blob_cur[1].x, self.__blob_cur[1].y
	--local x,y = self:convert_xy_bus_up_to_local( bx,by )
	--self:print( "do_mouse_move( "..x..", "..y.." )" )
--	self:do_mouse_move( x,y )

	local CSX,CSY	= .1,.1

	local id = bui:get_id_interacting()
	--d = d or 0
	local x,y = uif.x,uif.y
	local xb,yb = ox + uif.x_begin, oy + uif.y_begin

	local si = UIF_MANUAL_SIZE_MIN * 2
	local se = UIF_MANUAL_SIZE_ACTIVE * 2
	local size = (uif.d_manual or 0) * 2
	size = math.max( size , se )

	gol.set_line_smooth( true )
	--aaa.draw_cube(	x, y, 0, si )
	--	gol.push_matrix()
	--		gol.translate( x, y )
	--gol.scale( self:get_ratio_y(), self:get_ratio_x() )
	--gol.color_white()
	local val_begin = bui.__uif_val_begin
	local cur = val_begin.val --hackuif
	if type(cur) == "boolean" then
		cur = cur and 1 or 0
	end	
	local u_begin = val_begin.val_cano --hackuif
	local u = bui:get_value_cano_clamped( id )
	--gol.color_black( .8 )
	--self:gol_meter_color( .25 )
	local fa = 0.25
	local sa = 0.25 + u_begin*fa

	gol.set_line_width( 1 )
	self:gol_color_back()
	aaa.draw_disk_arc_axe_z(	xb,yb, 0, size,		sa-fa, fa,			18 )

	bui:gol_meter_color( )	--hackuif
	aaa.draw_disk_arc_axe_z(	xb,yb, 0, size,			sa-u*fa, u*fa,		18 )

	local c = self:get_uif_color( 1, true )
	local coli = {	c[1],	c[2],	c[3],	c[4]*.6	}
	local cole = {	c[1],	c[2],	c[3],	c[4]*.1	}
	gol.color( coli )
	aaa.draw_disk_arc_axe_z	(			xb,yb, 0, si,			sa-fa, fa,				12 )
	aaa.draw_annulus_color_gradient(	xb,yb, si*.5, se*.5,	sa-fa, sa,  coli, cole,	12 )

	gol.color( UIF_COLOR_SELECT )
	gol.set_line_width( BU.__draw_text_line_width * .75 )
	aaa.draw_line( xb,yb, uif.x,uif.y )

	local defs = bui:get_values_def_table()
	local nb = #defs
	if nb then
		local min, max = bui:get_min_max()
		for i=0,#defs do
			local def = defs[i]
			local xdef = u_begin + i*.25
			if def ~= nil then
				local span = ( max-min )
				if cur then
					local angle = math.abs( def-cur ) / span * .5 * math.pi
					if ( def-cur < 0 ) then
						angle = -angle
					end
					local dx = math.cos( math.pi * .5 - angle )
					local dy = math.sin( math.pi * .5 - angle )
					local dxs = dx * size * .5
					local dys = dy * size * .5
					gol.color( UIF_COLOR_DEF_VALUE )
					aaa.draw_line_uv( xb,yb, dxs,dys )
					self:draw_uif_text( bui:format_value(def),        xb+dxs+dx*.1-CSX, yb+dys+dy*.1, CSX*.8, CSY*.8, UIF_COLOR_DEF_VALUE )
				else
					self:print_error( "BU:draw_uif_slide() no cur value" )
				end
			end
		end
	end

	gol.color( UIF_COLOR_CURRENT )
	local oys = size*.5 + CSY
	aaa.draw_line_uv( xb,yb, 0, oys )
	gol.color_black( 1. )
	aaa.draw_circle_arc_axe_z(	xb,yb, 0, si,	sa-fa-1/40, fa+1/20,	12 )
	aaa.draw_circle_arc_axe_z(	xb,yb, 0, se,	sa-fa-1/40, fa+1/20,	12 )

	self:set_uif_text_attr()

	self:draw_uif_text( bui:format_value( cur ),					xb,		yb + oys -.1,	CSX, CSY,    	UIF_COLOR_CURRENT )
	self:draw_uif_text( bui:get_text(),								x-CSY,	y+2.2*CSY,		CSX, CSY,		UIF_COLOR_SELECT )
	self:draw_uif_text( bui:format_value( bui:get_value( id ) ),	x-CSY,	y+CSY - .3,		CSX, CSY,		UIF_COLOR_SELECT )
	--	gol.pop_matrix()
end


function BU:is_uif_running() 	return self.__uif_data ~= nil end
function BU:get_uif_data()		return self.__uif_data	end
function BU:set_uif_data( t )	self.__uif_data = t		end

function BU:define_uif_sub( sub_def )
	self:get_uif_data().sub = sub_def
end

function BU:get_uif_color( alpha, b_active )
	local c = self:get_color_named( (b_active or b_active==nil) and "uif_active" or "uif" )
	if not alpha or alpha >= 1 then		return c
	else								return { c[1], c[2], c[3], c[4]*alpha }
	end
end
function BU:set_uif_color( alpha, b_active )
	gol.color( self:get_uif_color( alpha, b_active ) )
end

--todo make this a generic fn (no uif) passing color instead of active
function BU:draw_uif_text( str, x,y, sx,sy, color )
	if not str then return end

	local to = self.__text_obj
	to:set_next_shadow()
	gol.color( color )
	self:draw_text( str, x,y, sx,sy )
end

function BU:set_uif_text_attr()
	gol.set_line_width( 2 )
end

function BU:__get_uif_button_sx( rect )	return UIF_DY * 4	end
function BU:__get_uif_button_sy( rect )	return UIF_DY		end

function BU:__get_uif_rect_def( rect )
	local DY = UIF_DY
	return	((rect and rect[1]) or 0) * DY,
			((rect and rect[2]) or 0) * DY,
			((rect and rect[3]) or 4) * DY,
			((rect and rect[4]) or 1) * DY
end

--todo merge with draw path
function BU:__process_uif_def( x,y, def )
	--self:print( "sel is "..BU:get_uif_name_sel( uif ).." : "..name.." "..b )
	local l,r, b,t
	local c,s
	if def.angle then
		local a = def.angle * math.pi2
		-- local da = def.dangle and (def.dangle * math.pi2 * .5) or nil
		c,s = math.cos(a),math.sin(a)
		-- --draw arrows
		-- if da then
		-- 	set_color( b_sel, .7 )
		-- 	fn(	x + rm * c,			y + rm * s,
		-- 		x + rm*cos(a-da),	y + rm*sin(a-da),
		-- 		x + r * c,			y + r * s,
		-- 		x + rm*cos(a+da),	y + rm*sin(a+da)
		-- 		)
		-- end
	else
		c,s = 0,0
	end
			

	--if not def.b_text_no and not (def.b_text_no_when_selected and b_sel) then
		--self:draw_text_visible( name,	x+c*2,	y+s*2,	r*4, r,	true )
		--local f = 2.5
	local just = def.justify or "center"
	local dist = UIF_DY * ( 1. + (def.dist or 1.) )	-- size are relative to the base size of the UIF
	local xt = x + c*dist
	local yt = y + s*dist

	local rx,ry, rsx,rsy = self:__get_uif_rect_def( def.rect )

	xt = xt + rx + (def.dx or 0)
	yt = yt + ry + (def.dy or 0)
	return xt-rsx/2, xt+rsx/2, yt-rsy/2, yt+rsy/2
end

function BU:__set_color_uif_back()
	gol.color( 0, .25, .25, .8 )
end
function BU:__draw_uif_back( x,y )
	--aaa.print_fn()

	local uif = self:get_uif_data()

	if not uif.x_left then
		-- local rm = UIF_NEUTRAL

		-- --todo find pixel size
		-- --gol.color_black()

		local dx = .01
		local dy = -dx
		local zones = uif.zones
		local FAR = 1000000
		local gl,gr, gb,gt = FAR,-FAR, FAR,-FAR
		
		-- make size of of rect including all the element of UIF
		for name,def in PAIRS( zones ) do
			local l,r, b,t = self:__process_uif_def( x,y, def )
			gl = math.min( gl, l )
			gr = math.max( gr, r )
			gb = math.min( gb, b )
			gt = math.max( gt, t )
		end

		local x_margin = .05
		gl = gl - x_margin
		gr = gr + x_margin
		local y_margin = x_margin
		gb = gb - y_margin
		gt = gt + y_margin

		uif.x_left,uif.x_right	= gl,gr
		uif.y_bottom,uif.y_top	= gb,gt
	end

	local l,r = uif.x_left,uif.x_right
	local b,t = uif.y_bottom,uif.y_top

	local x,y = (l+r)*.5, (b+t)*.5
	local dt = clamp_01( (aaa.time.t_real - uif.time_begin) * 12. ) * .5
	local sx,sy = dt*(r-l),dt*(t-b)
	local l,r = x-sx,x+sx 
	local b,g = y-sy,y+sy
	
	uif.text_sy = .15
	local y_top = t + uif.text_sy
	uif.text_y = t

-- BACK RECT
	self:__set_color_uif_back()
	aaa.draw_rect(		l,b, r,y_top )

	gol.set_line_width( BU.__draw_text_line_width )
-- FRAME
	gol.color_magenta()
	aaa.draw_rect_line(	l,b, r,y_top )
	-- line under title
-- LINE UNDER TITLE
	aaa.draw_line(		l,t, r,t )
end

--todofont regroup at lower level to optimize
function aaa.img.draw_lbz_size( bind, l,b,z, size, b_frame, b_info )
	if bind then
		local sx,sy

		local psx,psy, ch_nb = aaa.img.get_size_channel( bind )
		if psx then
			if psx > psy then	sx,sy = 1, psy / psx
			else				sx,sy = psx / psy, 1
			end	
			sx,sy = sx*size, sy*size
		else
			 sx,sy = 1,1
		end

		local x,y = l+sx/2,b+sy/2

		local dim = gol.get_texture_dim()

			if b_frame then
				--local lw = gol.get_line_width()
					gol.set_line_width( BU.__draw_text_line_width * 3 ) 
					gol.color_magenta()
					gol.set_texture_dim(0)
					aaa.draw_rect_line_axe_z( x,y,z, sx,sy )
				--gol.set_line_width(lw)
			end
			
			gol.set_texture_dim(2)
			gol.bind_texture(bind)
			gol.set_quad_uv()
			gol.color_white()
			aaa.draw_rect_uv_axe_z( x,y,z, sx,sy )

			if b_info then
				size = size / 12
				local name	= TEXS:get_name_from_bind( bind )
				gol.color_cyan()
				BU:draw_text_nice_shadow( name.."@"..bind,			l,b+size,z,	size,size, "left", "left" )
			
				gol.color_yellow()
				BU:draw_text_nice_shadow( psx.."x"..psy.." "..aaa.img.get_format_name( bind ),	l,b,z,		size,size, "left", "left" )
			end

		gol.set_texture_dim(dim)
		
	end
end

function BU:__draw_uif_zones( uif, zones, b_sub, x,y, dx,dy )
	-- text style is set per-zone below (bind_flat -> glut/line text for textures,
	-- everything else -> the default nice/shadow text used by main buttons)

	local r = UIF_DY
	local rn = UIF_NEUTRAL
	--local ri = r * .25
	local sin,cos = math.sin,math.cos
	--local fn = i==1 and gol.draw_line_strip_2d or gol.draw_triangle_fan_2d
	local fn = gol.draw_triangle_fan_2d
	local ph = math.sin( aaa.time.t_real*6.28*3 )

	local function set_color( b_sel, alpha )
		self:set_uif_color( b_sel and 1 or alpha, b_sel )
	end

	-- pre-pass: panel backdrops drawn first so other zones layer on top.
	-- A panel mimics the UIF back chrome: filled rect, magenta frame, and a
	-- title strip ADDED ON TOP (outside the panel rect, not eating its content
	-- area), like __draw_uif_back. This is one nesting level today; the same
	-- shape can be reused later if UIF turns into a recursive dynamic menu.
	for name,def in PAIRS( zones ) do
		if def.type == "panel" and not def.b_text_no then
			local pl, pr, pb, pt = self:__process_uif_def( x,y, def )
			local title_sy = uif.text_sy or 0.15
			-- title strip sits above the panel rect, outside the content area
			local title_t = pt + title_sy
			-- lrbt covers the whole panel chrome (content + title strip)
			def.lrbt = { l=pl, r=pr, b=pb, t=title_t }

			-- fill content + title strip with the same color
			if def.color then	gol.color( def.color )
			else				self:__set_color_uif_back()
			end
			aaa.draw_rect( pl, pb, pr, title_t )

			-- outer frame + separator at the top of the content (= bottom of title)
			gol.set_line_width( BU.__draw_text_line_width )
			gol.color_magenta()
			aaa.draw_rect_line( pl, pb, pr, title_t )
			aaa.draw_line( pl, pt, pr, pt )

			-- title text: explicit def.title wins, otherwise fall back to the
			-- parent zone name that opened this sub (passed via uif.sub_parent_name)
			local title = def.title or uif.sub_parent_name or name
			if title and title ~= "" then
				local oy = (title_t - pt) / 8
				gol.color_white()
				self:draw_text_lrbt( title, pl, pr, pt+oy, title_t, "center", true )
			end
		end
	end

	-- build a draw order: largest rects first so smaller overlapping rects layer on top
	local draw_order = {}
	for name,def in PAIRS( zones ) do
		if def.type ~= "panel" then	-- panels handled in pre-pass above
			local rect = def.rect
			local area = rect and ((rect[3] or 4) * (rect[4] or 1)) or 0
			table.insert( draw_order, { name=name, def=def, area=area } )
		end
	end
	table.sort( draw_order, function(a, b) return a.area > b.area end )

	local texture_tmp
	for _, entry in ipairs( draw_order ) do
		local name, def = entry.name, entry.def
		local c,s
		local a,da
		if def.angle == nil then
			c,s = 0,0
		else
			a = def.angle * math.pi2
			-- local da = def.dangle and (def.dangle * math.pi2 * .5) or nil
			c,s = math.cos(a),math.sin(a)
			da = def.dangle and (def.dangle * math.pi) or nil
		end
		local b_sel = self:is_uif_name_sel( uif, name, b_sub )

		-- draw arrows da mean thisis an angular zone
		if da then
			set_color( b_sel, .7 )
			fn(	x + c         * rn,	y + s         * rn,
				x + cos(a-da) * rn,	y + sin(a-da) * rn,
				x + c         * r,	y + s         * r,
				x + cos(a+da) * rn,	y + sin(a+da) * rn
				)
		end
		
		if not def.b_text_no and not (def.b_text_no_when_selected and b_sel) then
			-- main-style rendering for any plain button (no sub_def trigger, no
			-- bind texture) regardless of whether it is in main zones or sub.zones
			local b_sub_no_local = not def.sub_def and not def.bind_flat
			if b_sub then
				if def.bind_flat then	self:set_text_glut()
				else					self:set_text_nice()
				end
			end

			--self:draw_text_visible( name,	x+c*2,	y+s*2,	r*4, r,	true )
			local just = def.justify or "center"
			local dist = ( 1. + (def.dist or 1.) ) * r	-- size are relative to the base size of the UIF
			local xt = x + c * dist
			local yt = y + s * dist

			local rx,ry, rsx,rsy = self:__get_uif_rect_def( def.rect )
			xt = xt + rx + (def.dx or 0)
			yt = yt + ry + (def.dy or 0)
			local l,r, b,t = xt-rsx*.5, xt+rsx*.5, yt-rsy*.5, yt+rsy*.5

			if def.type == "frame" then
				if not def.b_above then
					-- below-frames render here, layered with the buttons by area;
					-- frames with b_above are drawn in the post-pass so they sit
					-- on top of all the sub buttons
					gol.set_line_width( BU.__draw_text_line_width )
					if def.color then gol.color( def.color ) else gol.color_white() end
					aaa.draw_rect_line( l,b, r,t )
				end
			elseif def.type == "panel" then
				-- handled in pre-pass
			else
				--self:print( name..": ".. t-b)
				--t = t + .125
				local bind = def.bind_flat
				if da then
					def.lrbt = nil
				else
					-- we store size for testing with mouse position
					def.lrbt = { l=l,r=r, b=b,t=t }
					-- then draw
-- BACK_COLOR
					--self:print( name.." lr is ["..def.lrbt.l..", "..def.lrbt.r.." ]" )
					if b_sub_no_local then
						if b_sel then	gol.color_green( .7 )
						else			--gol.color_red( .65 + ph * .15 )
										gol.color_black( .7 )
										--gol.color( BU:get_color_back_named( "action" ) )
						end
					else
						if bind then
							local isx = aaa.img.get_size( bind )
							if not isx then
								bind = nil
							end
							if bind then
								gol.color( 0,b_sel and .75 or .5,0, .8 )
							else
								gol.color( b_sel and .75 or .5,0,0, .8 )
							end
						else
							local b_green
							local sub = def.sub_def
							if sub then
								b_green = uif.sub and uif.sub.group == sub.group
							end
							if b_green then
								gol.color_green()
							else
								gol.color( b_sel and .75 or .5,0,0, .8 )
							end
						end
					end
-- BACK
					aaa.draw_rect( l,b, r,t )
				end			
-- FRAME
				if b_sel and not da then
					gol.set_line_width( BU.__draw_text_line_width * .5)
					gol.color_magenta()
					aaa.draw_rect_line( l,b, r,t )
					gol.color_white()
				end

				if bind then
					if b_sel then
						texture_tmp = { x=(l+r)/2, y=(b+t)/2, l=r, b=t, bind = bind }
					end

					local s = math.min( rsx,rsy )
					aaa.img.draw_lbz_size( bind, l,b,0, s, false, false )

					if rsy < rsx then
						l = l + math.min( s, rsx*.5 )
					else
						b = b + math.min( s, rsy*.5 )
					end
				end
-- BUSY CROSS
				-- diagonal cross marking a slot button whose BU_MEU already has a
				-- MEU attached (b_empty == false on the def). Drawn after the
				-- background and frame, before the text, so the label stays on top.
				if def.b_empty == false then
					gol.set_line_width( BU.__draw_text_line_width * 2 )
					gol.color_cyan( .7 )
					-- inset the cross by 10% on each side -> 20% smaller than the button
					local dxc = (r-l) * 0.1
					local dyc = (t-b) * 0.1
					aaa.draw_line( l+dxc, b+dyc, r-dxc, t-dyc )
					aaa.draw_line( l+dxc, t-dyc, r-dxc, b+dyc )
				end
-- TEXT
				l,r, b,t = xt-rsx*.45, xt+rsx*.45, yt-rsy*.3, yt+rsy*.5
				if b_sub_no_local then
					local to = self.__text_obj
					to:set_next_shadow()
					--local o = rsy * .1-- visually ajusted
					to.__shadow_du = 1
					to.__shadow_dv = -1
					--self:print( name )					
					set_color( b_sel, 1. )
					--gol.color_back()
					self:draw_text_lrbt( name, l,r, b,t, just, true )
				else
					local o
					if b_sel then
						local to = self.__text_obj
						to:set_next_shadow()
						if not b_sub then	-- text is nice
							o = rsy * .1 -- visually ajusted
							to.__shadow_du = -1.00001 --strange bug with one it is reversed
							to.__shadow_dv = 1
						else -- text is line
							o = rsy * .05
							to.__shadow_du = -o
							to.__shadow_dv = o
						end
					else
						o = 0
					end
					gol.color_white()
					self:draw_text_lrbt( name, l+o,r+o, b-o,t-o, just, true )
				end
			end
		end
	end

	-- draw texture on top
	if texture_tmp then
		local s = 1.2
		aaa.img.draw_lbz_size( texture_tmp.bind, texture_tmp.l,texture_tmp.b,0, s, true, true )
	end

	-- post-pass: above-frames render last, so they sit on top of every sub button
	for name,def in PAIRS( zones ) do
		if def.type == "frame" and def.b_above then
			local pl,pr, pb,pt = self:__process_uif_def( x,y, def )
			gol.set_line_width( BU.__draw_text_line_width )
			if def.color then gol.color( def.color ) else gol.color_white() end
			aaa.draw_rect_line( pl, pb, pr, pt )
		end
	end
end

function BU:__draw_uif_base( x,y )
	--aaa.print_fn()

	self:__draw_uif_back( x,y )

	local uif = self:get_uif_data()

	-- UIF title drawn right after the back chrome so it sits in z-order behind
	-- the main zones, sub panel and overlay. The panel may visually cover it.
	do
		local str = self:get_uif_name()
		local l,r = uif.x_left, uif.x_right
		local b = uif.text_y
		local t = b + uif.text_sy
		local oy = (t-b)/8
		gol.color_white()
		self:draw_text_lrbt( str, l,r, b+oy,t, "center", true )
	end

	--todo find pixel size
	--gol.color_black()
	local dx = .01
	local dy = -dx

	-- draw_zones
	self:__draw_uif_zones( uif, uif.zones, false, x,y, dx,dy )
	if not uif.b_in_main then
		gol.color( 0, 0, 0, .5 )
		aaa.draw_rect( uif.x_left, uif.y_bottom, uif.x_right, uif.y_top )
	end
	local sub = uif.sub
	if sub then
		--self:print( "draw_sub" )
		--table.print( sub, "sub_uif", 3 )
		self:__draw_uif_zones( uif, sub.zones, true, x,y, dx,dy )
	end

end

function BU:get_uif_name()
	return self:get_name()
end

--todo deal with double interaction
function BU:draw_uif()
	local uif = self:get_uif_data()
	if not uif then return end

	gol.reset()
	--BU.__s_text_type = 3
	local tt_pushed = rawget( self, "__s_text_type" )
	self:set_text_nice()
	--BU:set_text_justification( "center" )	--todofont try to compensate for another bug

		self:__draw_uif_base( uif.x_begin, uif.y_begin )

		local name_sel = BU:get_uif_name_sel( uif )
		if name_sel == "slide" then
			self:draw_uif_slide( 0, UIF_DY )
		end

		self:set_uif_color( 1, name_sel~=nil )
		--gol.color( UIF_COLOR_SELECT )
		aaa.draw_null( uif.x, uif.y, 0, UIF_DY * (1.2+.2*math.sin(aaa.time.t_real*10)) )

	self:__set_text_type( tt_pushed )
end
