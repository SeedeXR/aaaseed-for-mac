
function meu:define_seq_ui( bus )
	self.bu_seq = self:add_bu_sequence( bus, ga:get_seqs() )
end

function meu:set_vol( vol )
	aaa.audio.set_volume( vol )
end
function meu:change_lang( bu )
	app:change_lang_by_id( clamp( bu:get_value(), 1, 4 ) )
end
function meu:define_ui()
	local bu
	local par

	self.__b_need_seqs_define_ui = true

	local ix, iy = 9, 1
	local dx = 2.5
	bu = self:add_button( {ix+dx,	iy }, "Simul", app, "b_simul", false )
		bu:set_text_rect_ratio( 3 )
	bu = self:add_slider(	{ix,iy,	dx,1}, "A " )
	bu = self:add_slider(	{ix+8-dx,iy,	dx,1}, "B " )

	iy = iy + 1

	local function add_slider_out( ix, iy, name )
		local bu = self:add_slider(	{ix,iy,	2,2}, name )
		bu:set_ui_active( false )
		return bu
	end
	bu = add_slider_out( ix, iy, "A" )
	bu = add_slider_out( ix+6, iy, "B" )
	bu = add_slider_out( ix+3, iy, "AB" )
	iy = iy + 3
	bu = self:add_slider(	{ix,iy,	4,1}, 	"Time Up",		app, "time_delay_up", 	2, 1, 10  )
	bu = self:add_slider(	{ix,iy+1,	4,1}, 	"Time Down",	app, "time_delay_down",	4, 1, 10  )
	iy = iy + 2

	iy = 8
	bu = self:add_selector(	{ix,iy,	8,1.6},	"Langue" )	--notext not save in preset
		bu:set_nb( 2, 2 )
		bu:set_item_text( 1, "Francais", "Anglais", "Espagnol", "Signes" )
		bu:set_method_on_value_change( self, "change_lang", bu )

	iy = iy + 2
	bu = self:add_trig_method(	{ix, iy,			4, 1}, "Normal",		self, "set_vol", 1	)
	bu = self:add_trig_method(	{ix+4, iy,		4, 1}, "Low",		self, "set_vol", .5	)
	iy = iy + 1
 	bu = self:add_slider(	{ix,iy,	8,1}, "Volume", 	self, "volume_ui",	1, 0, 1 )
	bu = self:add_slider(	{ix,iy+1,	8,1}, "Music",  	self, "vol_mus_ui",	1, 0, 1 )
	bu = self:add_slider(	{ix,iy+2,	8,1}, "Voix",  		self, "vol_voi_ui",	1, 0, 1 )
end

function meu:update_ui()
	local t = { "a", "b" }
	for i=1,2 do
		local key = t[i]
		local zone = app:get_zone(i)
		self:set_bu_value( key, zone.presence )
		key = key.." "
		if app.b_simul then
			zone.presence_raw = self:get_bu_value( key )
		else
			self:set_bu_value( key,	zone.presence_raw )
		end
	end
	self:set_bu_value( "ab",	app.presence_global )
end

function meu:update()
	--	this way the seqs can be initialized after the GP
	local seqs = ga:get_seqs()
	self:check_need_define_seq_ui( seqs )

	app:update_sound( self.volume_ui, self.vol_mus_ui, self.vol_voi_ui  )


	local pre = app:update_presence()
	--self:print( pre )
	if seqs:is_run() then
		local seq_id = seqs:get_seq_cur_id()

		local m = app.meu_lang_sign

		if app.seq_id_last ~= seq_id then
			app.seq_id_last = seq_id
			if app.b_lang_sign then
				local t_be = { nil, 10, 30, 	60,  nil, 	110, 	150 	}
				local be = t_be[seq_id]
				if be then
					m:set_time( be )
					m:stop()
					self.b_lang_sign_need_play = true
					app.b_lang_sign_draw = false
				end
			end
		else
			if app.b_lang_sign then
				local t_seq = seqs:get_seq_cur():get_time()
				if t_seq < 1. then
				elseif  t_seq < 2. then
					if self.b_lang_sign_need_play then
						self.b_lang_sign_need_play = false
						m:start()
						app.b_lang_sign_draw = false
					end
				else
					app.b_lang_sign_draw = true
					local t_en = { nil, 27, 52.6, 	nil, 105.9, 146.4,	162.8	}
					local en = t_en[seq_id]
					if en then
						en = en + 1.
						local t = m:get_time()
						if t > en then
							m:stop()
							app.b_lang_sign_draw = false
						end
					end
				end
			else
				m:stop()
			end
	end

		if seq_id == 1 then
			if pre == 1 then
				seqs:inc_seq()
			end
		else
			if seq_id ~= 7 then
				if pre == 0 then
					seqs:restart(1)
				end
			end
		end
	end
end
