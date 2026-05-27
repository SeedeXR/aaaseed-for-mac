function meu:define_ui()
	local ref = self.ref
	local bu

	self:add_bu_texture_target_unit()

	self:add_mapping_by_side_only()
	self.ui.bu_col		= self:add_rgbf(	{1,6.8,	8,1 })

	self:add_button( {1,9.2, 1, 1 }, "Square", self, "b_square", true )

	self:add_blending()

	self:add_camera()

	self:add_rendering()

	self:add_size_uvf_video()

	self:add_monitor()
end

function meu:update()
	local ref = self.ref
	local g = self.b_square and 1 or (.6 + math.sin( aaa.time.t  * 2 ) * .4)
	--local g = .6 + math.sin( aaa.time.t  * 2 ) * .4
	self.ui.bu_col:set_rgba( g,g,g, 1 )
	param.set( ref.sq_active, self.b_square )
	local cov = param.get( ref.coverage )
	self:set_bu_value( "Square", cov > 0 )
end

function meu:init()
	local ref = self.ref
	local lay = self:get_layer(2)
	ref.sq_active = param.get_ref( lay, "active" )
	local bdd = self:get_layer_bdd( 3 )
	ref.coverage = param.get_ref( bdd, "out_coverage" )
end