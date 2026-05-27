
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	--ui.cam = self:add_camera()
	self:add_trzs( {1, 1, 8, 1} )

	local ix,iy = 1,1
end

function meu:draw()
	local t = self.t or 0
	t = t + aaa.time.dt
	if t > 60 then
		t=0
	end
	self.t = t
	if t<3 then
		t = t / 3.
	elseif t < 12 then
		t = 1
	elseif t < 15 then
		t = (15-t)/3.
	else
		t = 0
	end
--	if t>0 then
		MAAEB.set_alpha( t )
		MAAEB.draw( 6, -3.2, 0, 1, false, nil, 4 )
--	end
end
--function meu:update()
--end
function OSC_MESS:do_signature( id )
	self:pop_tag()
	self:dump( "signature "..id )

	local sig = app:get_meu_by_name_no_error( "signature_1" )
	if not sig then return end

	local tag = self:get_tag()
	if tag == "position" or tag == "pos" then
		self:pop_tag()
		local what = self:get_tag()
		if 		what == "x"		then
			local x = clamp( self:get_arg(), -3.5, 3.5 )
			aaa.print( "signature x "..x )
			sig:set_bu_value( "tra x", x )
		elseif	what == "y"		then
			local y = clamp( self:get_arg(), -2, 2 )
			aaa.print( "signature y "..y )
			sig:set_bu_value( "tra y", y )
		end
	elseif tag == "size" then
		local s = clamp( self:get_arg(), 1.5, 5 )
		aaa.print( "signature size "..s )
		sig:set_bu_value( "sca", s )
	end
end