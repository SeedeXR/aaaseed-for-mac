function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	ui.cam = self:add_camera()

	local ix,iy = 1,1
end

function meu:init()
	self.b_floating_point_install = not aaa.pc.is_maa()
	if self.b_floating_point_install then
		aaa.set_edit( false )
		aaa.flatland.set_draw( false )
	end
end

function meu:update()
	if self.b_floating_point_install then
		local count = self.count or 0
		if count < 10 then
			self.count = count + 1
			ga:set_ui_group_active( false )
		end
		if aaa.time.hour == 22 and aaa.time.minute == 30 and aaa.time.second <= 15 then
			aaa.shutdown_no_save()
		end
	end
end
