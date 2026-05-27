function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "2d", "3d", "geometry", "texture", "output", "Point", "procedural" },
			help = 	{
					"Handle UV mapping of face used in conjunction with Face Trak",
					"used in the App_Dior"
					}
		}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()

	bu = self:add_slider_multi( {3, 2, 12, 12}, "UV", 87 )
		local s = .04
		bu:set_elt_sxy( s, s )
		for i=1,bu:get_elt_nb() do
			local elt = bu:get_elt( i )
			elt:set_text( i )
			elt:set_text_xyf( -.5, .5, 1. )
			elt:set_text_rect_ratio(1)
			elt:set_frame_plus( true )
		end
		--todo check this working
		bu:set_back_bind( 32 )
		local bus = bu:get_bus_down(1)
		bus:set_method(	"draw_before",	self, "draw_bu_uv_back" )
	self.ui.uv = bu

	bu = self:add_trig_method(	{1,1},			"init", 		self.ui.uv, "place_elts"	)
	bu = self:add_button(		{4,1},			"Feed_Bdd ", 	self,		"b_feed_bdd",		false	)
		bu:set_text_color( "red" )	--because the bdd accumulate and will fuck of the memory after few hours
	for i = 1,2 do
		local x = i==1 and 1 or 15
		bu = self:add_button(	{x,2,	2,.8 },	"Tri "..i, 		self,		"b_tri_"..i,		false	)
--		bu = self:add_button(	{1,3,	2,.8 },	"Rnd", 		self,		"b_color_rnd",	false	)
		bu = self:add_button(	{x,3,	2,.8 },	"Edge "..i, 	self,		"b_edge_"..i,		false	)
		bu = self:add_selector(	{x,4,	2,4},	"Img "..i	)
		--bu:set_text_draw( false )
		bu:set_nb_min_0( 1, 8 )
		bu:set_target_lua( self, "img_id_"..i )
		bu:set_item_text( 1, "No" )
		bu:set_item_text( 8, "Mon" )
		bu = self:add_slider(	{x,10,	2,1}, 	"Alpha_"..i, 	self, "alpha_"..i )
	end

	bu = self:add_trig_method(	{1,	14},		"Import 1", self,		"import",	1	)
	bu = self:add_trig_method(	{1,	15},		"Import 2", self,		"import",	2	)
end

function meu:init()
	local ref = self.ref

	ref.bdd = self:get_layer_bdd( 1 )
end

function meu:feed_bdd( )
end

function meu:import( id )
	aaa.print_method()
	--self:print( id )
	local face_info = app:get_face_info( 1, 1 )
	local uvs = face_info:get_pts()
	local uv = face_info:get_uv_rect()
	local su = V2.dist_to_line_ab( uv[3], uv[1], uv[2] )
	local sv = V2.dist_to_line_ab( uv[2], uv[1], uv[3] )
	local tout = {}
	for i=1,#uvs do
		local u = V2.dist_to_line_ab( uvs[i], uv[1], uv[2] )
		local v = V2.dist_to_line_ab( uvs[i], uv[1], uv[3] )
		tout[i] = { u/su, v/sv }
	end
	self.ui.uv:set_values( tout )
end

function meu:draw_tri_begin()
	gol.reset()
	gol.set_default()
	gol.set_blend_add()
	gol.texcoord_set_unit_0()

	--gol.set_blend_add_alpha_one()
	--gol.set_blend_add_color()
	--gol.set_blend_max()
end

function meu:draw_tri_tex( face_info )
	local pts = self.ui.uv:get_values()
	local uvs = face_info and face_info:get_pts_full()
	if uvs then
		gol.color_white( .5 )
		gol.set_texture_dim(2)
			gol.bind_texture( FACE_INFO.tex_bind )	--	src Camera was 107 hard coded
			if self.b_tri_1 then face_info:draw_tri_tex_one( 2, pts ) end
			if self.b_tri_2 then face_info:draw_tri_tex_one( 1, pts ) end
		gol.set_texture_dim(0)
	end
end

function meu:draw_tri_edge( face_info )
	local pts = self.ui.uv:get_values()
	if self.b_edge_1 then face_info:draw_tri_half( pts, true ) end
	if self.b_edge_2 then face_info:draw_tri_half( pts, false ) end
end

function meu:draw_tri( face_info, edge_color, line_width )
	if not face_info then return end
	--if true then return end
--	local pts = self.ui.uv:get_values()

	self:draw_tri_begin()
	--if b_tri then
		self:draw_tri_tex( face_info )
	--end

	local pts = self.ui.uv:get_values()
	local function draw_line_default( a, b )
		aaa.draw_line( a[1], a[2], b[1], b[2] )
	end
	local function draw_contour( fl, ... )
		for id=fl[1],fl[2]-1 do
			--self:print( "POINT_LIST:draw_line_begin_end : id "..id )
			draw_line_default( pts[id], pts[id+1] )
		end
		if fl[3] then
			draw_line_default( pts[fl[2]], pts[fl[1]] )
		end
	end
	if false then	--todo dior
		gol.color_cyan( .5 )
		draw_contour( FACE_INFO.face_lines[2] )
		draw_contour( FACE_INFO.face_lines[3] )
		draw_contour( FACE_INFO.face_lines[8] )
		draw_contour( FACE_INFO.face_lines[9] )
	end

	if edge_color[4]>0 then
		gol.color( edge_color[1], edge_color[2], edge_color[3], edge_color[4] )
		line_width = line_width or 1.
		gol.set_line_width( line_width )
		self:draw_tri_edge( face_info )
	end
end

function meu:draw_bu_back_tex( face_info, edge_color, line_width )
	--if true then return end
	local img_id = self.img_id_1
	--self:print( img_id )
	if app and img_id>0 then
		local bind
		if img_id == 7 then
			bind = BU_MONITOR.__last_bind
		else
			local method = app.get_img_fix
			if method then
				local img = method( app, img_id )
				if img then bind = img:get_bind() end
			end
		end
		aaa.draw_bind_rect( bind, 0, 0, 1, 1 )
		--maatex gol.set_texture_dim( 0 )
	end
end

function meu:draw_bu_uv_back()
	gol.color_white( self.alpha_1 )
	self:draw_bu_back_tex()
	local face_info = app.get_face_info and app:get_face_info( 1, 1 )
	self:draw_tri( face_info, { 1, 1, 0, 1  }, .1 )
end

function meu:update()
	if self.b_feed_bdd then
		self.ui.uv:cpy_point_to_bdd( self.ref.bdd, -.5, -.5, 4, 4 )
	end
end