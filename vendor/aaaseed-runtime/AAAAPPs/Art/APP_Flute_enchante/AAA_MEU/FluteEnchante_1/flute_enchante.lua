
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	bu = self:add_button(		{1,3,	4,1	},	"capteur", 			self, "b_sensor",		false	)
	bu = self:add_text_info(	{9,3,	8,1},	"info_capteur"		)
		self.ui.bu_info_capteur = bu
	bu = self:add_slider(		{9,6,	8,1},	"Max Threshold",	self, "threshold_max", 	0,		12,1000	)

	bu = self:add_button(		{1,4	},		"Simul", 			self, "b_simul",		false 	)
	bu = self:add_slider(		{1,5,	8,1},	"presence",			self, "presence",		0,		0,1	)
	bu = self:add_slider(		{1,9,	8,1},	"story",			self, "story",			0,		0,1	)
	bu = self:add_slider(		{1,10,	8,1},	"phase",			self, "phase",			0,		0,1	)

end

function meu:init()
	local ref = self.ref
	local bdd = self:get_layer_bdd( 1 )
	ref.cov = param.get_ref( bdd, "out_coverage" )
end

function meu:update()
	self:set_ui_slot_at_start( 5 )
	if not self.meu_pip		then	self.meu_pip	= self:get_meu_by_name( "PIP_1"	)	end
	if not self.meu_stroke	then	self.meu_stroke	= self:get_meu_by_name( "Stroke_1"	)	end
end

function meu:draw()
	local ref = self.ref

	self.story 		= self.story or 0
	self.phase		= self.phase or 0
	self.dataset_id	= self.dataset_id or 1

	self:draw_layers_begin()

		local b_presence
		if self.b_sensor then
			self:draw_layer( 1 )
			self.presence = param.get( self.ref.cov )
			self.ui.bu_info_capteur:set_text( "Coverage : "..self.presence )
			b_presence = self.presence > 0
--[[
			local text = param.get( self.ref.text_out )
			local str
			b_presence = false
			for line in text:gmatch("[^\r\n]+") do
				local n = tonumber(line)
				str = str and str.." "..n or n
				self:print( n )
				b_presence = b_presence or self:test_contact( n )
			end
			if b_presence then
				str = str.."\nTouched"
			end
			self.ui.bu_info_capteur:set_text( str )
--]]
		else
			b_presence = self.b_simul
			self.presence = b_presence and 1 or 0
		end
		

		--local d = b_presence and 100 or -100
		--self.presence = clamp_01( self.presence + aaa.time.dt * d )
		self:set_bu_value( "presence", self.presence )

		if self.presence > 0 then
			self.story = self.story + self.presence * aaa.time.dt
		else
			self.story = self.story - .1 * aaa.time.dt
		end
		self.story = clamp_01( self.story )
		self:set_bu_value( "story", self.story )

		if self.story == 1. then
			if self.story_last ~= 1. then
			end
		end

		if self.story == 0. then
			if self.story_last ~= 0. then
			end
		end

		if self.presence >0 then
			self.phase = self.phase + self.presence * aaa.time.dt * .2
		else
			self.phase = self.phase - aaa.time.dt * 1/400
		end
		self.phase = clamp_01( self.phase  )
		if self.phase == 1 then
			self.phase = 0
			self.dataset_id = self.dataset_id + 1
			if self.dataset_id == 11 then
				self.dataset_id = 1
			end
			self.meu_stroke:set_bu_value( "dataset", self.dataset_id )
		end
		self:set_bu_value( "phase", self.phase )

		self.meu_stroke:set_bu_value( "phase", 0,			1 )
		self.meu_stroke:set_bu_value( "phase", self.phase,	2 )
--		self.meu_stroke:set_bu_value( "begin", 0 )
--		self.meu_stroke:set_bu_value( "end", self.phase )

		self.meu_pip:get_mu():set_value( 1 - self.story )

		self.presence_last	= self.presence
		self.story_last		= self.story
--		if self.b_draw_part then	self:draw_layer( 2 )
--									self:draw_layer( 3 )	end
	self:draw_layers_end()

--	self.ui.bu_go:print( "TEST" )
	--self:print( "pick "..self:pick_video() )
end



