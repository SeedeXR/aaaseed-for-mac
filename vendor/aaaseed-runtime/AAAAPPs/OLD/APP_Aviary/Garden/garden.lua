APP.DECLARE( "AVIARY", APP_GP )

local L_APP = AVIARY
local SUPER = APP_GP

function L_APP:init_app( ... )
	self:set_init_monitor_m_nb( 0 )
	return oo.getsuper(L_APP).init_app( self, ... )
end
function L_APP:do_key( key )
	if key==48 then --0
		self:set_fx( 3 )
		return true
	elseif 49<=key and key<=58 then --right
		key = key-49
		local r = key%2
		key = (key-r) / 2
		key = key*4 + r + 1
		self:set_fx( key )
		return true
	end
	return oo.getsuper(L_APP).do_key( self, key )
end
function L_APP:do_key_special( key )
	self:print( key )
	if key==274 or key==273 then --left
		self:inc_fx( -1 )
		return true
	elseif key==275 or key==272 then --right
		self:inc_fx( 1 )
		return true
	end
	return oo.getsuper(L_APP).do_key_special( self, key )
end
function L_APP:get_hippo()
	return self:get_meu_by_name_no_error( "GreenHippo_1" )
end
function L_APP:set_fx( what )
	aaa.print( "ghippo fx change"..what )
	local hippo = self:get_hippo()
	if not hippo then return end

	hippo:set_fx( what )
end
function L_APP:inc_fx( inc )
	local hippo = self:get_hippo()
	if not hippo then return end

	self:set_fx( hippo:inc_fx( hippo:get_fx(), inc ) )
end


if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

