local pr = {y = 0}
function pr:print(...)
	for i=1,select("#", ...) do
		self.y = self.y - 1
		aaa.draw_str_maa_xy( select(i, ...), 1, self.y)
	end
end

CLASS.DECLARE( "VR_FX", nil,  {
	threshold = 0.005
	} )

function VR_FX:init_ref( obj_name )
	local ref = {}
	self.ref = ref
--	ref.ctl = aaa.obj.get( obj_name )
--	ref.tx = param.get_ref( ref.ctl, "translation_in_x" )
end

function VR_FX:create( name, obj_name )
	local self = VR_FX:create_instance( name )
	self:init_ref( obj_name )
	self.curve = aaa.obj.get_by_name("LuaVive_curve")
	self.b_drawing = false
	self.last_pos = V3.new_from(0, 0, 0)
	self.ctl = aaa.lua.global.get( "ctl_l" )
	return self
end

function VR_FX:push_point()
	local ref = self.ref
	local ctl = self.ctl
--	if not aaa.lua.global.get( "ctl" ) then
--		return
--	end
	local scale = ctl.scale
	if (ctl:get_trig() == 1) then
		local pos = ctl:get_pos() -- (/!\ acces a l'objet VR_CTL ?)
		local d = V3.dist_squared(self.last_pos, pos)
		--    DEBUG EXAMPLES --
		--table.print( pos, "toto" )
		--table.print( self.last_pos, "last" )
		--self:print( d )
	--	pr:print( table.dump( self, "self", 4 ) )
	--	table.print( self, "self", 4 )
	--	pr:print( "dist : "..d )
		--    DEBUG EXAMPLES --
		if d > self.threshold then
	--		pr:print("last_pos x : "..self.last_pos[1].."last_pos y : "..self.last_pos[2].."last_pos z : "..self.last_pos[3])
	--		pr:print("pos x : "..pos[1].."pos y : "..pos[2].."pos z : "..pos[3])
			V3.cpy(self.last_pos, pos)
			aaa.bdd.push_control_point_back( self.curve, 1, pos[1] * scale, pos[2] * scale, pos[3] * scale )
		end
	end
end

function VR_FX:clear_points()
	local ref = self.ref
	if (self.ctl:get_pad() > 0) then
		aaa.bdd.clear_control_points( self.curve, 1 )
	end
end

aaa.obj.update_then_draw("LuaVive2_cam")
gol.color( 0.25,0.75,1, 1)
gol.set_depth( 1 )
local layers = aaa.layers.get_cur()
aaa.layers.draw_begin( layers )
aaa.layers.draw_layer_all( layers )

if (aaa.lua.global.declare("fx")) then
	local obj = aaa.obj.get_by_name( aaa.dir.get_dir_tracker().."/default_b.sensor_6dof" )
	fx = VR_FX:create( "fx_1", obj )
end

fx:push_point()
fx:clear_points()

aaa.layers.draw_end( layers )
aaa.layers.skip_rest()
