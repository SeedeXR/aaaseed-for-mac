
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui

	self:add_bu_texture_target_layer(	{1,4}, "TexA", 1, false,	self:get_opencl_layer()	)

	local sha = self:get_shading()

	self:add_camera()
	self:add_shading_ui(	{9,2.5,		8,1 } )

	local ix,iy = 1,1
	local SY = 1
	local DY = .2
	

	self:add_particle_nb_ui({ix,iy,		8,SY*2}, ref.bdd, "particle" )
	iy = iy + SY*2 + DY

	local col = {}
	col[1] = self:add_rgba(	{ix,iy,		4,1},	"color_1", false )
	col[2] = self:add_rgba(	{ix+4,iy,	4,1},	"color_2", false )
	ui.color = col
	iy = 7
	self:add_slider(	{ix,iy,			8,SY},	"Hardness",		sha:get_ref_frag_float(1),	nil, 1, 0, 8 )

	iy = 8.2

	local SX = 8/3
	iy = iy + SY
	self:add_slider_two({ix,iy,			8,SY},	"Speed_range",	nil, sha:get_ref_comp_float(1), sha:get_ref_comp_float(2), 0, 1, 0, 1 )
	iy = iy + SY
	self:add_slider(	{ix,iy,			8,SY},	"Speed", 		sha:get_ref_comp_float(3),	nil, 0, 0, 1 )
	iy = iy + SY
	self:add_slider(	{ix+SX*0,iy,	SX,SY},	"Speed_x",		sha:get_ref_comp_vec4_x(1), nil, 0, 0, 1 ):set_color_back("x")
	self:add_slider(	{ix+SX*1,iy,	SX,SY},	"Speed_y",		sha:get_ref_comp_vec4_y(1), nil, 0, 0, 1 ):set_color_back("y")
	self:add_slider(	{ix+SX*2,iy,	SX,SY},	"Speed_z",		sha:get_ref_comp_vec4_z(1), nil, 0, 0, 1 ):set_color_back("z")
	iy = iy + SY
	self:add_slider_two({ix,iy,			8,SY},	"Size", 		nil, sha:get_ref_vert_float(1), sha:get_ref_vert_float(2), 0, 1, 0, 4 )
	iy = iy + SY
	self:add_slider(	{ix,iy,			8,SY},	"Density",		self, "density", 0, 0, 1 )

end

function meu:init()
	local ref = self.ref
	self:ogl_init()
	local bdd = self:get_bdd()
	ref.bdd				= bdd
	ref.point_nb 		= param.get_ref( bdd, "point_nb" )
	ref.point_nb_used	= param.get_ref( bdd, "point_nb_used" )
end

function meu:get_bdd()		return self:get_layer_bdd( 6 )	end

function meu:set_point_nb( nb )
	local bdd = self:get_bdd()
	param.set( bdd, "point_nb_used", nb )
	param.set( bdd, "point_nb", nb )
end

function meu:set_xyz_sxyz( x, y, z, sx, sy, sz )
	local sha = self:get_shading()
	sha:set_vert_vec4( 1, x, y, z )
	sha:set_vert_vec4( 2, sx, sy, sz )
end

function meu:update()

	local ref = self.ref
	local ui = self.ui
	local sha = self:get_shading()

	for i=1,2 do
		local r,g,b,a = ui.color[i]:get_rgba()
		sha:set_frag_vec4( i, r,g,b,a )
	end

	self:update_particle_nb()
end

function meu:draw()
	if ga:get_pass_info().name=="shadow" then return end

	local sha = self:get_shading()

	--inc time
	local time = (self.time or 0) + aaa.time.dt
	self.time = time
	sha:set_comp_float( 4, aaa.time.dt )

	local nb = 1024*1024			--max nb of paticule we use
	local depth = 8					-- depth of particle box
	local d_glass = 0				-- distance of glass
	local z_klr = depth + d_glass
	local h = 13					--height of particle box
	local pos_y = 6					--position of particle box
--	local pos = {10,pos_y,-z_klr}	--default pos of KL
--	local size = {28,h,depth}		--default size of KL
--	local density_correction = 1	--


--	sha:set_vert_vec4( 1, pos[1], pos[2], pos[3] )
--	sha:set_vert_vec4( 2, size[1], size[2], size[3] )

	sha:set_comp_vec4( 2, 0,0, 0 )

	MEU.draw( self )
--	if self:get_bu_value( "visu" )==1 then
--		self:draw_oclgl_galaxy_visu()
--	end
end
