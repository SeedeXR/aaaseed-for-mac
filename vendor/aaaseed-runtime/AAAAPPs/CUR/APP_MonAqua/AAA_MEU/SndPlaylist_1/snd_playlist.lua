function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ix, iy

	ix = 1
	iy = 1

	local pl
	if app.get_playlist then
		pl = app:get_playlist()
		self.playlist = pl
	else
		return
	end

	local ui = self.ui

	ui.bu_info_1 = self:add_text_info(	{1,iy,	8,1},	"Filename"		)
	ui.bu_info_2 = self:add_text_info(	{1,iy+1,	8,1},	"Track Info"	)

	iy = iy + 2.5
	bu = self:add_trig_method(	{1, iy,			4, 1},	"prev",		pl, "prev_track" )
	bu = self:add_trig_method(	{1, iy+1,		4, 1},	"next",		pl, "next_track" )
	bu = self:add_trig_method(	{5, iy,			4, 2},	"random",	pl, "change_track" )

--	iy = iy + 2.5
--	bu = self:add_slider(	{1,iy,	8,1}, "Vol_Max" )
--		ui.vol_max = bu
--	bu = self:add_slider(	{1,iy+1,	8,1}, "Vol_Min" )
--		ui.vol_min = bu

	iy = iy + 2.5
	bu = self:add_trig_method(	{ix, iy,			4, 1},	"Begin",	self, "begin" )
	bu = self:add_trig_method(	{ix, iy+1,		4, 1},	"Play",		self, "play" )
	bu = self:add_trig_method(	{ix, iy+2,		4, 1},	"Stop",		self, "stop" )
	bu = self:add_trig_method(	{ix+4, iy+1,		4, 1},	"Toggle",	self, "toggle" )

	iy = iy + 3
	bu = self:add_slider(	{1,iy,	8,1}, "Vol_Max" )
		ui.vol_max = bu
	bu = self:add_slider(	{1,iy+1,	8,1}, "Vol_Min" )
		ui.vol_min = bu
	iy = iy + 2.0
	bu = self:add_button(	{		ix, iy,				1, 1 },	"Auto Play", 		self, "b_autoplay",	false )

	ix, iy = 9,4
	bu = self:add_trig_method(	{ix, iy,			4, 1},	"Focus",	pl, "set_video_focus" ):set_color_back("focus")


end

function meu:get_vol_max()		return self.ui.vol_max:get_value()		end
function meu:get_vol_min()		return self.ui.vol_min:get_value()		end
function meu:set_vol_max( vol )	return self.ui.vol_max:set_value( vol )	end
function meu:set_vol_min( vol )	return self.ui.vol_min:set_value( vol )	end

function meu:begin( id_track )	if self.playlist then return self.playlist:begin_track( id_track )	end		end
function meu:play( id_track )	if self.playlist then return self.playlist:play_track( id_track )	end		end
function meu:stop()				if self.playlist then return self.playlist:stop_track()				end		end
function meu:toggle()			if self.playlist then return self.playlist:toggle_track()			end		end

function meu:init()
	local ref = self.ref
	self.vol = 0
end


function meu:format_time( time )
	local seconds = 0
	local minutes = 0
	local hours = 0
	local mins = 0
	if time >= 60 then
		seconds = time - math.floor( time / 60. ) * 60.
		mins = ( time - seconds ) / 60
	else
		seconds = time
	end
	if mins > 60 then
		minutes = mins - math.floor( mins / 60. ) * 60.
		hours = ( mins - minutes ) / 60
	else
		minutes = mins
	end
	local str = ""
	if hours > 0 then
		str = string.format( "%02d:%02d:%.2f", hours, minutes, seconds )
	else
		str = string.format( "%02d:%04.2f", minutes, seconds )
	end
	--	self:print( "format_time "..str)
	return str
end


function meu:update_ui()
	local playlist = self.playlist
	if not playlist then return end

	--	display track
	local	filename = playlist:get_video_name()
	if filename then
		self.filename = aaa.file.get_name_pure(filename)
	end
	local key = playlist:get_cur_key()
	self.ui.bu_info_1:set_text( self.filename )
	self.ui.bu_info_1:set_text_color_green_info( key )


	self.ui.bu_info_2:set_text( key and
									(	"Track "..playlist:get_cur_key()..
										"    "..self:format_time( playlist:get_video_time() ).."/"..self:format_time( playlist:get_video_duration() )..
										" sec") )
	self.ui.bu_info_2:set_text_color_green_info( key )

end

function meu:update()
	if self.b_autoplay then
		local playlist = self.playlist
		if not playlist then return end
		if playlist:get_video_time() >= playlist:get_video_duration() then
			playlist:prev_track()
		end
	end
end
