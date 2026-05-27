function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local param

	ui.cam = self:add_camera()
	self:add_bu_texture_target_unit()

	local ix,iy = 1,5

	local sha = self:get_shading()
	bu = self:add_button(	{ix, iy }, "Top", aaa.layer.get_rendering( ref.layer_3d ), "top_line", true )
	bu = self:add_slider(	{ix,iy+1,	8,1},	"Inter",	sha:get_ref_vert_float( 1 ), nil, 0, 0, 1 )
	bu = self:add_slider(	{ix,iy+2,	8,1},	"Size",		sha:get_ref_vert_float( 4 ), nil, 1, 0, 1 )
end

function meu:init()
	local ref = self.ref
	self:add_shading( 2  )
	ref.layer_3d = self:get_layer(2)
	ref.bdd = aaa.layer.get_bdd( ref.layer_3d  )
	ref.min = param.get_ref_xyz_packed( ref.bdd, "bounding_box_min" )
	ref.max = param.get_ref_xyz_packed( ref.bdd, "bounding_box_max" )

	ref.transfo 	= aaa.obj.get_down_by_class( ref.obj, "transfo_trs" )
		ref.rot_y		= param.get_ref( ref.transfo, "rotate_y" )

	local min = param.get_v3_packed( ref.min )
	local max = param.get_v3_packed( ref.max )

	local d =	{	 max[1]-min[1],		 max[2]-min[2],		 max[3]-min[3]		}
	local c =	{	(max[1]+					min[1])*.5, (max[2]+min[2])*.5, (max[3]+min[3])*.5	}


	self.rot = { 0, 0 }
end

function meu:draw()
	local seq = ga:get_seq_cur()
	if not seq then return end
	local ease = seq:get_ease_in()

	local ref = self.ref
	local id = (self:get_inst_key() == "1") and 1 or 2
	local face_info = app:get_face_info( id, 1 )
	local rot = self.rot[id]
	--self:print( id.." "..face_info.rot[2] )
	--	self:print( id.." "..face_info.pre )
	rot = interpolate( rot, face_info.rot[2], aaa.time.dt * 4. )
	self.rot[id] = rot
	rot = rot * 2
	param.set( ref.rot_y, clamp( ease*rot, -.15, .15 ) )
	local inter= ease --*clamp_01( (math.abs(rot)-.05)*10 )
	self:set_bu_value( "inter", inter )
	local zone = app:get_zone(id)
	zone.inter = inter

	--self:print( id.." "..zone.presence )
	--if face_info.pre > 0 then
		self:set_bu_value( "size", zone.presence > 0 and 1 or 0 )
		MEU.draw( self )
	--end

end