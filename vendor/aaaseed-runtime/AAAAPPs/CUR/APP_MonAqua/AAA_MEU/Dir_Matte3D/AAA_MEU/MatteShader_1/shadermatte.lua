
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	self.y_offset = 0.0
	self.b_overwrite = false
	self.b_mesh_colored = false
	self.mesh_color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 }

	local ix = 1
	local iy = 1
	local SY = .9
	local DY = .2

	self:add_shading_ui( {1,1,	8} )

	iy = 3
	bu = self:add_button(	{ix,iy,	SY,SY},	"Overwrite Offset", 	self, "b_overwrite", false )
	iy = iy + SY

	--shitty name to help understand
	--method 1 to pass to the shader
	bu = self:add_slider(	{ix,iy,	8,SY},	"Vertical Offset", 		self, "y_offset", .0,-1.0,1.0 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY},	"Alpha Threshold", 		sha:get_ref_frag_float(1), nil, 0,0,1 )

	-- iy = 4
	-- SY = 2.5
	-- for i=1,1 do
	-- 	self:add_bu_texture_target_unit( {9,iy, nil,SY}, "Tex_"..i, i )
	-- 	iy = iy + SY
	-- end
end

function meu:init()
	local ref = self.ref
	local sha = self:add_shading()
	sha:set_save_vert_float(	false,	1,1	)
	self.sha_discard = false
	self.sha_offset = -100
	self.sha_use_color = false

	self.b_update_shader = true
end

function meu:set_discard_uv( discard )
	--self:print( discard )
	if self.sha_discard ~= discard then
		self:print( "update discard" )
		local sha = self:get_shading()
		sha:set_frag_int_1( discard )
		gol.update_uniform_fragment_int()
		self.sha_discard = discard
	end
end

function meu:set_mesh_color( color )
	local sha = self:get_shading()
	self.mesh_color = color
	if self.b_mesh_colored and self.mesh_color then
	--	self:print( "update mesh color" )
		sha:set_frag_vec4( 2, self.mesh_color.r, self.mesh_color.g, self.mesh_color.b, self.mesh_color.a )
		gol.update_uniform_fragment_vec4()
	end
end

function meu:set_use_color( val )
	if self.b_mesh_colored ~= val then
		self:print( "update use color" )
		local sha = self:get_shading()
		self.b_mesh_colored = val
		sha:set_frag_int_2( self.b_mesh_colored and 1 or 0 )
		gol.update_uniform_fragment_int()
	end
end

function meu:set_vertical_offset( uv )
	if not self.b_overwrite then
		if uv then
			self.y_offset = uv.y
		else
			self.y_offset = 0.0
		end
	end
	if self.sha_offset ~= self.y_offset then
		local sha = self:get_shading()
		sha:set_frag_vec4_y( 1, self.y_offset )
		gol.update_uniform_fragment_vec4()
		self.sha_offset = self.y_offset
	end
end

function meu:draw()
	local sha = self:get_shading()
	--self.mesh_color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 }
 	self:draw_layers_begin()
		--method 2 to pass to the shader
	--	self:print( "self.y_offset "..self.y_offset)
	--	sha:set_frag_int_2( self.b_mesh_colored and 1 or 0 )
	--	gol.update_uniform_fragment_int()
		--if self.b_mesh_colored and self.mesh_color then
--			table.print( self.mesh_color, "Mesh 2", 2)
		--	sha:set_frag_vec4( 2, self.mesh_color.r, self.mesh_color.g, self.mesh_color.b, self.mesh_color.a )
		--	gol.update_uniform_fragment_vec4()
		--end
 		self:draw_layer( 1 )
 	self:draw_layers_end()
end




