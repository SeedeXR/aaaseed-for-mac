
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	self.time = 0.0
	self.b_play = false
	local ix = 1
	local iy = 1
	local SY = 1
	local DY = SY*.2

	iy = self:define_time(	{ix,iy,	1,SY}, 103.33, false )
end

local names = { "Fbx_MANTA_KL_1", "Fbx_MANTA_KL_2", "Fbx_MANTA_KC_1", "Fbx_MANTA_KC_2", "Fbx_MANTA_SO" }


function meu:update_ui()
	self:update_time_ui()
end

function meu:update()
	local t = self:update_time()

	if t then
		for i,name in IPAIRS( names ) do
			local m = self:get_meu_by_name_no_error( name )
			if m then
				m:set_time(t)
				m:set_play(false)
			end
		end
	end

end

function meu:draw()
end
