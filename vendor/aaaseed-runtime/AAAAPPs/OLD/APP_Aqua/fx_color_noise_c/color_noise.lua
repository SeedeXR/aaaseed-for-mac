local fx = FX.cur
if not fx then
	return
end


function fx:load()
	--self:box_debug( "fx:load()" )

	if self.verbose > 0 then
		self:print_inverse( "LOAD ----------------------------------------------------" )
	end
	MEDIA.set_dir_media( "Aqua/pal" )
	self.pal_bind = IMGS.get_bind( "v1.png" )

	local ref = self.ref
	aaa.layer.set_bind_2d( ref.layer_a, self.pal_bind )
	--	ref.part	= aaa.obj.get( "aqua_color_part_a" )
--		ref.part_pal_bind	= param.get_ref( ref.part, "render_life_image_bind" )
--		param.set( ref.part_pal_bind, self.pal_bind_alpha )
end

function fx:init()
	self:load()
	self.camera_to_use = "back"

	local ref = self.ref
	ref.shading	= aaa.layer.create_shading( ref.layer_a, self:get_name() )
end

function fx:add_slid( param_ref, name, min, max  )
	local bu = bus_cur:add_slider( name )
	bu:set_target_param( param_ref )
	bu:set_min_max( min or 0., max or 1. )
	return bu
end

function fx:add_ui_window()
	FX.add_ui_window( self, false )

	local ref = self.ref
	local sha = ref.shading
	self:add_slid( sha:get_ref_frag_float(1), "grey" )

	bus_cur:move_next()
	self:add_slid( sha:get_ref_vert_vec4_x( 1 ), "sx", .01, 5. )
	self:add_slid( sha:get_ref_vert_vec4_y( 1 ), "sy", .01, 5. )
	self:add_slid( sha:get_ref_vert_vec4_z( 1 ), "sz", .01, 5. )

	bus_cur:move_next()
	self:add_slid( sha:get_ref_vert_vec4_x( 2 ), "ox", -10., 10. )
	self:add_slid( sha:get_ref_vert_vec4_y( 2 ), "oy", -10., 10. )
--	self:add_slid( sha:get_ref_vert_vec4_z( 2 ), "oz", -10., 10. )

	bus_cur:move_next()
	self:add_slid( sha:get_ref_frag_vec4_x( 1 ), "ax", .0, 1. )
	self:add_slid( sha:get_ref_frag_vec4_y( 1 ), "ay", .0, 1. )
	self:add_slid( sha:get_ref_frag_vec4_z( 1 ), "bx", .0, 1. )
	self:add_slid( sha:get_ref_frag_vec4_w( 1 ), "by", .0, 1. )


	bus_cur:move_next_col()
	self:add_slid( sha:get_ref_vert_float(1), "alpha_center",	-3.,	3. )
	self:add_slid( sha:get_ref_vert_float(2), "alpha_range",	0.,		6. )

	bus_cur:move_next()
	self:add_slid( sha:get_ref_vert_vec4_x( 3 ), "a_sx", .01, 5. )
	self:add_slid( sha:get_ref_vert_vec4_y( 3 ), "a_sy", .01, 5. )
	self:add_slid( sha:get_ref_vert_vec4_z( 3 ), "a_sz", .01, 5. )

	bus_cur:move_next()
	self:add_slid( sha:get_ref_vert_vec4_x( 4 ), "a_ox", -10., 10. )
	self:add_slid( sha:get_ref_vert_vec4_y( 4 ), "a_oy", -10., 10. )
--	self:add_slid( sha:get_ref_vert_vec4_z( 4 ), "a_oz", -10., 10. )

end