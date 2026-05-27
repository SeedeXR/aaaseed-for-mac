function meu:define_ui()
	self.__boids_meus = nil

	local ref = self.ref
	local bu
	local par
	local ui = self.ui

--	todo change cam done before use target to take over selector in other meu
--	self:add_camera()

	local sy = .8
	local dy = .2
	local ix,iy

	ix,iy = 9,1
	self:add_camera()
	iy = iy + sy*1.5

	self:add_shading_ui(	{ix,iy,		nil,sy} )
	local sha = self:get_shading()

	if sha then
		iy = iy + sy

		bu = self:add_selector(	{ix,iy,	8,sy}, 	"type" ):set_target_param(sha:get_ref( "frag", "int", 1))
			bu:set_nb_min_0(8)
		iy = iy + sy

		ix,iy = 1,1.5

		self:add_trig(		{ix,	iy,		4, sy}, "Restart", ref.b_restart )	--:set_target_lua( self, "b_restart" )
		self:add_trig(		{ix+4,	iy,		4, sy},	"Load" ):set_method_on_click( self, "load_images" )

		-- iy = iy + sy*1.5
		-- self:add_button(	{	ix,	iy },				"Matrix", self, "b_matrix_draw", false ):set_text_draw( false )
		-- self:add_slider(	{ix+1,iy,	3,sy},	"Matrix_alpha" ):set_target_lua( self, "matrix_alpha" )
		-- self:add_slider(	{ix+4,iy,	4,sy},	"Matrix_size" ):set_min_max( 0, 40 ):set_target_lua( self, "matrix_size" )

		iy = iy + sy*1.5
		self:add_button(	{ix,	iy,		sy,sy},				"Plaques", self, "b_plaque_draw", false )
		iy = iy + sy
		bu = self:add_selector(	{ix,iy,	8,sy*3}, "pal" )
			bu:set_nb(3,3)
			for i=1, LV:get_color_nb() do
				bu:set_item_text( i, LV:get_color_name(i) )
			end
			bu:set_item_text( 9, "Free" )
			self.ui.bu_pal = bu
		iy = iy + sy*3
		self.ui.bu_info = self:add_text_info(	{ix,iy,	8,sy}, "LVColor info" )
		iy = iy + sy
		self.ui.bu_rgb = self:add_rgbf(	{ix,iy,	8,sy*1.5 })
		iy = iy + sy*1.5
		self:add_slider(	{ix,iy,			8,sy},	"Central",	sha:get_ref( "frag", "float", 2 ), nil, 1,0,1 )
		self:add_slider(	{ix,iy+sy,		8,sy},	"Contrast",	sha:get_ref( "frag", "float", 3 ), nil, 1,0,2 )
		self:add_slider(	{ix,iy+sy*2,	8,sy},	"Power",	sha:get_ref( "frag", "float", 4 ), nil, 1,0,4 )
		iy = iy + sy*3 + dy
	end
	self:add_bu_texture_target_unit( {1,iy,8,3},		"TexTest" )

	ix,iy = 9,4.5
	local tab = {}
	for i=1,7 do
		bu = self:add_slider(	{ix,iy,	8,sy},	LV:get_color_name(i), tab, i, 1,0,1 ):set_color_back( LV:get_color(i) )
		iy = iy + sy
	end
	iy = iy + dy
	self.color_presence = tab
	self:add_slider(	{ix,iy,	8,sy},	"Concentration",	self, "concentration",	0,0,1 )
	iy = iy + sy
	bu = self:add_slider(	{ix,iy,	8,sy},	"Size",				self, "size", 			1,0,4 )
	iy = iy + sy
end

function meu:load_images()
	local imgs = {}
	for i = 1,LV:get_color_nb() do
		local name = LV:get_color_name( i )
		local dir = app.media_dir_rel.."AAALV/Colors/selfies/ByColor/"..name.."/"
		imgs[i] = IMG_SEQ:create_from_dir( name, dir )
	end
	self.imgs = imgs
end

function meu:init()
	local ref = self.ref
	self.parts = self:get_layer_bdd(3)
	ref.b_restart = param.get_ref( self.parts, "restart_trig" )
end

function meu:draw_parts( bdd )
	local nb = aaa.bdd.get_point_nb( bdd )
	aaa.show( nb, "point_nb" )
	local pts = aaa.bdd.get_points_with_id(	bdd )
	if not pts then return end

	local sha = self:get_shading()
	if not sha then return end

	local r = .1

	local imgs = self.imgs
	if not imgs then return end
	if not self.b_plaque_draw then return end

	local color_presence = self.color_presence
	local z_cam = 3.33
	local zo = -16
	local F = self.concentration * .2
	local fa = (1-F) / (zo-z_cam)
	local fb = 1 - fa*zo
	local len_fade = .3
	local fade

	local size = interpolate( self.size_cur or 1, self.size, .2 )
	self.size_cur = size
	gol.set_texture_dim(2)
	local pgr_used = gol.get_program_used()
	local col = {}
	for i=1,#pts do
		local pt = pts[i]
		local x,y,z,id = pt[1], pt[2], pt[3], pt[4]
		if z < 3.33 then
			local f = fa * z + fb
			x,y = x*f, y*f
			local id_col = (id % 6)
			local presence = color_presence[ id_col + 1 ]
			if presence > 0. then
				local seq = imgs[id_col + 1]
				local nb = seq:get_nb()
				local id_art = math.floor( (id-id_col) / 6 + 1 )
				--self:print( id_col )
				local bind = seq:get_bind( id_art % nb + 1 )
				local px,py = aaa.img.get_size( bind )
				if px then
					fade = z - zo
					if fade < 1 then
					else
						fade = z - (z_cam - len_fade)
						if fade > 0 then
							fade = (1-fade/len_fade)
						else
							fade = 1
						end
					end
					local c = LV:get_color( id_col + 1 )
					sha:set_frag_vec4( 1, c[1], c[2], c[3], presence * fade )	--self:get_alpha() )

					gol.push_matrix()
						gol.translate( x,y,z )
						local rx = px/py
						local sx = math.sqrt(rx) * .125 * size
						local sy = sx/rx

						gol.update_uniform_fragment_vec4()
						--gol.update_uniform_ubo()

						gol.bind_texture( bind )
						aaa.draw_rect_uv(	-sx,-sy, sx, sy )
					gol.pop_matrix()
				end
			end
		end
	end
	sha:set_frag_int_2( 12 )
	gol.update_uniform_fragment_int()
	--sha:set_frag_float_1_2( 1, 1 )
	gol.update_uniform_fragment_float()
	--gol.use_program(0)
	sha:set_frag_int_2( 0 )
	gol.update_uniform_fragment_int()
end

function meu:update_ui()
	local ui = self.ui
	ui.bu_info:set_text( LV:get_color_name( ui.bu_pal:get_value() ) )
end

function meu:update()
	if not self.imgs then
		self:load_images()
	end

	local bu_pal = self.ui.bu_pal
	if bu_pal then
		local id = self.ui.bu_pal:get_value()
		if id < 8  then
			local c = LV:get_color( id )
			if c then
				self.ui.bu_rgb:set_rgba( c[1], c[2], c[3] )
			end
		end
	end
end

function meu:draw()
	local sha = self:get_shading()
	if sha then
		local r,g,b = self.ui.bu_rgb:get_rgba()
		sha:set_frag_vec4( 1, r,g,b, self:get_alpha() )
	end

	self:draw_layers_begin()
		self:draw_layer( 3 )
		self:draw_layer( 1 )
		self:draw_parts( self.parts )
		local c = LV:get_color( 7 )
		if c then
			self.ui.bu_rgb:set_rgba( c[1], c[2], c[3] )
		end
		sha:set_frag_vec4( 1, c[1], c[2], c[3], self:get_bu_value( "Safran" ) )
		self:draw_layer( 4 )
	self:draw_layers_end()
end