
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = .9
	local SX = 1
	local DY = .2

	self:add_camera()
	self:add_rendering(		{nil,nil,		8,2*SY })

--	local sha = self:get_shading()
	bu = self:add_trig_method(	{ix,iy, 		4,SY},	"Restart",	self, "restart"	)
	iy = iy + SY
	bu = self:add_button(		{ix,iy, 		4,SY},	"Play",		self, "b_play", 	false	)
	bu = self:add_button(		{ix+4,iy, 		4,SY},	"Wrap",		self, "b_wrap", 	false	)
	iy = iy + SY
	bu = self:add_slider(		{ix,iy, 		8,SY},	"Duration",	self, "duration", 	4,	0, 180	)
	iy = iy + SY
	bu = self:add_slider(		{ix,iy, 		8,SY},	"Phase",	self, "phase", 		0,	0,1)
	iy = iy + SY
	
	iy = iy + SY + DY

	bu = self:add_slider(		{ix,iy, 		8,SY},	"Size",		self, "size", 		1,	0,16)
end

function meu:draw_icon()
	gol.push_matrix()
		gol.translate_x( -.35 )
		gol.scale_x( .25 )
		local turn = wrap_01(aaa.time.t * .25)
		aaa.draw_disk_arc_axe_z(	-0,0,0, 1, 0.25, -turn, 16*turn )
	gol.pop_matrix()
end

function meu:restart()
	self.phase = 0
end


function meu:init()
	local ref = self.ref

	self.phase = 0 
--	ref.layer_attr		= self:get_layer(1)
end

-- function meu:update_ui()
-- 	local ui = self.ui
-- 	local ref = self.ref
-- end

function meu:update()
	local dur = self.duration
	if dur ~= 0 then
		local phase = self.phase
		if self.b_play then
			phase = phase + aaa.time.dt / dur
		end
		if self.b_wrap then
			phase = wrap_01(phase)
		end
		self.phase = phase
	end
end

function meu:draw()
	local ref = self.ref

	self:draw_layers_begin()
		self:draw_layer(1)
		local turn = clamp_01(self.phase)
		aaa.draw_disk_arc_axe_z(	0,0,0,	self.size, 0.75, -turn, 4*turn+1 )
	self:draw_layers_end()
end

