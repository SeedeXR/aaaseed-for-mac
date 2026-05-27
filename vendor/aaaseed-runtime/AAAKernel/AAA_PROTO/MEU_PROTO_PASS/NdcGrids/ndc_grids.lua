
function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "2d", "texture", "core", "coregraphic", "renderpass", "draw", "unfinished" },
			help = 	{
					"Add lines, grid, circle as technical elements for the output to help the setup process",
					"an optional last pass before outputing the image",
					"Output to the current Fbo"
					}
		}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local ix = 1
	local iy = 1
	local SY = .9
	local SYC = 2
	local DY = SY*.5

	--self:add_camera()

	self:add_rendering( 	nil, 			{ ref=self:get_layer_obj( 2, "rendering" )  } )

	local bdd_grid = self:get_layer_bdd(3)
	ix,iy  = 9,2
	self:add_button(		{ix,iy,		SY,SY},	"BackGround", 		self, "b_back", false ):set_text_visible(false)
	self:add_rgbfa(			{ix+1,iy,	7,SY}, "BackGround_color",	aaa.obj.get_down_by_class( self:get_layer(1), "color" ) )
	--self:set_tab_key( "Main" )
	iy = iy + SY + DY

	bu = self:add_button(	{ix,iy,			SY,SY},		"Resize",		self, "b_resize", 	false		):set_text_visible(false)
	bu = self:add_slider(	{ix+SY,iy,		4-SY,SY},	"Offset X",		self, "offset_x",	0, -1,1 ):set_color_back("x")
	bu = self:add_slider(	{ix+SY,iy+SY,	4-SY,SY},	"Offset Y",		self, "offset_y",	0, -1,1 ):set_color_back("y")
	
	bu = self:add_slider(	{ix+4,iy,		4,SY},	"Size X",		self, "size_x",		1, 0,1 ):set_color_back("x")
		bu:add_values_def( .125, .25, .375, .5, .625, .75, .875 )
	bu = self:add_slider(	{ix+4,iy+SY,	4,SY},	"Size Y",		self, "size_y",		1, 0,1 ):set_color_back("y")
		bu:add_values_def( .125, .25, .375, .5, .625, .75, .875 )

	ix,iy  = 1,1
	self:add_button(		{ix,iy,			SY,SY},	"Foreground", 		self, "b_fore", true ):set_text_visible(false)
	self:add_rgbfa(			{ix+1,iy,		7,SY},	"Color",				aaa.obj.get_down_by_class( self:get_layer(2), "color" ) )
	iy = iy + SY
	bu = self:add_button(	{ix,iy,			SY,SY},	"Grid", 			self, "b_grid", false )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,			4,SY},	"x_nb",				self, "nb_x", 16, 4, 256 ):set_color_back("x"):set_value_type_integer(true)
		bu:add_values_def( 5,9,17,33,65,129)
	bu = self:add_slider(	{ix+4,iy,		4,SY},	"y_nb",				self, "nb_y", 16, 4, 256 ):set_color_back("y"):set_value_type_integer(true)
		bu:add_values_def( 5,9,17,33,65,129)

	iy = iy + SY

	bu = self:add_button(	{ix,iy,			SY,SY},	"Cross", 			self, "b_cross", false )
	bu = self:add_button(	{ix+4,iy,		SY,SY},	"Circle", 			self, "b_cercle", false )
	bu = self:add_slider(	{ix+4,iy+SY,	4,SY},	"Circle Nb", 		self, "circle_nb",		4,	1,16 )
		:set_value_type_integer(true):set_text("Nb")
	iy = iy + SY * 2 + DY

	bu = self:add_button(	{ix,iy,			SY,SY},	"Circles", 			self, "b_cercles", false )
	bu = self:add_slider(	{ix+4,iy,		4,SY},	"Circles Nb X", 	self, "circles_nb_x",	5,	2,16 )
		:set_value_type_integer(true):set_color_back("x"):set_text("Nb X")
	bu = self:add_slider(	{ix+4,iy+SY,	4,SY},	"Circles Nb Y", 	self, "circles_nb_y",	5,	2,16 )
		:set_value_type_integer(true):set_color_back("y"):set_text("Nb Y")
	iy = iy + SY*2
	--self:set_tab_key( "Overlaps" )

	-- Horizontal overlap grids

	local y_overlap = iy
	self:add_text_info(		{ix,iy,		6,.7},	"Overlap Horizontal" )
	iy = iy + SY --* 1.5
	for i=1,5 do
		bu = self:add_button(	{ix,iy,		2,SY},	"OH"..i, 					self, "b_overlap_hori_"..i, false )
		if i == 1 then bu:set_text("OH") end
		bu = self:add_slider(	{ix+2,iy,	2,SY},	"hori "..i.." Position",	self, "hori_pos_"..i, .5, 0, 1 ):set_color_back("x")
			bu:add_values_def( .25, .5, .75 )
			bu:set_text( "Position" )
		bu = self:add_slider(	{ix+4,iy,	2,SY},	"hori "..i.." Size",		self, "hori_size_"..i, .1, 0, 1 ):set_color_back("x")
			bu:add_values_def( .125, .25, .375, .5, .625, .75, .875 )
			bu:set_text( "Size" )
		bu = self:add_slider(	{ix+6,iy,	2,SY},	"hori "..i.." nb",			self, "nb_h_"..i, 4, 4, 16 ):set_value_type_integer(true)
			bu:set_color_back("x")
			bu:set_text( "Nb" )
		iy = iy + SY
	end

	-- Vertical overlap grids
	ix,iy = 9,y_overlap
	self:add_text_info(	{ix,iy,	6,.7},	"Overlap Vertical" )
	iy = iy + SY --* 1.5
	for i=1,5 do
		bu = self:add_button(	{ix,iy,		2,SY},	"OV"..i, 					self, "b_overlap_vert_"..i, false )
		if i == 1 then bu:set_text("OV") end
		bu = self:add_slider(	{ix+2,iy,	2,SY},	"vert "..i.." Position",	self, "vert_pos_"..i, .5, 0, 1 ):set_color_back("y")
			bu:add_values_def( .25, .5, .75 )
			bu:set_text( "Position" )
		bu = self:add_slider(	{ix+4,iy,	2,SY},	"vert "..i.." Size",		self, "vert_size_"..i, .1, 0, 1 ):set_color_back("y")
			bu:add_values_def( .125, .25, .375, .5, .625, .75, .875 )
			bu:set_text( "Size" )
		bu = self:add_slider(	{ix+6,iy,	2,SY},	"vert "..i.." nb",			self, "nb_v_"..i, 4, 4, 16 ):set_value_type_integer(true)
			bu:set_color_back("y")
			bu:set_text( "Nb" )
		iy = iy + SY
	end


--we just use the current fbo here no src
--todo is this an Ndc Meu ?
--	self:define_ndc_src(	{9,3.2,		8,4 },	{"Src"} )
end

function meu:set_background_only( b_on )
	self:set_bu_value( "background", b_on  )
	self:set_bu_value( "Foreground", false  )
end

function meu:init()
	local ref = self.ref
	self:add_shading()
	ref.bdd_grid = self:get_layer_bdd(3)
	ref.nb_u = param.get_ref( ref.bdd_grid, "nb_u" )
	ref.nb_v = param.get_ref( ref.bdd_grid, "nb_v" )
	ref.x = param.get_ref( ref.bdd_grid, "center_x" )
	ref.y = param.get_ref( ref.bdd_grid, "center_y" )
	ref.model = aaa.obj.get_down_by_class( self:get_layer(3), "model" )
	ref.su = param.get_ref( ref.model, "size_u" )
	ref.sv = param.get_ref( ref.model, "size_v" )
end

function meu:draw_grid( x, y, nb_u, nb_v, sx, sy )
	local ref = self.ref
	param.set( ref.x, x )
	param.set( ref.y, y )
	param.set( ref.nb_u, nb_u )
	param.set( ref.nb_v, nb_v )
	param.set( ref.su, sx )
	param.set( ref.sv, sy )
	self:draw_layer( 3 )
end

function meu:draw()

 	self:draw_layers_begin()
		if false then
			self:draw_layer( 1 )
			self:draw_layer( 2 )
			self:draw_layer( 3 )
		else
			local ox,oy, sx,sy
			if self.b_resize then
				ox,oy, sx,sy = self.offset_x,self.offset_y, self.size_x,self.size_y
			else
				ox,oy, sx,sy = 0,0, 1,1
			end

			if self.b_back then
				self:draw_layer( 1 )
				aaa.draw_rect_axe_z(	ox,oy,0,	sx*2,sy*2	)
			end

			if self.b_fore then
				self:draw_layer( 2 )	--attr
				if self.b_grid then
					self:draw_grid( ox,oy, self.nb_x+1,self.nb_y+1, sx,sy )
				end
				if self.b_cross then
					gol.draw_lines_2d( ox-sx,oy-sy,ox+sx,oy+sy, ox+sx,oy-sy, ox-sx,oy+sy )
				end
				local nb = 96
				if self.b_cercle then
					local dr = 2 / self.circle_nb
					for r = dr,2, dr do
						aaa.draw_ellipse_line_axe_z( ox,oy,0,	sx*r,sy*r,	nb )
					end
				end
				if self.b_cercles then
					local nb_pt = nb/2
					local nb_x = self.circles_nb_x
					local nb_y = self.circles_nb_y
					local dx = sx*2/nb_x
					local dy = sy*2/nb_y
					for i = 1,nb_x do
						local x = ox + (i-.5-nb_x/2) * dx
						for j = 1,nb_y do
							local y = oy + (j-.5-nb_y/2) * dy
							aaa.draw_ellipse_line_axe_z( x,y,0, dx,dy, nb_pt )
						end
					end
				end
				for i=1,8 do
					if self["b_overlap_hori_"..i] then
						self:draw_grid( ox+(self["hori_pos_"..i]-.5)*2*sx, oy,	self["nb_h_"..i] +1, self.nb_y*4+1, self["hori_size_"..i]*sx, sy )
					end
				end
				for i=1,8 do
					if self["b_overlap_vert_"..i] then
						self:draw_grid( ox, oy+(self["vert_pos_"..i]-.5)*2*sy,	self.nb_x*4+1, self["nb_v_"..i] +1, sx, self["vert_size_"..i]*sy )
					end
				end
			end
		end
	self:draw_layers_end()
end



