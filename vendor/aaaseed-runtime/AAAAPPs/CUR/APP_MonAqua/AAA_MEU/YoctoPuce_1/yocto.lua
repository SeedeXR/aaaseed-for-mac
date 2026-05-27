
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu

	--self:add_rgbf(	{1,4.5 } )

	--self:add_bu_texture_target_unit()

	self:add_camera()
	--self:add_rendering()

	local ix = 1
	local iy = 1
	local SY = .9
	local DY = .2


	iy = 3
	bu = self:add_button(	{ix, iy,		SY, SY },	"Active", 	ref.active, nil, true )
	iy = iy + SY
	bu = self:add_button(	{ix, iy,		SY, SY },	"Open", 	ref.open, nil, false )
	iy = iy + SY
	bu = self:add_button(	{ix, iy,		SY, SY },	"Verbose", 	ref.verbose, nil, false )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	3,SY}, 	"Polling Interval Size", 	ref.polling_interval,nil,		1,		0.1, 8 )
	iy = iy + SY
	bu = self:add_button(	{ix, iy,		SY, SY }, 	"Signal Inverted", 	self, "b_inverted",		false )

	iy = iy + SY
	ui.bu_input = self:add_text_info(	{ix,iy,	8,SY},	"Input :"				)

	--local i = 1;

end

function meu:init()
	local ref = self.ref

	self.time = aaa.time.t
	self.last_input = -1
	--todomonaq use BDD_FBX as for non matte stuff
	local yocto = self:get_layer_bdd( 1 )
	ref.yocto = yocto

	ref.active				= param.get_ref( yocto, "active" )
	ref.open				= param.get_ref( yocto, "open" )
	ref.verbose				= param.get_ref( yocto, "verbose" )
	ref.polling_interval	= param.get_ref( yocto, "polling_interval")
	ref.input				= param.get_ref( yocto, "input" )
end

function meu:send_command()
end

function meu:update()
	local ui = self.ui
	local ref = self.ref
	--self.last_input = -1
	local input = param.get( ref.input )
	local b_active = param.get( ref.active )
	local b_open = param.get( ref.open )
	--self:print( "Input is "..input )

	if b_active then
		local time = aaa.time.t

		if time - self.time > 10.0 or input ~= self.last_input then
			-- refresh state
			local value = 1
			if self.b_inverted then
				value = 0
			end
			local b_white_screen = input ~= value
			if b_open == 0 then
				b_white_screen = false
			end
			self:print( "Yocto Puce : sending network command "..( b_white_screen and "white screens" or "projection") )
			app:send_method_verbose( "set_screens_white", b_white_screen and "true" or "false" )
			local obj = aaa.obj.get_from_top_by_class( "tex_video_master" )
			local r = param.get_ref_no_error( obj, "audio_allow" )
			if r then
				param.set( r, not b_white_screen )
			end
			ui.bu_input:set_text( "Input : "..( b_white_screen and "No Signal" or "Signal Received" ) )
			self.time = time
		end
		self.last_input = input
	else
		self.last_input = -1
	end
end

function meu:draw()
 	self:draw_layers_begin()
 		self:draw_layer( 1 )
	self:draw_layers_end()
end




