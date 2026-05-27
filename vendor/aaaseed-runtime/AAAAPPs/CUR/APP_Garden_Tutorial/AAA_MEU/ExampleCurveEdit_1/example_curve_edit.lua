
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	local ix, iy = 1,1
	local SY = .8
	local DY = .2

	self:add_camera()
	self:add_rendering()
	iy = iy + 1	
	local y_begin = iy + DY

--------------------
	self:set_tab_key( "Main" )
	ix, iy = 1, y_begin

	bu = self:add_trig_method(	{ix,iy,		4,SY},		"Test 1", 		self,		"do_test", 1	 )
	bu = self:add_trig_method(	{ix+4,iy,	4,SY},		"Test 2", 		self,		"do_test", 3	 )
	iy = iy + SY + DY


	self:add_bu_texture_target_unit( {ix,iy, 8,6} )
	
end

function meu:init()
end

function meu:do_test( s_how )
end