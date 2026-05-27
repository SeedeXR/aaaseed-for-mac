function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgbfa(	{9,2} )

	self:add_camera()
	self:add_mapping_by_side()
	self:add_bu_texture_target_unit()
	self:add_rendering()

	local ix = 9
	local iy = 5
	self:add_trig(	{		ix, 	iy }, 	"Restart",	ref.bdd_restart )
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	8,1},	"step" , ref.move_nb, nil, 1, 1, 256 )
		bu:set_value_type_integer( true )
	iy = iy + 1
	bu = self:add_slider(	{ix,iy,	8,1},	"Speed" , self, "speed", 1, -8, 8 )
		bu:set_meter( false )


end
function meu:init()
	local ref = self.ref
	self.t = 0
	ref.bdd = aaa.obj.get_down_by_class( ref.__layer_marked, "bdd_img_substrate" )
		ref.move_nb		=	param.get_ref( ref.bdd, "move_nb" )
		ref.bdd_restart	= 	param.get_ref( ref.bdd, "restart_trig" )
end
function meu:update()
end

function meu:draw()
	self:draw_layers_begin()
		self:draw_layers()
	self:draw_layers_end()
end
