function meu:define_meu_infos( )
	return { 	author = "Mâa from a shader by Inigo Quilez (iq)",
				tags = { "2d", "3d", "Vj", "Draw", "procedural", "texture", "unfinished" },
				help =	{	"compute volumetric clouds in a texture and draw it mapped on a grid.",
							"Computed by a fragment shader doing ray marching.",
							"The UI need to be done"
						}
			}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local ix,iy = 1,2
	local SY = 1

	self:add_rgbf(	{1,8.2 } )
	self:add_mapping_by_side()

	self:add_mapping_hexa()
	self:add_nb_uv()

	self:add_trig_method( 	{ix,iy,		2,SY},	"Phase Reset",	self, "reset_phase"	)
		:set_text( "Reset" )
		:set_color_back( "reset" )
	self:add_sliders_xyz(	{ix+2,iy,	6,SY},	"Phase",		self.phase, false,		4		)
	iy = iy + SY

	self:add_button( 		{ix,iy,		2,SY},	"Phase Speed",	self, "b_phase_move", 	false	)
		:set_text( "Speed" )
	self:add_sliders_xyz(	{ix+2,iy,	6,SY},	"Speed",		self.speed, false,		4		)
	iy = iy + SY

	self:add_camera()

	ix,iy = 9,4
	self:add_shading_ui( {ix,iy,	8,SY} )

end

function meu:reset_phase()
	self.phase.x = 0
	self.phase.y = 0
	self.phase.z = 0
end

function meu:init()
	local ref = self.ref
	aaa.layer.set_bind_2d( self:get_layer(1), self:get_bind_by_name("noise") )
	local sha = self:add_shading()
	self.phase = {}
	self:reset_phase()
	self.speed = { x=0, y=0, z=0 }
	
	--sha:set_save_frag_float( false, 1,8 )
end

function meu:update()
	local sha  = self:get_shading()
	local dt = aaa.time.dt
	if self.b_phase_move then
		self.phase.x = self.phase.x + self.speed.x * dt
		self.phase.y = self.phase.y + self.speed.y * dt
		self.phase.z = self.phase.z + self.speed.z * dt
	end
	sha:set_frag_vec4( 1, self.phase.x, self.phase.y, self.phase.z )

	--todo add a time ui
	sha:set_frag_float_4( aaa.time.t )
end

