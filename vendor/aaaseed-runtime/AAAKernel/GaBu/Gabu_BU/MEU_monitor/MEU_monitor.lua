
--	MONITOR
--
--todo remove this : pre BU_MONITOR probably
--[[
--todo this in pip and spip who have even more commun code
--todo	not in pop anymore
function MEU:update_ui_tex()

	--bu:set_xy( .3, .2 )
	if self.dim_next ~= tar.dim then
		self.dim_next = tar.dim
		self.b_target_changed = true
	end

	if tar.dim == 2 then
		local bind = aaa.img.make_bind_2d( tar.bank_2d, tar.bind_2d )
		--if not aaa.img.exist( bind ) then
		--	aaa.img.move_to_gpu( bind )
		--end
		local sx, sy = aaa.img.get_size( bind )
		if not sx then
			aaa.img.move_to_gpu( bind )
		end
		if self.bind_next ~= bind then
			local sx, sy
			--if aaa.img.exist( bind ) then
				sx, sy = aaa.img.get_size( bind )
			--end
			if sx and sx > 0 then
				if self.time_left==nil then
					self.bind_next = bind
					self.b_target_changed = true
				end
			end
			--aaa.print( bind.." : "..sx.." x "..sy )
			--todo deal with monitor / dimension here too ?
		end
--		else
--			if self.dim_next ~= 0 then
--				self.dim_next = 0
--				self.b_target_changed = true
--			end
	end
end
--]]


function MEU:__get_monitor_pos_def( rect )
	rect = rect or {}
	return {rect[1] or 9,rect[2] or 3.55, rect[3] or 8,rect[4] or 8*9/16}
end

MEU.doc.__add_monitor_helper = "( rect, name, id, bind ) called only by MEU:add_monitor_id() and MEU:add_monitor() (2024 September), add a fix monitor"
function MEU:__add_monitor_helper( rect, name, id, bind )
	local b_fix = true
	name = name or "Monitor"

	local irect = self:__get_monitor_pos_def( rect )
	rect = self:make_rect_from_irect( irect )

	--aaa.print_inverse( "________________________________________________"..bind )
	bind = bind or self:get_texture_bind_2d( id )
	if bind == nil then
		self:print_error( "In MEU:__add_monitor_helper() we have no bind" )
	end

	--self:print( "add monitor fix with name "..name.." and bind "..bind )
	local bu_mon = bus_cur:add_monitor( name, rect, bind, b_fix )
		--todo change for example pass id only when needed
		--bu_mon:set_text( name )
		--todoqq do that cleaned
		if id then
			bu_mon:__delegate_get_set_bind_2d( self, "get_texture_bind_2d", "set_texture_bind_2d", id )
		end
		--self.ui.bu_tex = bu
		bu_mon:set_ui_active( true )

	--bu = self:add_text_info(	{x + nx*.5,y,	.8,.8}, "Monitor_Info" )
	return bu_mon
end

MEU.doc.add_monitor_id = "( rect, name, id )"
function MEU:add_monitor_id( rect, name, id )
	return self:__add_monitor_helper( rect, name, id or 1,	nil )
end
MEU.doc.add_monitor = "( rect, name, bind )"
function MEU:add_monitor( rect, name, bind )
	return self:__add_monitor_helper( rect, name, nil,		bind )
end
