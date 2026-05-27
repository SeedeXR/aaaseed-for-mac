

function meu.pong( ip, b_answer )
    local meu_remote = "RemoteKinect_1"
	if b_answer then
		aaa.print( "Got ping from ip : "..ip )
		ip = aaa.net.ip[1]
		aaa.print( "Sending pong with our ip : "..ip )
		aaa.net.lua_send( 8, 1, "app:get_meu_by_name( \""..meu_remote.."\" ).pong( \""..ip.."\" )" )
	else
		aaa.print( "Got pong from ip : "..ip )
	end
end

function meu:ping()
    local ip = aaa.net.ip[1]
    local meu_remote = self:get_name()
    self:print( "Sending ping to "..meu_remote.." with our ip : "..ip )
	aaa.net.lua_send( 8, 1, "app:get_meu_by_name( \""..meu_remote.."\" ).pong( \""..ip.."\", \""..true.."\" ) " )
end


function meu:define_ui()
	local ix, iy = 1.5, 1.5
	local SX, SY = 2, 1
	local ui = self.ui
	local bu

	self:add_trig_method(	{ix, iy,	 SX, SY}, "Ping", self, "ping" )

	--ui.bu_cover = self:add_text_info(	{1,1,	8,1}, "Cover" )
end

function meu:init()
	--self.ip = aaa.net.ip[1]
end
function meu:update()
end
