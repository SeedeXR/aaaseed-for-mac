function meu:define_ui()
	local ref = self.ref
	local bu

	--Simple slider
	bu = self:add_slider(	{1,11.5,	4,1},	"Dplane 5_1")
	bu:set_dplane(5)
	bu = self:add_slider(	{1,7,	4,1},	"Dplane 0_1")
	bu = self:add_slider(	{1,8,	4,1},	"Dplane -1_1")
	bu:set_dplane(-1)
	bu = self:add_slider(	{1,9,	4,1},	"Dplane -2_1")
	bu:set_dplane(-2)
	bu = self:add_slider(	{1,8.5,	4,1},	"Dplane 1_2")
	bu:set_dplane(1)
	bu = self:add_slider(	{1,9.5,	4,1},	"Dplane 1_3")
	bu:set_dplane(1)
	bu = self:add_slider(	{1,11,	4,1},	"Dplane 2_2")
	bu:set_dplane(2)
	bu = self:add_slider(	{1,12,	4,1},	"Dplane 0_2")
	bu = self:add_slider(	{1,12.7,	4,1},	"Dplane 0_3")



	bu = self:add_slider(	{5,3,	4,1},	"Dplane 1_4")
	bu:set_dplane(1)
	bu = self:add_slider(	{5,3.6,	4,1},	"Dplane 1_5")
	bu:set_dplane(1)
	bu = self:add_slider(	{5,4.2,	4,1},	"Dplane 1_6")
	bu:set_dplane(1)
	bu = self:add_slider(	{5,4.8,	4,1},	"Dplane 1_7")
	bu:set_dplane(1)
	bu = self:add_slider(	{5,5.4,	4,1},	"Dplane 1_8")
	bu:set_dplane(1)
	bu = self:add_slider(	{5,6,	4,1},	"Dplane 1_9")
	bu:set_dplane(1)
end
