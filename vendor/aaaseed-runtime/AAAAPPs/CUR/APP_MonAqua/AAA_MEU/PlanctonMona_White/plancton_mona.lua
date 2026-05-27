
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui

	self:define_ui_oclgl_galaxy()
	self:add_bu_texture_target_layer(	{1,4}, "TexA", 1, false,	self:get_opencl_layer()	)
	local sha = self:get_shading()

	local ix,iy = 1,2
	local SY = 1
	iy = iy + SY
	local col = {}
	col[1] = self:add_rgba(	{ix,iy,		4,1},	"color_1", false )
	col[2] = self:add_rgba(	{ix+4,iy,	4,1},	"color_2", false )
	ui.color = col
	iy = 7
	self:add_slider(		{ix,iy,		8,SY},	"Hardness",		sha:get_ref_frag_float(1),	nil, 1, 0, 8 )

	iy = 8.2

	local SX = 8/3
	iy = iy + SY
	self:add_slider_two(	{ix,iy,		8,SY},	"Speed_range",	nil, sha:get_ref_comp_float(1), sha:get_ref_comp_float(2), 0, 1, 0, 1 )
	iy = iy + SY
	self:add_slider(		{ix,iy,		8,SY},	"Speed", 		sha:get_ref_comp_float(3),	nil, 0, 0, 1 )
	iy = iy + SY
	self:add_slider(	{ix+SX*0,iy,	SX,SY},	"Speed_x",		sha:get_ref_comp_vec4_x(1), nil, 0, 0, 1 ):set_color_back("x")
	self:add_slider(	{ix+SX*1,iy,	SX,SY},	"Speed_y",		sha:get_ref_comp_vec4_y(1), nil, 0, 0, 1 ):set_color_back("y")
	self:add_slider(	{ix+SX*2,iy,	SX,SY},	"Speed_z",		sha:get_ref_comp_vec4_z(1), nil, 0, 0, 1 ):set_color_back("z")
	iy = iy + SY
	self:add_slider_two(	{ix,iy,		8,SY},	"Size", 		nil, sha:get_ref_vert_float(1), sha:get_ref_vert_float(2), 0, 1, 0, 4 )
	iy = iy + SY
	self:add_slider(		{ix,iy,		8,SY},	"Density",		self, "density", 0, 0, 1 )

end

function meu:init()
	local ref = self.ref
	self:ogl_init()
	local bdd = self:get_bdd()
	ref.bdd = bdd
	ref.point_nb 		= param.get_ref( bdd, "point_nb" )
	ref.point_nb_used	= param.get_ref( bdd, "point_nb_used" )
	ref.restart_trig	= param.get_ref( bdd, "restart_trig" )
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
end

function meu:draw()
	app:switch_mu( "NdcDof_1", false )		--hackmona switch here is kind of confusing

	if ga:get_pass_info().name=="shadow" then return end
	if not app:is_kake() then return end

	local scene_name = app:get_scene()
	if not scene_name then return end

	if scene_name~="DN1" then
		self.time = 0	-- reset time
		param.set( self.ref.restart_trig, true )
		return
	end

	app:switch_mu( "NdcDof_1", true )		--hackmona switch here is kind of confusing
	local ref = self.ref
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
	local sk = app:get_kake_size()
	local pos
	local size
	local density_correction = 1
	if app:is_kc() then
		pos = { 21+depth*.5+d_glass, pos_y, 0 }
		size = { depth, h, 11 }
		density_correction = 11/(sk*3)	-- ratio of box size
	else
		local li = app:get_location_index() or 2	-- or is just a secu in case should not happen
		pos = { app:get_kake_center_to_entrance_from_index(li), pos_y, -z_klr }	--default pos of KL
		size = { sk*3., h, depth }		--default size of KL
		if app:is_kr() then
			pos[3] = z_klr	-- we change change z from kl
		end
	end

	aaa.show( scene_name.." "..time, "ponte time" )
	local ph = time	--wrap( time, 0, 12 )
	local density = math.fn_linear(	ph, 1,
			0,		0,
			10, 	5,
			30, 	20,
			50, 	40,
			70,		20,
			90,		0,
			100,	0
		)
	--density = 100
	aaa.show( density, "Coraux" )
	density = density * .01 * self.density

	--local nb = param.get( ref.point_nb )
	param.set( ref.point_nb, nb )
	param.set( ref.point_nb_used, clamp_01( density * density_correction) * nb )
	sha:set_vert_vec4( 1, pos[1], pos[2], pos[3] )
	sha:set_vert_vec4( 2, size[1], size[2], size[3] )

	if app:is_kc() then
		sha:set_comp_vec4( 2, 0,0, 0 )
	else
		local item = app:get_presence_SO( app:get_location() )
		local u,v = item.u, item.v
		if app:is_kl() then v = 1.-v end
		sha:set_comp_vec4( 2, u, v, item.coverage )
	end

	MEU.draw( self )

--	if self:get_bu_value( "visu" )==1 then
--		self:draw_oclgl_galaxy_visu()
--	end
end
