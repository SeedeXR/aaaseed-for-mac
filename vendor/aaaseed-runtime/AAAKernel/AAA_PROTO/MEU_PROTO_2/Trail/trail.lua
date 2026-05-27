function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local sha = self:get_shading()

	local SX,SY = 8,1
	local SYC = SY*.6
	local DY = .2

	local ix,iy = 1,1
	self:add_size_uvf_video(		{ix,iy,		SX,SY*2}	)
	iy = iy + SY*2 + DY
	self:add_trz(					{ix,iy,		SX,SY*2}	)
	iy = iy + SY*2 + DY
	self:add_nb_uv(					{ix,iy,		SX,SY}		)
	iy = iy + SY
	self:add_bu_texture_target_unit(			{ix,iy,		8,6},		"Tex", 1, true )

	iy = iy + 6
	self:add_mapping_by_side_only(	{ix,iy, 	SX,SY}		)
	iy = iy + SY + DY

	self:add_rendering(				{ix,iy, 	SX,SY*2}	)

	ix,iy = 9,1
	self:add_camera()
	iy = iy + SY + DY
	self:add_shading_ui(			{ix,iy,		8,SY}	)
	iy = iy + SY + DY

	self:add_button(				{ix,iy,		3,SY},		"Inverse",	sha:get_ref_frag_int(1),nil 	)
	iy = iy + SY
	ui.bu_color = self:add_rgbfa(	{ix,iy,		8,SY},		"Color", 	false )
	iy = iy + SY + DY

	ui.bu_offset = self:add_rgbfa(	{ix,iy,		8,SY},		"Offset", 	false )
	ui.bu_offset:set_rgba( 0,0,0, 0 )
	iy = iy + SY

	ui.bu_gamma = self:add_rgbfa(	{ix,iy,		8,SY}, 		"Gamma",	false )
	iy = iy + SY + DY
	self:add_button(				{ix,iy,		2,SY }, 	"Clamp",	sha:get_ref_frag_int(2)	)
	iy = iy + SY + DY

	self:add_blending( {ix,iy, 2,SY} )

end

function meu:init()
	local sha = self:add_shading()
	-- sha:set_save_frag_int(		false,	1,3 )
	-- sha:set_save_frag_float(	false,	1,3 )
	-- sha:set_save_frag_vec4(		false,	1,1 )
	-- sha:set_save_frag_vec4(		false,	4,5 )
end

function meu:update()
end

function meu:__draw_low( alpha )	--todo restore dim
--	bind1 = 33
--	bind2 = 37
	--aaa.print_fn()
	local ref = self.ref

	-- param.set( ref.tex_dim, dim )

	ref.color:set_alpha( alpha )

	local sha = self:get_shading()

	local ui = self.ui
	local r,g,b,a

	r,g,b,a = ui.bu_color:get_rgba()
	sha:set_frag_vec4( 1, r,g,b, a*alpha )

	r,g,b,a = ui.bu_offset:get_rgba()
	sha:set_frag_vec4( 2, r,g,b, a )

	r,g,b,a = ui.bu_gamma:get_rgba()
	sha:set_frag_vec4( 3, r,g,b, a )


	--self:print( r.." "..g.." "..b.." "..a )

	--gol.dump_tex_unit()
	self:draw_layer( 1 )
	--gol.dump_tex_unit()
	--aaa.obj.update_then_draw( ref.__layer_marked )
end

function meu:draw()
	local ref = self.ref

	self:draw_layers_begin()

		local color = ref.color
		local alpha = color:get_alpha()

		self:__draw_low( self:get_alpha() )

		--self:draw_layer( 1 )
	self:draw_layers_end()

--	self.ui.bu_go:print( "TEST" )
end
