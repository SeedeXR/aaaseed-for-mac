function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local SY = 1
	local DY = .2
	local ix,iy = 1,1
	
	self:add_rendering()
	self:add_camera()

	self:add_trig_fn(	{ix,iy,		3,SY},	"Focus",	aaa.obj.set_focus_ui, self:get_layer_bdd(1) )
	iy = iy + SY
	self:add_slider( 	{ix,iy,		8,SY},	"Offset", 	ref.multiple,	"offset_axe",	0, -1,1 )
end

function meu:init()
	local ref = self.ref

	local layer = self:get_layer(1)

	ref.multiple				= aaa.obj.get_branch_by_class( layer, "multiple" )
	-- ref.multiple_size_u			= param.get_ref( ref.multiple, "size_u" )
	-- ref.multiple_size_v			= param.get_ref( ref.multiple, "size_v" )
	-- ref.multiple_size_axe		= param.get_ref( ref.multiple, "size_axe" )
	-- ref.multiple_size_factor	= param.get_ref( ref.multiple, "size_factor" )
end