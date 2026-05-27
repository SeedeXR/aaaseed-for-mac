
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

	ui.bu_info_1 = self:add_text_info(	{1,iy,	8,1},	"filename"	)
	ui.bu_info_2 = self:add_text_info(	{1,iy+1,	8,1},	"track_info" )
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

	ix, iy = 9,4
	bu = self:add_trig_method(	{ix, iy,			4, 1},	"Focus",	pl, "set_video_focus" ):set_color_back("focus")

end

function meu:get_vol_max()		return self.ui.vol_max:get_value()		end
function meu:get_vol_min()		return self.ui.vol_min:get_value()		end
function meu:set_vol_max( vol )	return self.ui.vol_max:set_value( vol )	end
function meu:set_vol_min( vol )	return self.ui.vol_min:set_value( vol )	end

function meu:begin( id_track )	return self.playlist:begin_track( id_track )	end
function meu:play( id_track )	return self.playlist:play_track( id_track )		end
function meu:stop()				return self.playlist:stop_track()				end
function meu:toggle()			return self.playlist:toggle_track()				end

function meu:init()
	local ref = self.ref
	self.vol = 0
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
										"    "..BU:format_time_duration( playlist:get_video_time(), playlist:get_video_duration() )..
										" sec") )
	self.ui.bu_info_2:set_text_color_green_info( key )

--	duration bad for mp3 (thank you microsoft)
--											.." : "..BU:format_time_duration( playlist:get_video_time_cano(), playlist:get_video_duration() )
--									)
end

-- function meu:update()
-- 	--self:print( "update()" )
-- 	--	change volume
-- 	--	local vol = self:get_vol_max()
-- 	--if vol ~= self.vol then
-- 	--	vol = self.vol + (vol - self.vol ) * aaa.time.dt * 16.
-- 	--end
-- 	--aaa.audio.set_volume( vol )
-- 	--self.vol = vol
-- end
