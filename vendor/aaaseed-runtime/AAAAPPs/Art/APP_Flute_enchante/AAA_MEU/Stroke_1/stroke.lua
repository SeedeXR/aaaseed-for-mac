function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ui = self.ui

	self:add_camera()

	local ix, iy = 1,1
	local sy = 1
	bu = self:add_trig_method(	{ix,iy,			4, sy},		"Focus",		self, "set_focus_bdd" )

	iy = iy + sy
	self.speed = 1.
	bu = self:add_slider(		{ix+1,iy,		7,sy}, 		"speed",		self, "speed" )

	iy = iy + sy
	bu = self:add_button(		{ix,iy,			1, sy },	"Record",		self.ref.record_asked, nil,		0	)
	bu = self:add_button(		{ix,iy+sy,		1, sy },	"Intercept",	self.ref.ui_intercept, nil,		0	)
	bu = self:add_trig_restart(	{ix+4,iy, 		3, sy }		)

	iy = iy + sy * 2
	bu = self:add_button(		{ix,iy,			1, sy },	"Crosshair",	self.ref.draw_crosshair, nil,	0	)
	bu = self:add_button(		{ix,iy+sy,		1, sy },	"Rectangle",	self.ref.draw_rectangle, nil,	0	)

	iy = iy + sy * 3
	bu = self:add_slider(		{ix,iy,			8,sy}, 		"Dataset",		self.ref.dataset_id, nil,		1,	1,256	)

	iy = iy + sy
	bu = self:add_button(		{ix,iy,			1, sy },	"Use Phase",	self.ref.draw_method, nil,		0,	0, 1	)
	bu = self:add_slider_two(	{ix,iy+sy,		8,sy},		"Phase",		nil, self.ref.phase_begin, self.ref.phase_end,		0,1 )
	bu = self:add_text_info(	{ix,iy+sy*2,	8,sy},		"Phase Info" )
		--bu:set_text_color( {1,0,0,1} )
		--bu:set_visible( false )
		ui.bu_info_phase = bu

	iy = iy + sy * 4
end

function meu:init()
	local ref = self.ref
	local bdd = self:get_layer_bdd( 1 )
	ref.dataset_id		= param.get_ref( bdd, "dataset_id"			)
	ref.phase_begin 	= param.get_ref( bdd, "draw_phase_begin"	)
	ref.phase_end		= param.get_ref( bdd, "draw_phase_end"		)
	ref.stroke_begin 	= param.get_ref( bdd, "draw_stroke_begin"	)
	ref.stroke_end		= param.get_ref( bdd, "draw_stroke_end"		)
	ref.stroke_nb		= param.get_ref( bdd, "stroke_nb"			)
	ref.record_asked	= param.get_ref( bdd, "record_asked"		)
	ref.ui_intercept	= param.get_ref( bdd, "ui_intercept"		)
	ref.draw_method		= param.get_ref( bdd, "draw_method"			)
	ref.draw_crosshair	= param.get_ref( bdd, "draw_crosshair"		)
	ref.draw_rectangle	= param.get_ref( bdd, "draw_rectangle"		)
end

function meu:set_focus_bdd()
	local bdd = self:get_layer_bdd( 1 )
	aaa.obj.set_focus_ui( bdd )
end

function meu:restart()
end

function meu:update_ui()
	local ui = self.ui
	local ref = self.ref
	local str = string.format( "[% 6d,% 6d[ on % 6d", param.get(ref.stroke_begin), param.get(ref.stroke_end), param.get(ref.stroke_nb) )
	ui.bu_info_phase:set_text( str )
end

function meu:draw()
	MEU.draw( self )
end
