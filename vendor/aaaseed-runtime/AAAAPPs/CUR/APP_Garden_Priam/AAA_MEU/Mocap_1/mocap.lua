function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()
end

function meu:update()
	--local m = MEU.get_inst_last_by_type( "Kinect" )
	--self:print( "found "..m )
end

function meu:get_bdd_mocap( id )
	return self:get_layer_bdd( id+1 )
end