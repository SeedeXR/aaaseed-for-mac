function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par

	ui.cam = self:add_camera()

	local ix,iy = 1,1

	bu = self:add_button(	{ix, iy }, "Kinect Auto Switch", self, "b_kauto", true )

	iy = iy +2
	self.fx_nb		= 16
	self.fx_id 		= 1
	self.fx_id_last = self.fx_nb
	bu = self:add_selector(	{ix+1,iy,	3,6},	"FX" )	--notext not save in preset
		bu:set_nb( 2, 8 )
		ui.bu_fx = bu
		bu:set_text_draw( false )

		--	now we can put the hook
		bu:set_hook_on_value_change( function( bu, balue ) self:set_fx_low( balue:get_value() ) end )
end

function meu:init_spe()
	local tmeu = {}
	self.tmeu = tmeu

	tmeu.kinect		= self:get_meu_by_name( "Kinect_2" )
	tmeu.displace	= self:get_meu_by_name( "Displace_1" )
	tmeu.opencv		= self:get_meu_by_name( "OpenCV_1" )
	tmeu.fp1		= self:get_meu_by_name( "FP4_1" )
	tmeu.fp2		= self:get_meu_by_name( "FP4_2" )
end

--[[
function meu:update_ui()
	local ui = self.ui
	local bu = ui.fx

	--aaa.debug.show( bu:get_value(), "fx" )
	--aaa.debug.show( ui.cam:get_value(), "cam" )
	local v = bu:get_value() + 1
	if self.__fx_id_last ~= v then
		self.__fx_id_last = v
		self:set_fx_low( v )
	end
end
--]]
function meu:init()

	local b, e = string.find( aaa.net.machine, "PAR4KEET" )
	if b and e then
		self.b_hippo_machine = true
		aaa.force_window( 32, 32, 320, 190 )
		aaa.push_window()
		aaa.push_console()
	elseif not aaa.pc.is_maa() then
		aaa.set_edit( false )
		ga:set_ui_group_active( false )
		aaa.flatland.set_draw( false )
	end
	--aaa.box_error( "machine "..aaa.net.machine.." "..b.." "..e )
end

function meu:get_fx()
	return self.fx_id
end
function meu:set_fx( v )
	aaa.debug.show( v )
	self:set_bu_value( "fx", v-1 )
end

function meu:set_fx_no()
	local tmeu = self.tmeu
	if tmeu then
		tmeu.displace	:set_mu_value( 0 )
		tmeu.opencv		:set_mu_value( 0 )
		tmeu.fp1		:set_mu_value( 0 )
		tmeu.fp2		:set_mu_value( 0 )
	end
end
function meu:set_fx_low( fx )
	if self.fx_id == fx then return end
	self.fx_id = fx
	self:print( "sex_fx( "..fx.." )" )

	self:set_fx_no()

	local tmeu = self.tmeu
	fx = fx -1
	local sub = math.floor( fx / 4 )
	local rest = fx % 4

	if tmeu then
		if rest==0 or rest==1 then
			local off = rest + sub*2
			tmeu.displace:set_mu_value( 1 )
			tmeu.displace:change_preset( off+1 )
		elseif rest==2 then
			tmeu.opencv	:set_mu_value( 1 )
			tmeu.fp1	:set_mu_value( 1 )
		elseif rest==3 then
			tmeu.opencv	:set_mu_value( 1 )
			tmeu.fp2	:set_mu_value( 1 )
		end
	end
end

function meu:inc_fx( fx, inc )
	fx = fx + inc
	if 		fx < 1			then fx = self.fx_nb
	elseif	fx > self.fx_nb	then fx = 1
	end
	return fx
end

function meu:update()
	if aaa.time.hour == 19 and aaa.time.minute == 50 and aaa.time.second <= 15 then
		if not aaa.pc.is_maa() then
			aaa.shutdown_no_save()
		end
	end
	if self.b_hippo_machine then
		if not self.hippo_machine_init_count then
			aaa.force_window( 32, 32, 320, 190 )
			aaa.set_edit( false )
			ga:set_ui_group_active( false )
			aaa.flatland.set_draw( false )
			self.hippo_machine_init_count = 1
			--kinect		= self:get_meu_by_name( "Kinect_2" )
		elseif self.hippo_machine_init_count == 1 then
			aaa.force_window()
			self.hippo_machine_init_count = 2
		end
	end

	local cnt = self.count_spe or 1
	if cnt < 20 then
		cnt = cnt + 1
		if cnt == 15 then
			self:init_spe()
		end
		self.count_spe = cnt
		return
	end

	--local ui = self.ui
	--local bu = ui.bu_fx
	--local tmeu = self.tmeu
	--aaa.debug.show( tmeu[1], 1  )
	--aaa.debug.show( tmeu[2], 2  )

	local fx = self.fx_id
	local mk = self.tmeu.kinect
	--aaa.debug.show( mk, "mk" )
	aaa.debug.show( self.b_kauto, "Kinect auto" )
	if (not self.b_kauto) and mk and mk.hand_le then
		if mk.hand_le.b_trig then fx = self:inc_fx( fx, -1 )	end
		if mk.hand_ri.b_trig then fx = self:inc_fx( fx, 1 )		end
	end
	self:set_fx_low( fx )
end


function OSC_MESS:do_ghippo_logo( id )
	self:pop_tag()
	self:dump( "ghippo logo "..id )

	local pip = self:get_meu_by_name_no_error( "pip_1" )
	if not pip then return end

	local tag = self:get_tag()
	if tag == "position" or tag == "pos" then
		self:pop_tag()
		local what = self:get_tag()
		if 		what == "x"		then
			local x = clamp( self:get_arg(), -3.5, 3.5 )
			aaa.print( "ghippo logo x "..x)
			pip:set_bu_value( "tra x", x )
		elseif	what == "y"		then
			local y = clamp( self:get_arg(), -2, 2 )
			aaa.print( "ghippo logo y "..y )
			pip:set_bu_value( "tra y", y )
		end
	elseif tag == "size" then
		local s = clamp( self:get_arg(), .5, 2 )
		aaa.print( "ghippo logo size "..s )
		pip:set_bu_value( "sfactor", s )
	end
end
function OSC_MESS:do_ghippo()
	self:pop_tag()
	self:dump( "ghippo" )
	local tag = self:get_tag()
	if tag == "fx" then
		self:pop_tag()
		local what = self:get_tag()
		if 		what == "change"	then
			aaa.print( "ghippo fx change"..self:get_arg() )
			local hippo = app:get_meu_by_name_no_error( "GreenHippo_1" )
			if not hippo then return end
			hippo:set_fx( self:get_arg() )
		end
	elseif tag == "logo1" or tag == "logo" then
		self:do_ghippo_logo( 1 )
	end
end


--aaa.net.osc_send( 2, "/AAASeed/begin",					aaa.time.t 			)
--aaa.net.osc_send( 2, "/AAASeed/test",						3, 2, "zob", 1, 0	)
--aaa.net.osc_send( 2, "/AAASeed/time",						aaa.time.t  		)
--aaa.net.osc_send( 2, "/AAASeed/resolution/x",				1920*2				)
--aaa.net.osc_send( 2, "/AAASeed/resolution/y",				1080*2			)
--aaa.net.osc_send( 2, "/AAASeed/signature/pos/x",			1.8					)
--aaa.net.osc_send( 2, "/AAASeed/signature/pos/y",			1.62				)
--aaa.net.osc_send( 2, "/AAASeed/signature/size",			2					)
--aaa.net.osc_send( 2, "/AAASeed/ghippo/fx/change",			6					)
--aaa.net.osc_send( 2, "/AAASeed/ghippo/logo1/pos/x",		-2.5				)
--aaa.net.osc_send( 2, "/AAASeed/ghippo/logo1/pos/y",		1.5				)
--aaa.net.osc_send( 2, "/AAASeed/ghippo/logo1/size",		0				)
--aaa.net.osc_send( 2, "/AAASeed/gui/show",					1					)
--aaa.net.osc_send( 2, "/AAASeed/kinect/gesture/enable",	0					)
--aaa.net.osc_send( 2, "/AAASeed/kinect/close",				0.89				)
--aaa.net.osc_send( 2, "/AAASeed/kinect/skew/vertical",		0.					)
--aaa.net.osc_send( 2, "/AAASeed/process",					1					)
--aaa.net.osc_send( 2, "/AAASeed/exit_no_save"									)
--aaa.net.osc_send( 2, "/AAASeed/edit",						0					)
--aaa.net.osc_send( 2, "/AAASeed/end",						aaa.time.t 			)
--aaa.net.osc_send( 2, "/AAASeed/endb", aaa.time.t )
--aaa.net.osc_send( 2, "/AAASeed/save"	)

--aaa.net.osc_test( "/AAASeed/" )
--OSC_MESS.take_and_dump( "/AAASeed/" )
--aaa.sleep(2000)

local function do_osc( m )
	m:pop_tag()
	local tag = m:get_tag()
	if tag == "begin"				then	aaa.print(	"BEGIN" )
	elseif tag == "end" 			then	aaa.print(	"END" )
	elseif tag == "ghippo"			then	m:do_ghippo(		)
	else
		m:do_aaaseed()
	end
end

OSC_MESS:process_messages( "/AAASeed/", do_osc )
