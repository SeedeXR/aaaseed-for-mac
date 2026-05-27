function meu:define_ui()
	self.__boids_meus = nil

	local ref = self.ref
	local bu
	local par
	local ui = self.ui

--	todo change cam done before use target to take over selector in other meu
--	self:add_camera()

	local SY = 1
	local ix,iy

	ix,iy = 9,1
	--self:add_camera()
	self:add_trig(		{ix,iy,		4, SY}, "Reset Ship" ):set_method_on_click( self, "reset_ship" )
	iy = iy + SY*1.5

	self:add_shading_ui({ix,iy,		nil,SY} )
	iy = iy + SY

	local sha = self:get_shading()
	bu = self:add_selector(	{ix,iy,	8,SY}, 	"type" ):set_target_param(sha:get_ref( "frag", "int", 1))
		bu:set_nb_min_0(8)
	iy = iy + SY

	ix,iy = 1,1.5
	self:add_trig(		{ix,iy,		4,nil}, "Restart" ):set_target_lua( self, "b_restart" )

	iy = iy + SY*1.5
	self:add_button(	{ix,iy},			"Matrix", self, "b_matrix_draw", false ):set_text_draw( false )
	self:add_slider(	{ix+1,iy,	3,SY},	"Matrix_alpha" ):set_target_lua( self, "matrix_alpha" )
	self:add_slider(	{ix+4,iy,	4,SY},	"Matrix_size" ):set_min_max( 0, 40 ):set_target_lua( self, "matrix_size" )

	iy = iy + SY*1.5
	self:add_button(	{ix,iy},			"Plaques", self, "b_plaque_draw", false )

	self:add_bu_texture_target_unit( {1,8,8,3},		"TexTest" )

end

function meu:load_images()
	if not self.imgs then
		self.imgs = IMG_SEQ:create( "all", app.media_dir_rel.."Monart/images/testa_", "tga", 1, 53, 1 )
	end
end

function meu:init()
	local ref = self.ref
	ref.ship = {}
	local ship = ref.ship
	local obj = aaa.obj.get_from_top_by_class( "ship" )
	ship.obj = obj
	--self:box_debug( "ship is "..ship.obj )
	ship.tra = param.get_ref( obj, "analog_translation_forward" )
	ship.rot = param.get_ref( obj, "analog_rotation_right" )
end

function meu:reset_ship()
	param.set( aaa.camera.get_ui(), "trig_reset", true )
end
function meu:move_ship( forw, rot )
	local ship = self.ref.ship
	param.set( ship.tra, forw )
	param.set( ship.rot, rot )
end

function meu:draw_one( id_art )
	local bind = self.imgs:get_bind(id_art)
	gol.bind_texture( bind )
	local px,py = aaa.img.get_size( bind )
	if px then
		local sy = .5
		local sx = sy * px/py
		local mx = .9
		if sx > mx then
			sx = mx
			sy = sx * py/px
		end
		local x,y,z = sx,sy,-1
		local x0, y0, z0 = aaa.camera.cam_to_world( -x,y,z )
		local x1, y1, z1 = aaa.camera.cam_to_world( -x,-y,z )
		local x2, y2, z2 = aaa.camera.cam_to_world( x,y,z )
		local x3, y3, z3 = aaa.camera.cam_to_world( x,-y,z )
		aaa.draw_triangle_strip_4xyz_uv	(	x0,y0,z0, x1,y1,z1, x2,y2,z2, x3,y3,z3 )
	end
end
function meu:draw_collec( id_collec )
	--self:print( "draw_collec " .. id_collec )
	local bdd = self.__parts[id_collec]
	local nb = aaa.bdd.get_point_nb( bdd )
	local sha = self:get_shading()
	aaa.show( nb, "point_nb" )

	local pals	=	{	{	{ 1, 1,.1,1 },	{ 1,.1,	1,1 }	},
						{	{ .1, 1,1,1 },	{ 1,.1,	1,1 }	},
						{	{ .1,1,1,1 },	{ 1,.1,.1,1 }	},
					}

	--self:print( "alpha is "..self.alpha_regular )
	local pal = pals[id_collec]
	local pts = aaa.bdd.get_points_with_id(	bdd )
	if not pts then
		return
	end

	--self:print( id_collec )
	local r = .1
	local img_nb = 54
	local pgr_used = gol.get_program_used()
	local col = {}
	for i=1,#pts do
		local pt = pts[i]
		local x,y,z,id = pt[1], pt[2], pt[3], pt[4]

		gol.push_matrix()
			gol.translate( x,y,z )

			local inter = math.fmod( id, 256 ) / 256

			V3.interpolate( col, pal[1], pal[2], inter )
			col[4] = self.matrix_alpha
			gol.color( col )
			if self.b_matrix_draw then
				--gol.use_program( 0 )
				--gol.color_white( self.matrix_alpha )
				--gol.set_texture_dim(0)

				sha:set_frag_int_2( 0 )
				gol.update_uniform_fragment_int()
				aaa.draw_null( self.matrix_size )
--				gol.color_green( self.matrix_alpha*50	 )
--				aaa.draw_str_xyz( tostring(pt[4]), x,y,z, .1, .1 )
 				--gol.use_program( pgr_used )
			end

			if self.b_plaque_draw then
				local id_art = (id % img_nb) + 1
				local bind = self.imgs:get_bind(id_art)
				local px,py = aaa.img.get_size( bind )
				if px then
					local rx = px/py
					local sx = math.sqrt(rx) * .125
					local sy = sx/rx
					local b_draw_image = false

					local alpha_glo = 1
					alpha_glo = math.fn_linear( y, 1,  -2,0, -1.8,1, 2,1, 3,0 )
					if alpha_glo>0 then	b_draw_image = true end

					math.randomseed( id )
					gol.rotate_y( math.random() )
					--gol.rotate_x( .25 )
					local ry = math.fn_linear( y,1,  -1.5,0, -1.,-.25 )
					gol.rotate_x( ry + .25 )

					--aaa.draw_rect(	-sx, -sy, sx, sy )

					local mix_tex = 0
					sha:set_frag_int_2( id_art )
					gol.update_uniform_fragment_int()
					if b_draw_image then
						mix_tex = math.fn_linear( y, 1,  -1.5,0, -1,1, 1.7,1, 2.2,0 )
						--mix_tex = 0
						gol.set_texture_dim(2)
						--self:print( id_art )
						gol.bind_texture( bind )
					else
						gol.set_texture_dim(0)
					end

					sha:set_frag_float_1_2( alpha_glo, clamp_01(mix_tex) )
					--sha:set_frag_float_1_2( 1, 0 )
					gol.update_uniform_fragment_float()
					aaa.draw_rect_uv(	-sx,-sy, sx, sy )
					--gol.rotate_y( math.random() )
				end
			end
		gol.pop_matrix()
	end
	sha:set_frag_int_2( 12 )
	gol.update_uniform_fragment_int()
	sha:set_frag_float_1_2( 1, 1 )
	gol.update_uniform_fragment_float()
	--gol.use_program(0)
	if self.b_one then
		self:draw_one( self.picked_id or 1 )
	end
	sha:set_frag_int_2( 0 )
	gol.update_uniform_fragment_int()
end

function meu:update()
	self:load_images()

	local function curve( v )
		local p = 3
		if v>0 then v = math.pow( v, p )
		else 		v = -math.pow( -v, p )
		end
		return v
	end
	local blobs = ga:get_blobs()
	local x,y
	if blobs:get_blob_nb() > 0 then
		local xm,ym =  aaa.mouse.get_xy_render()
		x = (clamp_01(xm)-.5) * 2
		y = (clamp_01(ym)-.5) * 2
		aaa.show( x, "x" )
		aaa.show( y, "y" )
		x = curve(x)
		y = curve(y)
		local ft = .25
		local fr = 8
		x = x * fr
		y = y * ft

		if not self.clicked then
			self:print( "Mouse is down" )
			if inside( xm, .45, .55) and ym < .05 then
				self:reset_ship()
			end
			local last_t = self.click_last_t or 0.
			local t = aaa.time.t
			if self.b_one then
				self.b_one = false
			else
				if t-last_t < .5 then
					self:print( "MonArt double click" )
					local m = self:get_meu_by_name( "Pick_1" )
					if m then
						local id = m:get_picked_id()
						if inside( id, 1, 53 ) then
							self.picked_id = id
							self.b_one = true
						end
					end
				end
			end
			self.click_last_t = t
		end
		self.clicked = true
	else
		x = (self.rot or 0) * .9
		y = (self.tra or 0) * .9
		self.clicked = false
	end
	self.rot = x
	self.tra = y
	self:move_ship( y, x )
end

function meu:draw()
	local parts = self.__parts
	if not parts then
		parts = {}
		for i=1,3 do
			local m = self:get_meu_by_name( "MonartPart_"..i )
			if not m then return end
			parts[i] = m:get_layer_bdd(1)
		end
		self.__parts = parts
	end

	self:draw_layers_begin()
		self:draw_layers()

		aaa.camera.set_ui( aaa.camera.get_cur() )
		local x,y,z =  aaa.camera.cam_to_world( 0,0,0 )
		--aaa.show( x, "x" ) aaa.show( y, "y" ) aaa.show( z, "z" )
		local ord = x<0 and (z<0 and {2,1,3} or {3,2,1}) or (z<0 and {1,2,3} or {3,1,2})
		for i=1,3 do
			self:draw_collec( ord[i] )
		end
	self:draw_layers_end()
end