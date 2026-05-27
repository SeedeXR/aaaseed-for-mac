
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	self:add_camera()

	local ix,iy = 1,2
	local sy = 1
--[[
	ui.bu_nb = self:add_text_info(	{ix,iy,	4,sy},			"None" )

	iy = iy + 2
	self:add_button( {ix, 	iy }, 	"Draw_blob",	self, "b_draw_blob",	false )
	self:add_button( {ix+1,	iy+1 },	"phy",			self, "b_draw_phy",		false )
	self:add_button( {ix+1,	iy+2 },	"Id",			self, "b_draw_id",		false )

	ix,iy = 9,3
	self:add_slider(	{ix,iy,	8,1},	"Anticipation",			self, "anti_factor",	5, 0,10 )
	self:add_slider(	{ix,iy+1,	8,1},	"Anticipation Limit",	self, "anti_limit",		1, 0,10 )
--]]
end

function meu:init()
	local ref = self.ref
end

