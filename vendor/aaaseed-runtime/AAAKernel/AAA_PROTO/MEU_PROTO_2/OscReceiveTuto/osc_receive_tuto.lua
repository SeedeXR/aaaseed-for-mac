function meu:define_meu_infos( )
	return { author = "Mâa", date="2025 November",
			tags = { "core", "input", "interoperability", "Tutorial", "device" },
			help = { "Print and read custom simple OSC block to set slider value",
					 " (2025 November)"
					}
		}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = 1
	local DY = .2

	local SXS = 4	
	local SYS = SY * 2
	local bu

	local osc = self.osc

	--self:add_camera()
	--self:add_rendering()
	--self:add_transfo()

	bu = self:add_text(     {ix,iy,			16,SY}, "message root" )
		ui.bu_osc_root = bu
	iy = iy + SY
	self:add_button(     	{ix,iy,			4,SY},	"Verbose", 		self,	"b_verbose",		false	)
	self:add_button(     	{ix+4,iy,		4,SY},	"Verbose Tag", 	self,	"b_verbose_tag",	false	)
	iy = iy + SY + DY

	self.tags = {}
	self.command_nb = 8
	for i = 1,self.command_nb do
		local t = {}
		bu = self:add_button(     	{ix,iy,			2,SY}, "Active "..i, t,"b_active", false  ):set_text( tostring(i))
		bu = self:add_text(     	{ix+2,iy,		6,SY}, "Name "..i )
			t.bu_name = bu
		bu = self:add_slider(     	{ix+8,iy,		8,SY}, "Value "..i, t,"value", 0,	-8,8 )
		iy = iy + SY
		self.tags[i] = t
	end

end

function meu:init()
	self.osc = OSC:create( "OSC_"..self:get_name() )
end

function meu:update()
	local osc = self.osc
	local ui = self.ui

	local mess_root = ui.bu_osc_root:get_value()
	
	local signature = "Tuto Osc"
	local function process_one( m )
		if self.b_verbose then
			m:dump( signature )
		end
		m:pop_tag()

		local tag = m:pop_tag()
		local value = m:pop_arg()

		if self.b_verbose_tag then
			self:print( "tag "..tag.." with value "..value )
		end

		for i = 1,self.command_nb do
			local t = self.tags[i]
			if t.b_active then
				local name = t.bu_name:get_text()
				if name == tag then
					t.value = value
					break
				end
			end
		end

	end

	OSC_MESS:process_messages( mess_root, process_one )

end


-- function meu:draw()
-- 	local ndim_frame = self.osc:get_ndim_done()
	
-- 	self:draw_layers_begin()
-- 		self:draw_layer( 1 )	--set attr

-- 	self:draw_layers_end()
-- end
