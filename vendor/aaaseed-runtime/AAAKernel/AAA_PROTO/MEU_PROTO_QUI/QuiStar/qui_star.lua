function meu:define_meu_infos( )
	return { 	author = "Quiwith mâa",
				tags = { "3d", "Point", "procedural", "art", "vj", "unfinished", "tutorial" },
				help =	{	"MEU encapsulating a c_bdd_particle, c_bdd_particle deal with a cpu rich particle system."
						}
			}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local SY = 1
	local DY = .2
	local ix,iy = 1,5
	local bdd = ref.bdd

	local SX = 8/3

	bu = self:add_slider( {ix,iy, SX,SY}, "Emit X", bdd, "emission_origin_x", 0, -8,8 )
		bu:set_color_back( "x" )
	bu = self:add_slider( {ix+SX,iy, SX,SY}, "Emit Y", bdd, "emission_origin_y", 0, -8,8 )
		bu:set_color_back( "y" )
	bu = self:add_slider( {ix+SX*2,iy, SX,SY}, "Emit Z", bdd, "emission_origin_z", 0, -8,8 )
		bu:set_color_back( "z" )
	iy = iy + SY + DY

	self:add_button( {ix,iy, 3,SY}, "Restart", bdd, "restart_trig" )
iy = iy + SY + DY

	self:add_button( {ix,iy, 3,SY}, "sphere", bdd, "emission_mask_sphere")
	iy = iy + SY + DY
	bu = self:add_slider( {ix,iy, 4,SY}, "life_time", bdd, "life_time", 0, 0,1000 )
	
	ix,iy = 9,2
	self:add_bu_texture_target_unit(	{ix,iy,			4,5},	"TEX", 1 ) 

	self:add_rgbf(	{1,2 } )
	self:add_blending( {2,3} )

	self:add_camera()
	self:add_rendering()


end

function meu:init()
	local ref = self.ref

	ref.bdd = self:get_layer_bdd( 1 )
end