function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ix, iy

	ix = 1
	iy = 1

	local pl = 	self:get_playlist()

	local ui = self.ui

	ui.bu_info_1 = self:add_text_info(	{1,iy,	16,1},	"filename"	)
	ui.bu_info_2 = self:add_text_info(	{1,iy+1,	16,1},	"track_info" )

	iy = iy + 2.5
	bu = self:add_trig_method(	{1, iy,	 4, 1},	"prev",		pl, "prev_track" )
	bu = self:add_trig_method(	{1, iy+1,	 4, 1},	"next",		pl, "next_track" )
	bu = self:add_trig_method(	{5, iy,	 4, 2},	"random",	pl, "change_track" )

	iy = iy + 2.5
	bu = self:add_slider(	{1,iy,	8,1}, "Vol_Max" )
		ui.vol_max = bu
	bu = self:add_slider(	{1,iy+1,	8,1}, "Vol_Min" )
		ui.vol_min = bu


--	bu = self:add_button( {ix, 12 }, "Scale Z", FOX, "spe_scale_use" ) --todo careful this 0 or 1 not true or false
end

function meu:get_playlist()
	local playlist = self.playlist
	if not playlist then
		playlist = PLAYLIST:create( "qwartz_move", true )
		playlist:add_sound_in_dir( app.media_dir_rel.."Qwartz/Move/Playlist/" )
		self.playlist = playlist
	end
	return playlist
end

function meu:get_vol_max()		return self.ui.vol_max:get_value()		end
function meu:get_vol_min()		return self.ui.vol_min:get_value()		end
function meu:set_vol_max( vol )	return self.ui.vol_max:set_value( vol )	end
function meu:set_vol_min( vol )	return self.ui.vol_min:set_value( vol )	end

function meu:init()
	local ref = self.ref
	self.vol = 0
end

function meu:update()
	local playlist = self.playlist
	playlist:update()
	--	change volume
	local vol
	if app.meu_kinect:is_skeleton() then
		vol = self:get_vol_max()
	else
		vol = self:get_vol_min()
	end
	if vol ~= self.vol then
		vol = self.vol + (vol - self.vol ) * aaa.time.dt
	end
	aaa.audio.set_volume( vol )
	self.vol = vol
	--	display track
	local	filename = playlist:get_video_name()
	if filename then
		filename = aaa.file.get_name_pure(filename)
	end
	self.ui.bu_info_1:set_text( filename )
	self.ui.bu_info_2:set_text( "Track "..playlist:get_cur_key().."    "..BU:format_number_in_text( playlist:get_video_time() ).." sec" )
end
