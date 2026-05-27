
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu


	local ix = 1
	local iy = 2
	local SY = 1
	local DY = .2

	iy = iy + SY * 1.4
end


local mp3_names =
{
	DJ1		= { mp3 = "DJ1-CLEANINGSTATION_17_07_20",		id_track = 1	},
	DJ2		= { mp3 = "DJ2_BAL MANTA_20_06_29",				id_track = 2	},
	DJ3		= { mp3 = "DJ3_CHORE_whale&whale_11_07_20_V2",	id_track = 3	},
	DJ4		= { mp3 = "DJ4_CHASSE-17_07_20",				id_track = 4	},
	DN1		= { mp3 = "DN1_PONTE CORAUX_11_07_20-V2", 		id_track = 5	},
	DN2		= { mp3 = "DN2_LOOPING MANTA_29_06_20",			id_track = 6	},
	DN3		= { mp3 = "DN3-SCENE6_11_07_20V2", 				id_track = 7	},
	PIJ1	= { mp3 = "PIJ1_17_07_20",						id_track = 8	},
	PIJ2	= { mp3 = "PPIJ2_LOOPABLE_ALL_16_07_20",		id_track = 9	},
	PIJ3	= { mp3 = "PIJ3_09_07_20",						id_track = 10	},
	PIN1	= { mp3 = "PIN1_BB TURTLE",						id_track = 11	},
	PIN2	= { mp3 = "PIN2_PELAGIA",						id_track = 12	},
	PIN3	= { mp3 = "PIN3_ALL_A BOUCLER",					id_track = 13	},
	--TRANSITION = "TRANSITION 1",
}

function MONACO_AQUA:get_playlist()
--hack mini don't play anymore so remove this
--	local b_tablet = app:is_tablet()
--	if b_tablet==nil or b_tablet then return end	--	strange because deal with init order troubles
	--if app:is_pc_mas() then
		local pl = self.playlist
		if not pl then
			MEDIA.set_dir_media( "Monaco" )
			local dir		= MEDIA.get_dir().."Sounds/"
	--		local names		= { "DJ1-CLEANINGSTATION", "DJ2_BAL MANTA_20_06_29", "DJ3", "PIN3_ALL",
	--		 "DJ1-CLEANINGSTATION", "DJ2_BAL MANTA_20_06_29", "DN2_LOOPING MANTA_29_06_20", "PIJ2_05_07_20" }
			pl = PLAYLIST:create( "mp3", false )
			for _, val in PAIRS( mp3_names ) do
				local name_full = dir..val.mp3..".mp3"
				self:print( "Adding to playlist "..name_full )
				if aaa.file.is_exist( name_full ) then
					local v = pl:add_sound_file( name_full, nil, val.id_track )
				else
					self:print_error( " this file don't exist" )
				end
			end
			self.playlist = pl
		end
		local pl_trans = self.playlist_transition
		if not pl_trans then
			MEDIA.set_dir_media( "Monaco" )
			local dir		= MEDIA.get_dir().."Sounds/"
			pl_trans = PLAYLIST:create( "mp3", false )
			local transition = "TRANSITION 1"
			local name_full = dir..transition..".mp3"
			self:print( "Adding to playlist "..name_full )
			if aaa.file.is_exist( name_full ) then
				local v = pl_trans:add_sound_file( name_full, nil, 1 )
			else
				self:print_error( " this file don't exist" )
			end
			self.playlist_transition = pl_trans
		end
		return pl
	--end
end

function MONACO_AQUA:set_playlist_volume( volume )
	if app:is_pc_mas() or app:is_pc_dev() then
		if self.playlist then
			self.volume_playlist = volume
			self.playlist:set_volume( self.volume_playlist * self.volume_master )
			self.playlist:update()
		end
	end
end

function MONACO_AQUA:set_transition_volume( volume )
	if app:is_pc_mas() or app:is_pc_dev() then
		if self.playlist_transition then
			--self:print( "setting volume to "..volume )
			self.volume_transition = volume
			self.playlist_transition:set_volume( self.volume_transition * self.volume_master )
			self.playlist_transition:update()
		end
	end
end

function MONACO_AQUA:set_volume_master( value )
	self.volume_master = value
	self:set_playlist_volume( self.volume_playlist )
	self:set_transition_volume( self.volume_transition )
end
function MONACO_AQUA:mute_sound( b_mute )
	self:set_volume_master( b_mute == true and .0 or 1. )
end
function MONACO_AQUA:net_mute_sound( b_mute )
	if app:is_pc_mas() then
		self:mute_sound( b_mute )
	end
end

function MONACO_AQUA:net_shutdown_allowed( b_on )
	self.b_shutdown_allowed = b_on
end

function MONACO_AQUA:playlist_play( name )
	if app:is_pc_mas() or app:is_pc_dev() then
		if mp3_names[ name ] then
			local meu_sndplaylist = self:get_meu_by_name_no_error( "SndPlaylist_1" )
			if meu_sndplaylist then
				self:print( "must play "..name )
				local b = meu_sndplaylist:stop()
				local b = meu_sndplaylist:begin( mp3_names[ name ].id_track )
				self:print( "begin playlist return "..b )
				b = meu_sndplaylist:play( mp3_names[ name ].id_track )
				self:print( "play playlist return "..b )
			end
		else
			self:print_error( "can't find playlist item "..name )
		end
	end
end
function MONACO_AQUA:playlist_stop()
	if app:is_pc_mas() or app:is_pc_dev() then
		self.playlist:stop_track()
	end
end
function MONACO_AQUA:play_transition( b )
	if app:is_pc_mas() or app:is_pc_dev() then
		if self.playlist_transition then
			self:print( "play_transition "..b)
			if b then
			--	self:playlist_stop()
			else
			end
			self.playlist_transition:stop_track()
			self.playlist_transition:begin_track( 1 )
			self.playlist_transition:play_track( 1 )
		end
	end
end
