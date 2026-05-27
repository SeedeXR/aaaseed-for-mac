
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local ix,iy = 1, 1
	local sha = self:get_shading()

	self:add_camera()
	self:add_bu_texture_target_unit()

	iy = 5
	bu = self:add_slider_two(	{ix,iy,	16,1}, "Level"  )
		bu:set_target( sha:get_ref_frag_float(1), nil, 0, 1 )
		bu:set_target( sha:get_ref_frag_float(2), nil, 01, 2 )
--		if vmin and vmax then
--			bu:set_min_max( vmin, vmax, 1 )
--			bu:set_min_max( vmin, vmax, 2 )
--		end

	bu = self:add_slider(	{ix,iy+1,	16,1}, "grey", sha:get_ref_frag_float(3), nil, 0, 0, 1  )
end

function meu:init()
	self:add_shading()
end

function meu:update()
	local ref = self.ref
--[[
	local bu_uv = self.ui.bu_uv
	local x,y  = bu_uv:get_elt_xy(1)
	local sx,sy  = bu_uv:get_elt_sxy(1)
	--self:print( sx.." / "..sy)
	--table.print( elt, "", 3 )
	param.set( sha:get_ref_frag_vec4_x(1), x - sx * .5 )
	param.set( sha:get_ref_frag_vec4_y(1), y - sy * .5 )
	param.set( sha:get_ref_frag_vec4_z(1), 1/sx )
	param.set( sha:get_ref_frag_vec4_w(1), 1/sy )
--]]
end
