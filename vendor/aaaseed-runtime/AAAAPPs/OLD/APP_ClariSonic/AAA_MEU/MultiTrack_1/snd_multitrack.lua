function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ix, iy

	ix = 9
	iy = 1

	local ui = self.ui

--	ui.bu_info_1 = self:add_text_info(	{1,iy,	16,1 } )
--	ui.bu_info_2 = self:add_text_info(	{1,iy+1,	16,1 } )

--	bu = self:add_trig_method(	{1, iy,	 4, 1},	"prev",		pl, "prev_track" )
--	bu = self:add_trig_method(	{1, iy+1,	 4, 1},	"next",		pl, "next_track" )
--	bu = self:add_trig_method(	{5, iy,	 4, 2},	"random",	pl, "change_track" )

	iy = 1
	ix = 4
	local SY = .6
	bu = self:add_trig_method(	{1, iy,			3,1},		"Stop",		self, "do_all_video", "set_stop" )
	bu = self:add_trig_method(	{5, iy,			3,1},		"Restart",	self, "do_all_video", "restart"  )
	iy = iy +1
		bu = self:add_button(	{1, iy,			SY,SY},		"active",	self, "active_all", false )
			bu:set_text_draw( false )
		bu = self:add_button(	{ix+SY, iy,		2-SY,SY},	"mute",		self, "mute_all", false )
			--bu:set_text_draw( false )
			bu:set_check_mul( true )
		bu = self:add_slider(	{ix+2,iy,		3.99,SY},	"Vol",		self, "vol" )
		bu = self:add_slider(	{ix+6.01,iy,	0.98,SY},	"Pan",		self, "pan", 0, -1, 1 )
			bu:set_meter( false )

	iy = iy +1
	ui.track = {}

	for i=1,self.nb_track do
		local track = {}
		bu = self:add_button(	{1, iy,			SY,SY},		"active"..i,	track, "active", false )
			bu:set_text_draw( false )
		track.bu_name = self:add_text_info(	{1.7,iy,	ix+SY-(1.7),SY*.8}, "name "..i )
		bu = self:add_button( {ix+SY, iy,		2-SY,SY},	"mute"..i,		track, "mute", false )
			bu:set_text_draw( false )
			bu:set_check_mul( true )
		bu = self:add_slider(	{ix+2,iy,		3.99,SY},	"Vol"..i,		track, "vol" )
			track.bu_vol = bu
		bu = self:add_slider(	{ix+6.01,iy,	0.98,SY},	"Pan"..i,		track, "pan", 0, -1, 1 )
			bu:set_meter( false )
		self:add_slider_two(	{ix+7,iy,		4,SY},		"Calib"..i, 	track, "min", "max", 0, 1, 0, 1 )

		ui.track[i] = track
		iy = iy + SY
	end
--	bu = self:add_button( {ix, 12 }, "Scale Z", FOX, "spe_scale_use" ) --todo careful this 0 or 1 not true or false
end

function meu:set_volume( vol )		self:set_bu_value( "vol", vol )		end
function meu:restart()				self:do_all_video( "restart" )		end
function meu:stop()					self:do_all_video( "set_stop" )		end

function meu:do_all_video( method_name )
	for i=1,self.nb_track do
		local video = self.videos[i]
		if video then
			video[method_name](video)
		end
	end
end

function meu:init()
	local ref = self.ref
	self.vol = 0

	self.nb_track = 20

	local dir = app.media_dir_rel.."Clarisonic/mp3/"
	local name = "track "
	local pre = dir..name

	local videos = {}
	for i=1,self.nb_track do
		local fname = pre..i..".mp3"
		local video = VIDEOS:get_sound( fname, true )
		if video then
			videos[i] = video
		else
			self:print(  "can't add sound file : "..fname )
		end
	end
	self.videos = videos
end

function meu:update()
	local ui = self.ui
	local videos = self.videos
	local names = { "ceci","est une",   "liste de son" }
	if videos then
		for i=1,self.nb_track do
			local t = ui.track[i]
			if t then
				t.bu_name:set_text( names[i] )
				local video = videos[i]
				if video then
					local v
					--self:print( "video "..i.." "..t.mute_all )
					if t.mute and self.mute_all then
						v = t.vol
					else
						v = 0
					end
					video:set_volume( (t.min + v * self.vol * (t.max-t.min)) )
					if t.active and self.active_all then
						video:play()
					else
						video:stop()
					end
					--video:set_pan( t.pan + self.pan )
					--table.print( t, "track "..i, 1)
				end
			end
		end
	end
end
