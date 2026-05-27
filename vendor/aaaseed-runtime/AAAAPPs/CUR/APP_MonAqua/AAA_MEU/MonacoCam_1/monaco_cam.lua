
function meu:pick_screen( name )
	local id
	if string.len(name)==3 then
		id = tonumber( name:sub(3,3) )
		name = name:sub(1,2)
		self:print( "Pick screen "..name.." "..id )
	else
		self:print( "Pick screen "..name )
	end

	local gbuffer = self.gbuffer
	if not gbuffer then return end
	local fbo = gbuffer:get_fbo()

	local cam = self.gbuffer_cam
	if not cam then return end

	aaa.obj.clear_param(cam)
	local tab =  app.cams[name]
	if not tab then return end

	--table.print( tab, "tab", 2 )
	local keys = { "rot_y", "frustum_offset_y" }
	for _,key in IPAIRS(keys) do
		local v = tab[key]
		--self:print( key.." -> "..v )
		if v ~= nil then
			param.set( cam, key, v )
		end
	end
	local focal = tab.focal_vertical
	if focal then
		param.set( cam, "focal", focal )
	else
		param.set( cam, "focal_horizontal", true )
		param.set( cam, "focal", tab.focal_horizontal )
	end
	if id then
		local i = name:sub(2,2)=="R" and (5-id) or id
		param.set( cam, "frustum_offset_x", (i-2.5)*1.16336)
	end
	param.set( cam, "clip_close_perspective",	tab.clip_close	)
	param.set( cam, "clip_far_perspective",		tab.clip_far	)
	local p = tab.pos
	param.set( cam, "tra_z", 0 )
	if p then
		param.set( cam, "center_x", p[1] )
		param.set( cam, "center_y", p[2] )
		param.set( cam, "center_z", p[3] )
	end
	local a = tab.angle_deg
	if a then
		param.set( cam, "pitch", -a / 360 )
	end

	local sx,sy

	if id then	sx,sy = tab.k_sxp, tab.syp
	else		sx,sy = tab.sxp, tab.syp
	end
	if app:is_pc_mas() then
		sx,sy = sx/8,sy/8
	else
	end
	fbo:set_pixel_size( sx,sy )

	self:set_shadow( name )

	self:set_plancton( name, id )
end

function meu:set_shadow( name )
	local values_by_name =
	{
		KL = { -4,22,	0,9,	-16,-4	},
		KR = { -4,22,	0,9,	4,16	},
		KC = { 20,32,	0,9,	-6,6 	},
		SO = { -1,22,	-10,1,	-5,5	}
	}

	local field =	{	"shadow_bbox_min_x",	"shadow_bbox_max_x",
						"shadow_bbox_min_y",	"shadow_bbox_max_y",
						"shadow_bbox_min_z",	"shadow_bbox_max_z",
					}
	local light = aaa.obj.get( app:get_dir_absolute().."default4.light" )
	if not light then return end

	local values = values_by_name[name]
	if not values then return end

	for i=1,6 do
		param.set( light, field[i], values[i] )
	end
end

function meu:set_plancton( name, id )
	local meu_plancton = app:get_meu_by_name_cached( "PlanctonMona_White" )
	if not meu_plancton then return end

	local KNB = 1024*256
	local values_by_name =
	{
		KL	= { KNB,	-2,23,	1,14,	-12,-4	},
		KR	= { KNB,	-2,23,	1,14,	4,12	},
		KC	= { KNB/2,	0,32,	1,14,	-6,6 	},
	}

	local val = values_by_name[name]
	meu_plancton:get_mu():set_value( val and 1 or 0 )
	if not val then
		return
	end

	local nb = val[1]
	if id then nb = nb / 4 end
	meu_plancton:set_point_nb( nb )

	local x,sx = (val[3]+val[2])*.5,  val[3]-val[2]
	local y,sy = val[4],  			  val[5]
	local z,sz = (val[7]+val[6])*.5,  val[7]-val[6]
	meu_plancton:set_xyz_sxyz( x,y,z, sx,sy,sz )

	-- local light = aaa.obj.get( app:get_dir_absolute().."default4.light" )
	-- if not light then return end

	-- for i=1,6 do
	-- 	param.set( light, field[i], values[i] )
	-- end
end

function meu:define_monaco_cam( ix,iy, sx,sy )
	local function add( name, ox, oy, nx, ny, text )
		local bu = self:add_trig_method(	{ix+ox, iy+(oy)*sy,	 nx*sx, ny*sy}, name )
			bu:set_method_on_click( self, "pick_screen", name )
			if text then
				bu:set_text( text )
			end
			if ny > 1 then
				bu:set_text_xy_fxy( 0, ny==2 and .25 or .3333, 1, 1/ny )
			end
			bu:set_text_inside( true )
	end
	local oy = 0
	self:add_trig_method(		{ix+5.5,	iy+.5,	2,1	},	"RESET_ALL", 	app, "pick_screen_all"	):set_color_back( "reset" )
	add(	"KC",	3, 0,	2, 2 )
	add(	"KL",	0, 2, 	1, 4, "L" )		add(	"KR",	7, 2, 	1, 4, "R" )
	add(	"KL4",	1, 2, 	2, 1 )			add(	"KR4",	5, 2, 	2, 1 )
	add(	"KL3",	1, 3, 	2, 1 )			add(	"KR3",	5, 3, 	2, 1 )
	add(	"KL2",	1, 4, 	2, 1 )			add(	"KR2",	5, 4, 	2, 1 )
	add(	"KL1",	1, 5, 	2, 1 )			add(	"KR1",	5, 5, 	2, 1 )
	add(	"SO",	3, 2,	2, 4, "Sol" )
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	--todo use gbuffer bu (delegate ?)
	bu = self:add_camera( {nil, nil, nil, 2}, 16 )
		bu:set_preset_use( false )
		bu:set_item_text( 2,	"X",	"Y",	"Z"		)
		bu:set_item_text( 13,	"KL",	"KC",	"KR"	)
		bu:set_item_text( 16,	"The"	)

	self:define_monaco_cam( 9, 5, 1, 1 )



	local ix = 1
	local iy = 1
	local SY = 1
	local DY = SY*.2

	local function add_box_selector( x, y, sx, sy, pre )
		local SLR = sx/3
		local SC = SLR
		local bu
		sy = sy / 3
		local function add(	x, y,	sx, sy, what )
			local post = pre.."_"..string.lower(what)
			bu = self:add_button(	{x, y,	sx, sy }, "Draw_"..post,		self, "b_"..post,	false )
			bu:set_text( what )
			bu:set_text_inside( true )
			bu:set_check_mul( true )
			if sy > SY then
				bu:set_text_xy_fxy( 0, .25 , 1, 1/2 )
			end
		end
		add(	x+SLR, y,		SC,  sy, 	"KC" 	)
		y = y + sy
		add(	x, y,			SLR, sy*2,	"KL" 	)
		add(	x+SLR, y,		SC,	 sy*2, 	"Sol"	)
		add(	x+SLR+SC, y,	SLR, sy*2,	"KR"	)
		y = y + sy * 3
		return y
	end

	self:add_button(	{ix, iy,		SY, SY }, "Anals",		self, "b_draw_anals",	true )
	iy = iy + SY

	self:add_button(	{ix, iy,		SY, SY }, "FP",			self, "b_draw_fp",	true )
	iy = iy + SY

	self:add_button(	{ix, iy,		SY, SY }, "Draw",		self, "b_draw",	true )
	iy = iy + SY

	self:add_button(	{ix, iy,		SY, SY }, "Box Grid",	self, "b_draw_box",	true )
	add_box_selector( ix+6, iy, 2, SY*2, "box_hide" )
	iy = iy + SY
	self:add_slider(	{ix,iy,	6,SY},	"Box Alpha",	self, "box_alpha",	0,0,1 )
	iy = iy + SY + DY

	local SYC = SY*1.
	self:add_button(	{ix, iy,		SY, SY }, "Monaco",		self, "b_draw_monaco",	true )
	add_box_selector(		ix+6, iy, 2, SY*2, "monaco_hide" )
	iy = iy + SY
	ui.bu_monaco_color	= self:add_rgbfa(	{ix,iy,	6,SYC}, "Monaco Color", 		false )
	iy = iy + SYC


	-- self:add_button(	{ix, iy,		SY, SY }, "Wissous",		self, "b_draw_wissous",		true )
	-- iy = iy + SY
	-- ui.bu_wissous_color	= self:add_rgbfa(	{ix,iy,	6,SYC}, "Wissous Color", 		false )
	-- iy = iy + SYC

	self:add_button(	{ix, iy,		SY, SY }, "Cams",			self, "b_draw_cams",	true )
	iy = iy + SY
	ui.bu_cams_color	= self:add_rgbfa(	{ix,iy,	6,SYC}, "Cams Color", 		false )
	iy = iy + SYC + DY


	self:add_button(	{ix, iy,		SY, SY }, "Zone",			self, "b_draw_zone",	true )
	iy = iy + SY
	ui.bu_zone_color	= self:add_rgbfa(	{ix,iy,	6,SYC}, "zone Color", 		false )
	iy = iy + SYC + DY

--	self:add_slider(	{ix,iy,	4,SY},	"Line Size",	self,	"line_size",	4, 1, 16		)
--	self:add_button(	{ix+4, iy,	SY, SY }, "Fill",		self,	"b_fill",		false	)
--	iy = iy + SY

	self:add_rendering()

	bu = self:add_trig_method(	{9,12,		4,1},	"define room",	app, "define_all", true		)
		bu:set_color_back( "reset" )
end

function meu:draw_screens( name, t, b_wis )
	--table.print( t, "screen", 3 )
	local pos = b_wis and t.pos_wis or t.pos
	local bu_color = b_wis and self.ui.bu_wissous_color or self.ui.bu_monaco_color
	local r,g,b,a = bu_color:get_rgba()
	local cen = pos.cen
	local suv = pos.suv
	gol.push_matrix()
		gol.translate( cen[1], cen[2], cen[3] )
		if t.rot_x then gol.rotate_x( t.rot_x ) end
		if t.rot_y then gol.rotate_y( t.rot_y ) end

--		aaa.draw_mul_line( 0,0, suv[1]/2,suv[2]/2 )
		local ds = .05
		local xl,yb,xr,yt = -suv[1]/2, -suv[2]/2, suv[1]/2, suv[2]/2
		gol.color( r,g,b, a*.75 )
		aaa.draw_rect(	xl+ds,	yb+ds,	xr-ds,	yt-ds	)
		gol.color( r,g,b, a )
		aaa.draw_null_2d( .5 )
		aaa.draw_rect(	xl,		yb,		xr,		yt		)

		if false then
			aaa.draw_str_xy( name, -suv[1]/2 + .1 , suv[2]/2 - .9 )
		else
			aaa.draw_str_xy( name, -suv[1]/2 + .1,	suv[2]/2 - .5 	,.5,.5 )
			--aaa.draw_str_xy( name,  .1,				- .5 			,.5,.5 )
			aaa.draw_str_xy( name,  -.4,				- .5 			,.5,.5 )
			aaa.draw_str_xy( name, suv[1]/2 - 1.,	-suv[2]/2 + .1	,.5,.5 )
		end
	gol.pop_matrix()
end

function meu:draw_deco( b_wis )
	for name, t in PAIRS(app.deco) do
		--self:print( t.sn )
		if not self["b_monaco_hide_"..t.sn] then
			self:draw_screens( name, t, b_wis )
		end
	end
end

function meu:draw_cam_one( t, size )
	--table.print( cen, "cen", 2 )
	--table.print( size, "size", 2 )
	local r,g,b,a
	if self.b_draw_cams then
		r,g,b,a = self.ui.bu_cams_color:get_rgba()
		gol.color( r,g,b, a )
		local x,y,z	= t.pos[1],t.pos[2],t.pos[3]
		for i,tar in IPAIRS(t.target) do
			--todo add flag here
			--gol.draw_lines_3d( x,y,z, 3*tar[1]-2*x,3*tar[2]-2*y,3*tar[3]-2*z )
			gol.draw_lines_3d( tar[1],tar[2],tar[3], 3*tar[1]-2*x,3*tar[2]-2*y,3*tar[3]-2*z )
		end
		aaa.draw_box( size[1],size[2],size[3], x,y,z )
		aaa.draw_null( x,y,z, .25 )
	end

	if self.b_draw_zone then
		local c = t.coefs
		if c then
			r,g,b,a = self.ui.bu_zone_color:get_rgba()
			local d = 4
			local d_end = d * 16
			local p1,p2 = {0,0,0}, {0,0,0}
			local is, id = t.i_src, t.i_dst
			--table.print( c, "c", 2 )
			local b_a = false
			for v=d,d_end,d do
				local f = v>32 and 0.5 or 1
				--self:print(f)
				gol.color( r*f,g*f,b*f, b_a and a or a/2 )
				b_a = not b_a
				local vs = c[1].beg + v * t.dir_src
				--self:print(v)
				p1[is] = vs
				p2[is] = vs
				p1[id] = c[1].a * vs + c[1].b
				p2[id] = c[2].a * vs + c[2].b
				gol.draw_lines_3d( p1[1],p1[2],p1[3], p2[1],p2[2],p2[3] )
			end
			gol.color( r,g,b, a )
			for i=1,2 do
				--self:print(v)
				for j=1,2 do
					local f = j==2 and 0.5 or 1
					gol.color( r*f,g*f,b*f, a )
					p1[is] = c[i].beg + d_end * t.dir_src * (j-1)/2
					p2[is] = c[i].beg + d_end * t.dir_src * j/2
					p1[id] = c[i].a * p1[is] + c[i].b
					p2[id] = c[i].a * p2[is] + c[i].b
					gol.draw_lines_3d( p1[1],p1[2],p1[3], p2[1],p2[2],p2[3] )
				end
			end
		end
	end
end

function meu:draw_cam( name, t )
	--table.print( t, "cam "..name, 2 )
	local f = .5
	self:draw_cam_one( t, { f, f, f } )
end
function meu:draw_cams()
	for name, t in PAIRS(app.cams) do
		self:draw_cam( name, t )
	end
end

function meu:draw_as_box()
	local alpha = self.box_alpha
	if alpha <= 0 then return end

	local lines = self.__lines
	if not lines then
		lines = {	so_x={}, so_z={},
					kl_y={}, kl_x={},
					kr_y={}, kr_x={},
					kc_y={}, kc_z={}	}

		local c = app.const.mona
		local dh = c.d_lr / 2
		local x = c.sol_s
		local i = 0
		local function define( t, min, max, step, b_beg, fn )
			if b_beg then i = 0 end
			for v=min,max,step do
				t[i+1],t[i+2],t[i+3],	t[i+4],t[i+5],t[i+6]	=	fn(v)
				i = i + 6
			end
		end

		define( lines.so_x, 0, x, .5,	true,	function(v) return	v,0,-dh,	v,0,dh		end )
		define( lines.so_z, -4, 4, .5,	true,	function(v) return	0,0,v,		x,0,v		end )
		define( lines.kl_y, 0, 9, .5,	true, 	function(v) return	0,v,-dh,	x,v,-dh		end )
		define( lines.kl_x, 0, x, .5,	true, 	function(v) return	v,0,-dh,	v,9,-dh		end )
		define( lines.kr_y, 0, 9, .5,	true, 	function(v) return	0,v,dh,		x,v,dh		end )
		define( lines.kr_x, 0, x, .5,	true, 	function(v) return	v,0,dh,		v,9,dh		end )
		define( lines.kc_y, 0, 9, .5,	true, 	function(v) return	x,v,-dh,	x,v,dh		end )
		define( lines.kc_z, -4, 4, .5,	true, 	function(v) return	x,0,v,		x,9,v		end )
		self.__lines = lines
	end

	local function draw( c1, t1, c2, t2 )
		gol["color_"..c1](alpha)	gol.draw_lines_3d( t1 )
		gol["color_"..c2](alpha)	gol.draw_lines_3d( t2 )
	end
	if not self.b_box_hide_sol	then	draw( "blue",	lines.so_x,		"red", 		lines.so_z	)	end
	if not self.b_box_hide_kl	then	draw( "red",	lines.kl_y,		"green", 	lines.kl_x	)	end
	if not self.b_box_hide_kr	then	draw( "red",	lines.kr_y,		"green", 	lines.kr_x	)	end
	if not self.b_box_hide_kc	then	draw( "blue",	lines.kc_y,		"green", 	lines.kc_z	)	end
end

function meu:update()
	app:define_all( false )

	--test
	--local v = self:box_dev( "Title", "text\non several line" )
	--self:box_debug( "value from aaa.box_dev is "..v )

	local gbuf, b_first = app:get_meu_by_name_cached( "Fbo_GBuffer" )
	if gbuf then
		if b_first then
			--	ref.cam_kinect	= AAACAM:create( "kinect projector", self:get_camera( 7 ) )

			--todo
			--ref.cam_	= AAACAM:create( "Bbuffer Monaco Cam", self:get_camera( 7 ) )
			self.gbuffer = gbuf

			if app:is_pc_mas() then
				local fbo = gbuf:get_fbo()
				fbo:set_pixel_size( 733,320 )
			end

			self.gbuffer_cam = gbuf:get_camera( 15 )

			local bu = gbuf.ui.bu_cam_sel
				bu:set_item_text( 2, 	"X", 	"Y",	"Z"		)
				bu:set_item_text( 12,	"SO",  	"KL",	"KC",	"KR"	)
				bu:set_item_text( 16,	"The"	)

			--we make the gbuf camera current so we don't need to select before every launch
			local cam = gbuf:get_camera_sel()
			aaa.obj.become_ui( cam )
		end
	end

end

function meu:draw_anals()
	self:draw_layer(3)
	-- gol.color_white( )
	-- local z = 4
	-- gol.set_texture_dim(0)
	-- gol.bind_texture( 67 )
	-- local xmin,xmax = 0, 20.75
	-- local zmin,zmax = -z, z
	-- aaa.draw_triangle_strip_4xyz_uv(	xmin,0,zmin, xmin,0,zmax, xmax,0,zmin, xmax,0,zmax )
	-- --aaa.draw_bind_rect( 97, -r, -r, r, r )
end
function meu:draw()
--	MEU.draw(self)
	local ui = self.ui
 	self:draw_layers_begin()
		self:draw_layer( 1 )
		if self.b_draw_anals	then	self:draw_anals()	end
		if self.b_draw_fp		then	self:draw_layer(4)	end
	if app.deco and self.b_draw then
		self:draw_layer( 1 )

		if self.b_draw_cams or self.b_draw_zone then
			self:draw_cams()
		end
		if self.b_draw_box		then	self:draw_as_box() end
		if self.b_draw_monaco	then	self:draw_deco( false )	end
		if self.b_draw_wissous	then	self:draw_deco( true )	end
	end
 	self:draw_layers_end()
end
