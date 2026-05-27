
local mdh_gene_pt = {
				{ "inter", 1, 2, .4 },
				{ "inter", 2, 21, -.1 },
				21,
				{ "inter", 3, 4, 1. },
				5,	6, 	7,
				9,	10,	11,
				13, 14, 15,
				17, 18, 19,
				}
local mdh_seg = {
				{ 3, 5, 6, 7 },
				{ 3, 8, 9, 10 },
				{ 1, 11, 12, 13 },
				{ 1, 14, 15, 16 },
				{ 11, 14 },
				{ 1, 2 },
				{ 2, 5 },
				{ 2, 8 },
				{ 3, 4 },
				}
local mdh_seg_dist_leg1 = { 13, 12, 11, 1 }
local mdh_seg_dist_leg2 = { 16, 15, 14, 1 }
local mdh_seg_dist_top = { 1, 2, 3, 4 }

local mdh_seg_dist_leg1_src	= { 17, 18, 19 }
local mdh_seg_dist_leg2_src	= { 13, 14, 15 }
local mdh_seg_dist_top_src	= { 1, 2, 3, 4 }

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()

	local ix, iy = 1, 1
	self:add_button( {ix, iy },		"3D",		self, "b_3d",		true )
	--self:add_button( {ix, iy+1 },		"Z",	self, "b_z",		true )
	iy = iy+2
	self:add_slider(	{ix,iy,	8,1},	"Resize", 	self, "resize", 3.8, 1, 5 )
	iy = iy+1
	self:add_button( {ix, iy },		"Segment",	self, "b_seg",		true )
	self:add_button( {ix, iy+1 },		"dot", 		self, "b_dot",		true )
	self:add_button( {ix, iy+2 },		"Number",	self, "b_number",	false )
	self:add_button( {ix, iy+3 },		"Null",		self, "b_null",		false )
	self:add_button( {ix, iy+4 },		"Color",	self, "b_color",	false )

	ix, iy = 9, 4
	self:add_slider(	{ix,iy,	8,2},	"Inter", 	self, "inter", 1, 0, 1 )
end
function meu:init()
	self.skels = { }
	for i=1,2 do
		local skel = {}
		skel.__pl_src	= POINT_LIST:create()
		skel.__pl 		= POINT_LIST:create()
		skel.__seg		= SEGMENT_LIST:create( "Skel "..i )
		skel.height		= 0
		self.skels[i] = skel
	end
end

------------------------------
--		only used for uv edit
function meu:get_pt_xyz( id )
	local pl = self.skel.__pl
	if id <= pl:get_size() then
		local pt = pl:get_pt( id )
		return pt[1], pt[2], pt[3]
	end
	return 0, 0, 0
end
function meu:get_segs()		return self.skel.__seg		end
-----------------------------

local resize_table =
{
	{ {y=.25, f=3.6},	{f=4.2} },			--	1
	{ {y=.3, f=3.9},	{y=.05, f=4.1} },	--	2
	{ {f=4.1}, 			{y=.5,f=3.7} },		--	3
	{ {y=.1, f=4.1}, 	{y=.15, f=3.4} },	--	4
	{ {y=.25, f=3.9}, 	{y=.5, f=3.65} },	--	5
	{ {y=.25, f=3.9}, 	{y=.05,f=4.2} },	--	6
	{ {y=.05, f=3.3}, 	{y=.05,f=3.4} }		--	7
}
function meu:update_skel( id )
	local skel = self.skels[id]
	self.skel = skel
	local is = app.skel_used[id]

	--self:print( id.." "..is )
	if is==0 then
		skel.b_valid = false
	else
		skel.b_valid = true
		local bdd_mocap = self:get_meu_by_name( "K2Mocap_1" ):get_bdd_mocap( is )

		local src = skel.__pl_src
		src:set_interpolate_factor( self.inter )
		src:read_bdd( bdd_mocap )
		src:add_xyz( app.skel_x[id], 0, 0 )

		local pl = skel.__pl
		pl:build( src, mdh_gene_pt )
		skel.__seg:build( mdh_seg )
		skel.__seg:set_point_list( pl )
		local a, b, c
		if false then
			a = pl:mesure_seg_list( mdh_seg_dist_leg1 )
			b = pl:mesure_seg_list( mdh_seg_dist_leg2 )
			c = pl:mesure_seg_list( mdh_seg_dist_top )
		else
			a = src:mesure_seg_list( mdh_seg_dist_leg1_src )
			b = src:mesure_seg_list( mdh_seg_dist_leg2_src )
			c = src:mesure_seg_list( mdh_seg_dist_top_src )
		end
		--aaa.print( a, b, c )
		local d = math.max( a, b ) + c
		if math.is_nan( skel.height ) then
			skel.height = 1.
		end
		--aaa.print( skel.height )
		d = interpolate( skel.height, d, aaa.time.dt * 1. )
		skel.height = d
		--mdh filter d
		local f = 1./d
		local min, max = pl:get_min_max_z()
		pl:scale_xyz( -f, f, 1 )
		local y = pl:get_min_y()
		--self:print(  y )
		pl:add_xyz( 0, -y, -(min+max)*.5 )
		self.z_ref = pl:get_pt(1)[3]

		local resize = resize_table[app:get_level()][id]
		local f = resize.f or self.resize
		local x = ((id==1)and -1 or .8) + (resize.x or 0)
		local y = -3 + (resize.y or 0)
		--self:print( f, x, y )
		pl:mul_add( f, f, x, y )
		--self:print( id.." done "..is )
	end
end


function meu:update()
	self:update_skel( 1 )
	self:update_skel( 2 )
end
local function get_z( pt )
	return meu.b_3d and pt[3] or 0
end
function meu:draw_number( id, pt )
	local s = .15
	local o = s * .2
	aaa.draw_str_xyz( id, pt[1]+o*5, pt[2]+o, get_z( pt ), s, s )
end

function meu:draw_point_as_2disk( id, pt )
	local a = 1
	local s = .05
	gol.color_white( a )
	aaa.draw_disk_axe_z( pt[1], pt[2], get_z(pt), s )
	gol.color_yellow( a )
	aaa.draw_disk_axe_z( pt[1], pt[2], get_z(pt), s*.75 )
end
function meu:draw_line( a, b )
	gol.vertex3( a[1], a[2], get_z(a), b[1], b[2], get_z(b) )
end
function meu:draw_seg( a_id, b_id )
	local pl = self.skel.__pl
	local a = pl:get_pt( a_id )
	local b = pl:get_pt( b_id )
	self:draw_line( a, b )
end
function meu:draw_segs()
	gol.begin_lines()
		self.skel.__seg:draw_with_method( self, "draw_seg" )
	gol.do_end()
end
function meu:set_color_circle_out( z )
	local g = self.b_z and .8 - z * .8 or 1
	gol.color( g, self.skel_alpha )
	--gol.color_green()
end
function meu:set_color_circle_in( id, x, y, z )
	if false then
		local g = self.b_z and .8 - z * .8 or 1
		gol.color( { g,g,g, self.skel_alpha } )
		--gol.color( self.color_circle_in )
	end
	local seq_id = ga:get_seq_cur():get_seq_id()
	if seq_id > 3 then
		gol.color_white( self.skel_alpha )
	else
		local a = self:get_color( id, x, y )
		--todo refine
		if a > 0 then
	--		gol.color( 1,.9,.2, self.skel_alpha )
			gol.color_white( self.skel_alpha )
		else
			gol.color_red( self.skel_alpha )
		end
	end
end
function meu:draw_point( id, pt )
	local s = .15 * .5
	local x, y, z = pt[1], pt[2], get_z(pt)
	self:set_color_circle_out( z )
	aaa.draw_rect_uv_at_z( x-s, y-s, x+s, y+s, z )
	self:set_color_circle_in( id, x, y, z )
	s = s *.7
	aaa.draw_rect_uv_at_z( x-s, y-s, x+s, y+s, get_z(pt) )
end
function meu:draw_point_2d( id, pt )
	local s = .04 * .5
	local x, y, z = pt[1], pt[2], get_z(pt)
	self:set_color_circle_out( z )
	aaa.draw_rect_uv( x-s, y-s, x+s, y+s )
	self:set_color_circle_in( id, x, y, z )
	s = s *.7
	aaa.draw_rect_uv( x-s, y-s, x+s, y+s )
end

function meu:prepare_circle()
	gol.bind_texture( 63 )
	gol.set_texture_dim( 2 )
	gol.texcoord_set_unit_0()
	gol.set_quad_uv()
end
function meu:draw_points( id )
	--local bind = 416 + (id-1)*2
	local pl = self.skel.__pl

	self:prepare_circle()

	pl:process_points_with_method_range( self, "draw_point", 1, 3 )
	pl:process_points_with_method_range( self, "draw_point", 5, 16 )

	--gol.bind_texture( bind+1 )
	local cen	= pl:get_pt(4)
	local bas	= pl:get_pt(3)
	local vb	= V2.get_sub( cen, bas )
	V2.normalize( vb )
	local va = V2.get_rotate_90_neg( vb )
	local s = .20
	local x,y = cen[1], cen[2]

	self:set_color_circle_out( cen[3] )
	V2.scale( va, s )
	V2.scale( vb, s*1.2		 )
--[[	debug code
	self:draw_point_2d( 23, {x,y} )
	local S = 1
	aaa.draw_triangle_strip_4xy_uv( 		x-S, y-S,
								x-S, y+S,
								x+S, y-S,
								x+S, y+S
						 )
]]--
	aaa.draw_triangle_strip_4xy_uv( 		x-va[1]-vb[1], y-va[2]-vb[2],
								x-va[1]+vb[1], y-va[2]+vb[2],
								x+va[1]-vb[1], y+va[2]-vb[2],
								x+va[1]+vb[1], y+va[2]+vb[2]
						 )
	self:set_color_circle_in( 4, cen[1], cen[2], cen[3] )
	s = .88
	V2.scale( va, s )
	V2.scale( vb, s )
	aaa.draw_triangle_strip_4xy_uv( 		x-va[1]-vb[1], y-va[2]-vb[2],
								x-va[1]+vb[1], y-va[2]+vb[2],
								x+va[1]-vb[1], y+va[2]-vb[2],
								x+va[1]+vb[1], y+va[2]+vb[2]
						 )


	--gol.bind_texture( bind )
	gol.set_texture_dim( 0 )
	gol.color_white(1)
end

function meu:draw_numbers()
	self.skel.__pl:process_points_with_method( self, "draw_number" )
end

local ratio_img_u = 1/4.5
local ratio_img_v = 1/8
function meu:get_color( id, x, y, b_color )
	local u,v = x*ratio_img_u+.5, y*ratio_img_v+.5
	if b_color then
		return aaa.img.get_color_uv( self.__bind, u, v, true )
	end
	local a
	if self.b_update_conformity then
		local s=.1
		a = aaa.img.get_component_uv_ellipse( self.__bind, u, v, s*ratio_img_u, s*ratio_img_v, 8, 4, true, false )
		--if a > 0 then a = 1 end
		a = clamp_01( a * 8 )
		self.player.point_conformity[id] = a
		if a > 0 then
			self.player.conformity = self.player.conformity + 1
		end
	else
		a = self.player.point_conformity[id]
	end
	a = a or 0
	return a, a, 1-a, 1
end
function meu:draw_color_img( id, pt )
	gol.set_texture_dim( 0 )
	gol.texcoord_set_unit_0()
	gol.set_quad_uv()

	local f =1.2
	local x,y = pt[1], pt[2]
	local r,g,b,a = self:get_color( id, x, y, true )
	gol.color( r,g,b, a*(self.skel_alpha or 1) )
	--self:set_color_circle_out( cen )
	local s = .2
---[[
	aaa.draw_triangle_strip_4xy_uv( 		x-s, y-s,
								x-s, y+s,
								x+s, y-s,
								x+s, y+s
						 )
--]]
end
function meu:draw_null( id, pt )
	aaa.draw_null( pt[1], pt[2], get_z(pt), .05 )
end

function meu:draw_body( id, b_update_conformity, alpha )
	--self:print( "draw_body "..id )
	self.skel_alpha = alpha
	b_update_conformity = b_update_conformity and not app.b_simul
	self.b_update_conformity = b_update_conformity
	self.player = app:get_player( id )

	gol.set_default()
	gol.set_line_width( 8. )

	local id_f = (id*2-3)
	local ox = -1.2 * id_f
	local img = app:get_img_game( id )
	self.__bind = img:get_bind()
	local skel = self.skels[id]
	self.skel = skel
	if b_update_conformity then
		self.player.conformity = 0
	end

	if id == 1 then
		self.color_circle_in = { 1,.75,0, 1 }
	else
		self.color_circle_in = { 1,0,0, 1 }
	end

	if skel.b_valid then
		--self:print( "draw_body "..id )
		gol.color_white( self.skel_alpha )

		if self.b_seg		then self:draw_segs() end
		if self.b_color		then skel.__pl:process_points_with_method( self, "draw_color_img" ) end
		if self.b_dot		then self:draw_points( id )	end
		if self.b_number	then self:draw_numbers() end
		if self.b_null		then skel.__pl:process_points_with_method( self, "draw_null" ) end
		--aaa.draw_disk_axe_z( 0,1,0,2)
	end

	if b_update_conformity then
		local con = self.player.conformity
		--self:print( con )
		con = con / 16
		self.player.conformity = con
		self.player.conformity_draw = interpolate( self.player.conformity_draw, con, aaa.time.dt*2. )
	end
end

function meu:draw()
	MEU.draw(self)
	self:draw_body( 1, true )
	self:draw_body( 2, true )
end