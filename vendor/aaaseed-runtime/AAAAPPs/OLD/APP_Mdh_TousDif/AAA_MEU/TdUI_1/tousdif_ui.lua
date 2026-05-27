
function meu:define_seq_ui( bus )
	self.bu_seq = self:add_bu_sequence( bus, ga:get_seqs() )
end

function meu:define_ui()
	local bu
	local par

	self.__b_need_seqs_define_ui = true

	local ix, iy = 9, 1
	local sy = 2

	ix, iy = 9, 1

	bu = self:add_button( {ix+2.5,iy }, "simul", app, "b_simul", false )
	bu = self:add_button( {ix,iy }, 	"Simul A", self, "a_pre" ):set_text("A")
		bu:set_text_rect_ratio( 2 )
	bu = self:add_button( {ix+6,iy }, 	"Sumil B", self, "b_pre" ):set_text("B")
		bu:set_text_rect_ratio( 2 )
	bu = self:add_slider(	{ix,iy+1,	4,1}, "A Con", self, "a_con" )
	bu = self:add_slider(	{ix+4,iy+1,	4,1}, "B Con", self, "b_con" )
	bu = self:add_slider(	{ix+2,iy+2,	4,1}, "Conformity", TD_PLAYER, "conformity" )

	iy = 4
	self.ui.bu_info = self:add_text_info(	{ix,iy,	4,1 }, "Info" )
	bu = self:add_trig_method(	{ix+4, iy,		2, 1}, "-", app, "inc_level_raw", -1 )
	bu = self:add_trig_method(	{ix+6, iy,		2, 1}, "+", app, "inc_level_raw", 1 )
	iy = iy + 1
	local sx = 8/3
	bu = self:add_slider(	{ix,iy,			sx,1}, 	"Game",		app, "duration_game", 		10, 10, 20  )
	bu = self:add_slider(	{ix+sx,iy,		sx,1}, 	"Result",	app, "duration_result",		2, 2, 5  )
	bu = self:add_slider(	{ix+sx*2,iy,	sx,1}, 	"Photo",	app, "duration_photo",		2, 2, 5  )

	ix, iy = 9, 5
	local sy = 2
	iy = iy + 2
	bu = self:add_slider(	{ix+2,iy,	2,2}, "Presence Raw A" ):set_text("A")
	bu = self:add_slider(	{ix+4,iy,	2,2}, "Presence Raw B" ):set_text("b")
	iy = iy + sy
	bu = self:add_slider(	{ix,iy,		4,1}, 	"Time Up",		app, "time_delay_up", 	2, 1, 10  )
	bu = self:add_slider(	{ix,iy+1,	4,1}, 	"Time Down",	app, "time_delay_down",	4, 1, 10  )
	iy = iy + 2
	bu = self:add_slider(	{ix,iy,		2,2}, "A" )
	bu = self:add_slider(	{ix+6,iy,	2,2}, "B" )
	bu = self:add_slider(	{ix+3,iy,	2,2}, "AB" )
end

function meu:update_ui()
	local player
	player = app:get_player(1)
	self:set_bu_value( "a",					player.presence )
	self:set_bu_value( "presence raw A",	player.presence_raw )
	player = app:get_player(2)
	self:set_bu_value( "b",					player.presence )
	self:set_bu_value( "presence raw A",	player.presence_raw )
	self:set_bu_value( "ab",				app.presence_global )

--	self.ui.bu_info:set_text( "Level "..app:get_level() )
end

function meu:update()
	--	this way the seqs can be initialized after the GP
	local seqs = ga:get_seqs()
	self:check_need_define_seq_ui( seqs )

	if app.b_simul then
		local player
		player = app:get_player(1)
		player.presence_raw		= self.a_pre
		player.conformity		= self.a_con
		player = app:get_player(2)
		player.presence_raw		= self.b_pre
		player.conformity		= self.b_con
	else
		for i=1,2 do
			local player = app:get_player(i)
			player.presence_raw	= app.skel_used[i]
		end
	end

	if seqs:is_run() then
		local seq_id = seqs:get_seq_cur_id()
		if seq_id == 4 then	-- video go to the end, no cut in explanation
		else
			local pre = app:update_presence()
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

end
