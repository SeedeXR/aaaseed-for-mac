--todo rename MEU
function meu:define_meu_infos( )
	return { 	author = "Mâa",
				version = 0,
				tags = { "Proprietary", "art", "procedural", "2d", "Draw", "texture", "Unfinished" },
				help = "Used for Jeddah, use opencl: restore or dump",
			}
end

function MEU:ocl_dpool_define_ui()
	local ref = self.ref
	local ui = self.ui

	local ocl = self:get_opencl()
	local sha = self:get_shading()

	local bu, ix, iy

	self:add_opencl_base()

	ui.color = self:add_color_1( {1,3} )
	self:add_camera()

	self:set_tab_key_def()
	ix, iy = 1, 6
	iy = iy + 1

	self:set_tab_key( "TEX" )
	self:add_oclgl_bu_textures( {1,4.5}, nil, 2 )
	self:add_bu_texture_target_layer( {9,3.5}, "TEX", 3, true,	self:get_layer(5)	)
	self:set_tab_key_def()

	self:add_oclgl_fade( ix, iy )

	ix, iy = 9, 5
	self:add_oclgl_xyz( {ix,iy}, "Speed" )

	iy = iy + 1
	self:add_oclgl_slider_ocl_param_target(	{ix,iy+1,	4,1},	"Speed_factor",		ocl:get_param_ref(7),	0,	-1,1 )
	iy = iy + 1
	self:add_oclgl_slider_ocl_param_target(	{ix,iy+1,	4,1},	"Image_repulse",	ocl:get_param_ref(8),	0,	-1,1 )
	self:add_oclgl_slider_ocl_param_target(	{ix+4,iy+1,	4,1},	"Viscosity",		ocl:get_param_ref(9),	0,	0,.99999 )

	param.set_comment( ocl:get_param_ref(1), "Speed_z" )
	param.set_comment( ocl:get_param_ref(5), "Speed_x" )
	param.set_comment( ocl:get_param_ref(6), "Speed_y" )

	ix, iy = 9, 10
	bu = self:add_slider_two(	{ix,iy,	8,1},	"Part_Size" )
		bu:set_target_param( sha:get_ref_vert_float( 2 ), 1,	"Part_Size_Min" )
		bu:set_target_param( sha:get_ref_vert_float( 3 ), 2,	"Part_Size_Max" )
		bu:set_min_max( 0, 32, 1 )
		bu:set_min_max( 0, 32, 2 )

end

function meu:define_ui()	self:ocl_dpool_define_ui()	end
function meu:init()			self:ocl_init()				end


function meu:update()
	local bind
	local ocl = self:get_opencl()
	for i=1,2 do
		bind = self:get_texture_bind_2d( i )
		if bind then ocl:set_tex_src( i, bind ) end
	end

	self:update_ocl_dpool( true )

--[[
	local ph = self.ph or -1.
	ph = ph + aaa.time.dt
	if ph > 40. then
		ph = -1
	end
	self.ph = ph
--]]

--[[
	ph = clamp_01( ph / 30.)
	ph = math.pow( ph, 2 )
	ocl:set_nb_by_set( ph * 1024 * 1024 * 4 )
--]]
end

function meu:draw()
	MEU.draw( self )
end
