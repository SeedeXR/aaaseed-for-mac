function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	ui.cam = self:add_camera()
	self:add_bu_texture_target_unit()

	local ix,iy = 1,5

	local layer = self:get_layer(1)
	local sha = self:get_shading()
	bu = self:add_button(	{ix, iy }, "Top", aaa.layer.get_rendering( layer ), "top_line", true )
	bu = self:add_slider(	{ix,iy+1,	8,1},	"Inter",	sha:get_ref_vert_float( 1 ), nil, 0, 0, 1 )
	bu = self:add_slider(	{ix,iy+2,	8,1},	"Size",		sha:get_ref_vert_float( 4 ), nil, 1, 0, 1 )
end

function meu:init()
	local ref = self.ref
	self:add_shading()
	local layer = self:get_layer(1)
	ref.bdd = aaa.layer.get_bdd( layer )
	ref.min = param.get_ref_xyz_packed( ref.bdd, "bounding_box_min" )
	ref.max = param.get_ref_xyz_packed( ref.bdd, "bounding_box_max" )

	local min = param.get_v3_packed( ref.min )
	local max = param.get_v3_packed( ref.max )

	local d =	{	 max[1]-min[1],		 max[2]-min[2],		 max[3]-min[3]		}
	local c =	{	(max[1]+min[1])*.5, (max[2]+min[2])*.5, (max[3]+min[3])*.5	}


end

function meu:update()
	local id = (self:get_inst_key() == "1") and 1 or 2
	self:set_bu_value( "inter", math.sin( aaa.time.t * .2 + id * math.pi ) )
	--avoid solving a bug, but do we care now
	self:set_bind_2d( 386 )
end