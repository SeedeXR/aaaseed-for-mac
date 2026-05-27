function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local ix,iy = 1,1
	local SY = 1
	local DY = .2
	local bu

	self:add_trig_method(	{ix,iy,		2,SY},	"Dump", self, "dump" )

	ix,iy = 1,3
	local S = 16 
	local DS = S*1/8
	local SX = 1
	bu = self:add_slider_multi_curve(	{ix,iy+DS,	S,8},	"Tuto", 3 )
		ui.bu_multi = bu
	self:add_trig_method(	{ix,iy,		SX,DS},		"+", bu, "add_elt_ui", 1  ):set_text_inside( true )
	self:add_trig_method(	{ix+SX,iy,	SX,DS},		"-", bu, "add_elt_ui", -1 ):set_text_inside( true )
end

-- get_curve_value
function meu:dump()
	local bu = self.ui.bu_multi
	-- bu:get_curve_value() would work too
	local nb = bu:get_elt_nb()
	for i=1,nb do
		local elt = bu:get_elt(i)
		local x,y = elt:get_xy()
		self:print( i.." -> "..x..", "..y )
		elt:set_text_inside( true )
	end
end

--[[
function meu:init_update_ui()
	if not self.b_first then
		local bu = self.ui.bu_multi
		local nb = bu:get_elt_nb()
		for i=1,nb do
			local elt = bu:get_elt(i)
			local sx,sy = elt:get_sxy()
			elt:set_sxy( sx*2, sy*2 )
		end
		self.b_first = true
	end
end
--]]