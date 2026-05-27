--todo deal with color
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgbf(	{1,12 } )

	self:add_camera():set_preset_use( false )

	self:add_rendering()
end

function meu:init()
	local ref = self.ref
	ref.__module_main = aaa.obj.get( "cobaye_module" )
end

function meu:draw()
	local ref = self.ref
	aaa.obj.update_then_draw( ref.__module_main )
end
