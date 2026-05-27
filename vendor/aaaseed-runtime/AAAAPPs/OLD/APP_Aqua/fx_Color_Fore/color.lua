if not FX.cur then
	return
end

local fx = FX.cur

function fx:load()
	self:print_inverse( "LOAD ----------------------------------------------------" )
	MEDIA.set_dir_media( "Aqua/pal" )
	self.pal_bind = IMGS.get_bind( "v1.png" )
	self.pal_bind_alpha = IMGS.get_bind( "v1_alpha.png" )

	local ref = self.ref
	ref.part	= aaa.obj.get( "aqua_color_part_a" )
		ref.part_pal_bind	= param.get_ref( ref.part, "render_life_image_bind" )
		param.set( ref.part_pal_bind, self.pal_bind_alpha )
end
function fx:init()
	self:load()
	fx.camera_to_use = "back"
end