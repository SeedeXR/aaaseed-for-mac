CLASS.DECLARE( "SKUI" )

function SKUI:create( name )
	local self = SKUI:create_instance( name )
	self:set_active( true )
	return self
end


--	VISIBLE	:	do we display ? basicly
function SKUI:set_visible(b)		self.b_visible = b				end
function SKUI:is_visible()		return self.b_visible			end

function SKUI:update()
	if self:is_active() then
		self:update_low()
	end
end
function SKUI:draw()
	if self:is_active() and self:is_visible() then
		self:draw_low()
	end
end
function SKUI:updraw()
	self:update()
	self:draw()
end

CLASS.DECLARE( "SKUI_ARROW", SKUI )

function SKUI_ARROW:create( name, skel )
	local self = SKUI_ARROW:create_instance( name )
	self.skel = skel
	self.arrow_state = 0	--	-1 left, 0 none, 1 right
	self.b_hori = true
	self:set_active( true )
	self:set_visible( false )
	return self
end

function SKUI_ARROW:set_base( node_central, size, z_offset )
	self.node_central = node_central
	self.size = size
	self.z_offset = z_offset
end

function SKUI_ARROW:set_visible( b )
	if self:is_visible() ~= b then
		oo.getsuper(oo.getclass(self)).set_visible( self, b )
		if not b then
			self:set_arrow( 0 )
			self:set_armed( true )
		end
	end
end

function SKUI_ARROW:set_armed( b )	self.b_armed = b		end
function SKUI_ARROW:is_armed( b )	return self.b_armed		end

function SKUI_ARROW:set_but_method( tab, method_name )
	self.but_table = tab
	self.method_name = method_name
end
function SKUI_ARROW:set_arrow( v )
	self.arrow_pos = v
	if		inside( v, -2, -1 )	then	v = -1
	elseif	inside( v, 1, 2 )	then	v = 1
	else								v = 0
	end
	if self.arrow_state ~= v then
		self.arrow_state = v
		if self.but_table and self.method_name then
			self.but_table[ self.method_name ]( self.but_table, v )
		end
	end
end

--todo make it generik
function SKUI_ARROW:is_inside( node )
--	self:print( "is_inside( "..node.." )" )
	local skel = self.skel
	local cen	= skel:get_point_cur( self.node_central )
	local pt	= skel:get_point_cur( node )
	local v, s
	--	Y
		v = pt[2] - cen[2]
		s = self.size * 1.5 + .05
		if outside( v, -s, s ) then return nil end
	--	Z
		v = pt[3] - (cen[3] + self.z_offset)
		if outside( v, -s, s ) then return nil end
	--	X
		v = pt[1] - cen[1]
		s = self.size * 2.5 + .05
		if outside( v, -s, s ) then return nil end
	--	ok now we are inside
		s = self.size * 0.5
		v = v / -s
		--aaa.mess.show( v )
	return v
end

function SKUI_ARROW:update_low()
	--aaa.mess.show( "update low" )
	local skel = self.skel
	local l = skel:is_gesture( "arm_straight_left" )
	local r = skel:is_gesture( "arm_straight_right" )
	local b_vis = false
	if l or r then
		--aaa.mess.show( "bras tendu" )
		l = l and self:is_inside( "hand_left" )
		r = r and self:is_inside( "hand_right" )
		if l or r then
			--aaa.mess.show( "INSIDE" )
			b_vis = true
			local v = l or r
			self:set_arrow( v )
		end
	end
	self:set_visible( b_vis )
end

function SKUI_ARROW:select_draw_fn( b )		return b and aaa.draw_arrow or aaa.draw_arrow_line	end
function SKUI_ARROW:draw_low_one_axe( x, y, z )
	local s = self.size
	self:select_draw_fn( self.arrow_state < 0 )( x+s, y, z,  s, s )
	self:select_draw_fn( self.arrow_state > 0 )( x-s, y, z, -s, s )

	local dx = -self.arrow_pos * s * 0.5
	gol.draw_line_loop_2d(	dx,	 s*2,
					 		dx,	-s*2 )
end

function SKUI_ARROW:draw_low()
	gol.color_green_80()
	gol.set_line_width( 3 )

	local ne = self.skel:get_point_cur( self.node_central )
	local x, y, z = ne[1], ne[2], ne[3]

	gol.push_matrix()
		gol.translate( x, y, z )
		if self.b_hori then
			self:draw_low_one_axe( 0, 0, 0 )
		end
		if self.b_vert then
			gol.rotate_z( -.25 )
			self:draw_low_one_axe( 0, 0, 0 )
		end
	gol.pop_matrix()
end


