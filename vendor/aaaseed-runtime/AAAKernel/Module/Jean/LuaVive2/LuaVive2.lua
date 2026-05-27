CLASS.DECLARE( "VR_CTL", nil,  {
	my_var = 0,
	scale = 4
	} )

function VR_CTL:init_ref( obj )
	local ref = {}
	self.ref = ref
	ref.ctl_l = obj
	ref.tx = param.get_ref( ref.ctl_l, "translation_x" )
	ref.ty = param.get_ref( ref.ctl_l, "translation_y" )
	ref.tz = param.get_ref( ref.ctl_l, "translation_z" )
	ref.rx = param.get_ref( ref.ctl_l, "rotation_x" )
	ref.ry = param.get_ref( ref.ctl_l, "rotation_y" )
	ref.rz = param.get_ref( ref.ctl_l, "rotation_z" )
	ref.pad = param.get_ref( ref.ctl_l, "button_1" )
	ref.trig = param.get_ref( ref.ctl_l, "button_2" )
end

function VR_CTL:create( name, obj )
	local self = VR_CTL:create_instance( name )
	self:print( "create()" )
	self:init_ref( obj )
	self.pos = V3.new()
	self.rot = V3.new()
	return self
end

local pr = {y = 10}

function VR_CTL:update_pos()
	self.pos = self:get_pos()
	return self.pos
end
function VR_CTL:update_rot()
	self.rot = self:get_rot()
	return self.rot
end

function VR_CTL:get_rot()
	local ref = self.ref
	return { param.get(ref.rx), param.get(ref.ry), param.get(ref.rz) }
end

function VR_CTL:get_pos()
	local ref = self.ref
	return { param.get(ref.tx), param.get(ref.ty), param.get(ref.tz) }
end

function VR_CTL:get_trig()
	local ref = self.ref
	return param.get(ref.trig)
end

function VR_CTL:get_pad()
	local ref = self.ref
	return param.get(ref.pad)
end

function VR_CTL:move_box( box, transfo )
	self:update_pos()
	param.set(transfo, "translate_x", self.pos[1])-- * self.scale )
	param.set(transfo, "translate_y", self.pos[2])-- * self.scale )
	param.set(transfo, "translate_z", self.pos[3])-- * self.scale )
end

function VR_CTL:rotate_box( box, transfo )
	self:update_rot()
	param.set(transfo, "rotate_x", self.rot[1])
	param.set(transfo, "rotate_y", self.rot[2])
	param.set(transfo, "rotate_z", self.rot[3])
end

function pr:print(...)
	gol.color( 0.25,0.75,1, 1 )
	for i=1,select("#", ...) do
		self.y = self.y - 1
		aaa.draw_str_maa_xy( select(i, ...), 1, self.y )
	end
end

local function doit()
	aaa.lua.global.declare( 'n' )
	n = n or 0
	n = n + 1
	pr:print("n : " ..n.." t : "..aaa.time.t)
end


--aaa.obj.update_then_draw( "LuaVive2_cam" )
doit()
gol.set_depth( 1 )


if aaa.lua.global.declare("ctl_l") then
	local obj_name = aaa.dir.get_dir_tracker().."/default_b.sensor_6dof"
	local obj = aaa.obj.get_no_error( obj_name )
	if obj then
		ctl_l = VR_CTL:create( "ctl_left", obj )
	end
end
if ctl_l then
	ctl_l:print()	--- = ctl_l.print(ctl_l) -> syntaxic sugar
	if (aaa.lua.global.declare("ctl_r")) then
		local obj_name = aaa.dir.get_dir_tracker().."/default_c.sensor_6dof"
		local obj = aaa.obj.get_no_error( obj_name )
		ctl_r = VR_CTL:create( "ctl_right", obj )
	end
	ctl_r:print()

	--	local layer = aaa.obj.get_up(box, layer)
	local transfo = aaa.obj.get_by_name( "transfo1" )

	local box = aaa.obj.get_by_name( "Vive_box_ctrlr1" )
	ctl_l:move_box( box, transfo )
	ctl_l:rotate_box( box, transfo )

	transfo = aaa.obj.get_by_name( "transfo2" )
	--	local lay = aaa.obj.get_up_by_class( box, "layer" )
	--	local transfo = aaa.obj.get_down_by_class( lay, "Transfo" )

	box = aaa.obj.get_by_name( "Vive_box_ctrlr2" )
	ctl_r:move_box( box, transfo )
	ctl_r:rotate_box( box, transfo )
end

local layers = aaa.layers.get_cur()
aaa.layers.draw_begin( layers )
	aaa.layers.draw_layer_all( layers )
aaa.layers.draw_end( layers )
aaa.layers.skip_rest()