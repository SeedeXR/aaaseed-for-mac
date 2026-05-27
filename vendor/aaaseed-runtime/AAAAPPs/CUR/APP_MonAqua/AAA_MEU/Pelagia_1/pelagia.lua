function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "2d", "3d", "proprietary", "art", "input", "texture", "tutorial", "Procedural" },
			help = 	{
					"Mâa's Hand written MEU for interactive the Pelagia scene in Monaco Aquarium",
					"Done very quickly when finally I got some quiet time to do the magic",
					"Very specific but a good example"
					}
		}
end

function meu:define_ui()
	local ui = self.ui
    local ix, iy = 2, 2
	local bu
	local SY = 1

	bu = self:add_text_info(	{ix,iy,	16,SY}, "time_info" )
		ui.bu_info = bu
	iy = iy + SY

    self:add_button( {ix,iy, 4,SY}, "KL", self, "b_draw_kl", true )
    iy = iy + SY
    self:add_button( {ix,iy, 4,SY}, "KC", self, "b_draw_kc", true )
    iy = iy + SY
   	self:add_button( {ix,iy, 4,SY}, "KR", self, "b_draw_kr", true )
    iy = iy + SY
	-- self:add_button( {ix, iy }, "Grid Overlap", self, "b_draw_grid_overlap", false )
    -- iy = iy + SY
	-- self:add_button( {ix, iy }, "Cross", self, "b_draw_cross", false )
    -- iy = iy + SY
	-- self:add_button( {ix, iy }, "Circle", self, "b_draw_circle", false )
	-- iy = iy + SY * 2
  	-- self:add_button( {ix, iy }, "Auto", self, "b_auto", false )
    -- iy = iy + SY

	-- self.bu_color = self:add_rgbfa(	{ix,iy,	8,2}, 	"Color", aaa.obj.get_down_by_class( self:get_layer(3), "color" ) )
	-- iy = iy + SY

--	bu = self:add_button(	{ix,		iy,		SY, SY },	"Circles", 			self, "b_cercles", false )
--	bu = self:add_slider(	{ix+4,iy,	4,SY},	"Circles Nb U",		self, "circles_nb_u", 5, 2, 16 ):set_value_type_integer(true)
	bu = self:add_slider(	{ix,iy,		8,SY},	"Density",		self, "density", .5, 0, 1. )

end

function meu:init()
    local ref = self.ref
	self.info = {}
end

function meu:draw_transparent( tank, post, info )
	local grea = tank:get_grea_by_name( "PELAGIA_"..post )
	if grea then
		grea:draw_transparent( info )
	end
end

function meu:update_ui()
	self.ui.bu_info:set_text( "Time is "..self.time )
end

function meu:draw()
	if (not app:is_kake()) and (not app:is_pc_dev() ) or (app:is_tablet())  then return end

	local scene_name = app:get_scene()
	if not scene_name then return end
	--self:print( scene_name )
	if scene_name~="PIN2" then
		self.time = 0	-- reset time
		return
	end

	local info = self.info
	--inc time
	local time = (self.time or 0) + aaa.time.dt
	self.time = time
	info.time = time

	local m = app:get_meu_by_name_cached( "TankFish_PIN2" )
	if not m then return end
 	local tank = m.tank
	if not tank then return end

	local meu_fbx = app:get_meu_by_name_cached( "Fbx_PELAGIA" )
	if meu_fbx then
		info.bind_beg, info.bind_end = meu_fbx:get_tex_seq_bind()
	end

 	self:draw_layers_begin()
		self:draw_layer(1)

		local density = math.fn_linear(	self.time, 1,
			0, 1,
			60, 1,
			80,	300/1900,
			116, 15/1900,
			150, 0,
			151, 0
		)
		info.density = self.density * density

		if self.b_draw_kl and (app:is_kl() or app:is_pc_dev()) then
			info.axe = "z"
			self:draw_transparent( tank, "KL", info )
			--self:print( "simul kl" )
		end
		if self.b_draw_kr and (app:is_kr() or app:is_pc_dev()) then
			info.axe = "z"
			self:draw_transparent( tank, "KR", info )
			--self:print( "simul kr" )
		end
		if self.b_draw_kc and (app:is_kc() or app:is_pc_dev()) then
			info.axe = "x"
			self:draw_transparent( tank, "KC", info )
		end

    self:draw_layers_end()
end
