
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local sha = self:get_shading()

	local ix, iy = 1,1
	local SY = .8
	local DY = .2

	self:add_shading_ui( {1,1} )
	self:add_camera()
	iy = iy + 1	
	local y_begin = iy + DY

--------------------
	self:set_tab_key( "Main" )
	ix, iy = 1, y_begin
	
	bu = self:add_trig_method(	{ix,iy,		4,SY},		"Load", 		self,		"load_media"	 )
	iy = iy + SY

	ix,iy = 9,5
	local SX = 8/3
	bu = self:add_slider(		{ix,iy,	8,SY},	"Size", 		self,	"size",			0,	0,1		)
		iy = iy + SY*2
	
	-- self:add_blending( {ix,iy} )

	self:add_rendering( {ix,iy} )
end

function meu:init()
	local sha = self:add_shading( 2 )
end

function meu:load_media()
	local is = IMG_SEQ:create_from_dir( "Marion", aaa.dir.make_media_path("Marion/DecoupageXXX/"), "PXL_*.tga" )
	self:print( " bind "..is:get_bind(1).." to "..is:get_bind(is:get_nb()) )
	self.img_seq = is
end

function meu:draw()
	local is = self.img_seq
	if not is then return end

	local ref = self.ref
	local ui = self.ui


	self:draw_layers_begin()

		self:draw_layer(1)
		self:draw_layer(2)

		math.randomseed( 42 )
		local sx,sy = 8,4
		for i=1,1200 do
			local img = is:get_img_rnd()
			local x,y = (math.random()-.5)*sx,  (math.random()-.5)*sy
			local sx = (.1 + math.random() * 64)
			local rz = math.random()
			img:draw_xyz_sx_rotz(  x,y,-sx,	sx, rz )
		end

	self:draw_layers_end()
end