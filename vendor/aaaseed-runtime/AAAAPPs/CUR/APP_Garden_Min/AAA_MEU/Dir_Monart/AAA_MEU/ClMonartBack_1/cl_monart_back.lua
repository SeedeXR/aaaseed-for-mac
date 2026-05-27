--todo rename
function meu:define_meu_infos( )
	return { 	author = "Mâa",
				version = 0,
				tags = { "Proprietary", "art", "procedural", "2d","3d", "Draw", "VJ", "texture", "Unfinished" },
				help = "Use in the Monart Environement, probably to do the backgraound, use OpenCl",
			}
end

function meu:define_ui()
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

	self:add_oclgl_bu_textures( {1,4.5}, "TEX", 3 )

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

function meu:init()			self:ocl_init()		end

function meu:update()
	local bind
	local ocl = self:get_opencl()
	for i=1,3 do
		bind = self:get_texture_bind_2d( i )
		if bind then ocl:set_tex_src( i, bind ) end
	end

	local ref = self.ref
	local sha = self:get_shading()
	local ui = self.ui

	--todoocl deal with this pattern
	local alpha = self:get_alpha()
	for i= 1,1 do
		local r,g,b,a = ui.color[i]:get_rgba()
		--ui.color[i]:print( i.." "..r.." "..g.." "..b.." "..a)
		--sha:set_frag_vec4( i, 1,.5,1,1 * alpha )
		sha:set_frag_vec4( i, r,g,b, a * alpha )
	end

---	local sa = math.abs( self.end_z - self.begin_z )
--	sa = sa~=0. and (1./sa) or 1
	param.set( ocl:get_param_ref(1), self.speed_z ) --they were multiply by sa
	param.set( ocl:get_param_ref(5), self.speed_x )
	param.set( ocl:get_param_ref(6), self.speed_y )

--[[
	local inf = self.petal
	if self.petal_freq==0 then
		inf = 0
	end
	param.set( sha:get_ref_frag_float( 2 ), inf*.5 )
	param.set( sha:get_ref_frag_float( 3 ), self.petal_freq)
--]]

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
