-- this is old code probably epidemik

if aaa.lua.global.declare( "snd" ) then
	local function snd_init()
		if snd == nil then
			aaa.print( "snd.init()" )
			snd = {}
			snd.playing = 0
	--		if app and app.rio then
	--			snd.vol = -30
	--		else
	--			snd.vol = -12
	--		end
			-- vulcania
			snd.vol = -30
			snd.vol_haut = -20
			snd.vol_bas = -30
			snd.mike_on = false
			snd.mike_vol = -12
			--for OSC unused
			--	b_on_last = false
			-- vol_last = -20
			snd.kling_osc_tree = snd.kling_osc_tree or "/aaa/kling/"
			snd.osc_tree = snd.osc_tree or (snd.kling_osc_tree.."sound/")
	--		if c5 and c5.pc.cam then		-- sound control is on
	--			snd.vc_init()
	--			snd.set_master_vol( snd.vol )
	--		end
		end
	end

	snd_init()
end

--/aaa/kling/voix/diffusion 0.4 1 1 0.4 0 0 0 0 0 0 0 0 0 0 0 0
--end
function snd.send_diffusion()
	snd.send_osc( snd.kling_osc_tree.."voix/diffusion", "0.4 1 1 0.4 0 0 0 0 0 0 0 0 0 0 0 0" )
--	snd.send_osc( snd.kling_osc_tree.."voix/diffusion", "0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0" )
end

function snd.callback()
	snd.send_osc( snd.kling_osc_tree.."callback/connect", "192.168.1.66 76610" )
end
--snd.callback()
--snd.send_diffusion()
function snd.send_osc_test()
	aaa.net.osc_send( 2, "/aaa_snd/", "hello" )
end
function snd.send_osc( tree, str )
--	aaa.print( "osc_send( "..tree.." , "..str )
	aaa.net.osc_send( 2, tree, str )
end
function snd.send( command, str )
	--aaa.print( "send" )
	--[[local i = math.random(64)	local bn = "Box"..make_nb_char2(box)
	local i = 8
	if i == 1 or i == 2 then
		if i == 1 then
			snd.send_osc( 2, "/jhgjhgjhg/jkhjh/"..command, str )
		else
			snd.send_osc( 2, snd.osc_tree..command, str )
		end
	else
		snd.send_osc( snd.osc_tree..command, str )
	end
	--]]
	snd.send_osc( snd.osc_tree..command, str )
end
function snd.quit()
	snd.send_osc( snd.kling_osc_tree.."quit", " " )
end

function snd.stop_all()
	snd.send_osc( snd.osc_tree.."stopall", "" )
end
--snd.stop_all()
function snd.dsp_on()
	snd.dsp( 1 )
end
function snd.dsp_off()
	snd.dsp( 0 )
end
function snd.dsp( on )
-- /aaa/kling/dsp 0 ou 1
	snd.send_osc( snd.kling_osc_tree.."dsp", tostring( on ) )
end

function snd.spat_amb()
	--amb, 1 1 1 1 1 1 1 1 1 1 1 1;
	--/aaa/kling/spatconfig/add conf1 0 0 1 0 0 0 0 0 0 0 0 1
	--snd.send_osc( snd.kling_osc_tree.."add", "amb 1 1 1 1 1 1 1 1 1 1 1 1" )
	--/aaa/kling/spatconfig/add conf1 0 0 1 0 0 0 0 0 0 0 0 1
	snd.send_osc( snd.kling_osc_tree.."spatconfig/add", "ambiance 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1" )
end
--snd.spat_amb()
function snd.voices( nb )
	--/aaa/kling/voices $1
	snd.send_osc( snd.kling_osc_tree.."voices", tostring(nb) )
end
--snd.voices( 16 )
function snd.speaker_clear()
--	/aaa/kling/speaker/clear
	snd.send_osc( snd.kling_osc_tree.."speaker/clear", "" )
end

function snd.speaker( nb, x, y, z, sx, sy, shape )
-- OSC-route /on /pos /size /shape /text /color /clear
-- shape : 0 - gaussian
--			1 - lineaire
--			2 - exponentiel 1
--			3 - exponentiel 2
--			4 - seuil
	local str = tostring(nb).." "
	snd.send_osc( snd.kling_osc_tree.."speaker/on", str.."1" )
	snd.send_osc( snd.kling_osc_tree.."speaker/pos", str..tonumber(x).." "..tonumber(y).." "..tonumber(z) )
	snd.send_osc( snd.kling_osc_tree.."speaker/size", str..tonumber(sx).." "..tonumber(sy) )
	snd.send_osc( snd.kling_osc_tree.."speaker/shape", str..tonumber(shape) )
	snd.send_osc( snd.kling_osc_tree.."speaker/text", str.."hp_"..tostring(nb) )
end
	--snd.send_osc( snd.kling_osc_tree.."speaker/on", tonumber(11).." 1" )
--	snd.send_osc( snd.kling_osc_tree.."speaker/pos", tonumber(2).." "..tonumber(0.24).." "..tonumber(0.1).." 0.0" )

local speaker_cos_35 = math.cos( 35 ) * .5
local speaker_sin_35 = math.sin( 35 ) * .5

local speaker_vc ={	{ x = 0.0, y = 0.5 },
				{ x = -speaker_cos_35, y = speaker_sin_35 },
				{ x = -0.5, y = 0.0 },
				{ x = -speaker_cos_35, y = -speaker_sin_35 },
				{ x = 0.0, y = -0.5 },
				{ x = speaker_cos_35, y = -speaker_sin_35 },
				{ x = 0.5, y = 0.0 },
				{ x = speaker_cos_35, y = speaker_sin_35 },
				size = 0.5,
				shape = 0,
				radius = 1.0,
				nb = 8,
				z = 5.0
			}
			--snd.speaker_clear()
--snd.vc_init()
function snd.vc_init()
	snd.voices( speaker_vc.nb * 2 )		-- voice number is twice speaker nb
	snd.speaker_clear()
--	snd.spat_amb()
	for i = 1,speaker_vc.nb do
		snd.speaker( i, speaker_vc[ i ].x * speaker_vc.radius, speaker_vc[ i ].y * speaker_vc.radius, speaker_vc.z, speaker_vc.size, speaker_vc.size, speaker_vc.shape )
	end
--	snd.dsp_on()
	snd.set_master_vol( snd.vol )
	-- start nappe
	--snd.nappe_start()
end

function snd.spat( name, config )
	--amb, 1 1 1 1 1 1 1 1 1 1 1 1;
	--/aaa/kling/spatconfig/add conf1 0 0 1 0 0 0 0 0 0 0 0 1
	snd.send_osc( snd.kling_osc_tree.."add", "name".."config" )
end

function snd.set_master_vol( vol )
	if vol ~= nil then
		snd.send_osc( snd.kling_osc_tree.."mastervoldB", tostring(vol) )
		--aaa.print( "mastervol "..tostring( vol ) )
	end
end

--for i = -80, -50 do
--snd.set_master_vol( i )
--end
--snd.set_master_vol( -80 )

function snd.set_mike( b_in )
	if b_in ~= nil then
		if b_in then
			snd.send_osc( snd.kling_osc_tree.."inputs/1/on", 1.0 )
		else
			snd.send_osc( snd.kling_osc_tree.."inputs/1/on", 0.0 )
--			snd.send_osc( snd.kling_osc_tree.."mastervoldB", vol )

		end
	end
end
--snd.set_mike( false )

function snd.mike_vol( vol )
	aaa.print( "snd.mike_vol" )
	if vol ~= nil then
		snd.send_osc( snd.kling_osc_tree.."inputs/1/voldB", vol )
		aaa.print( "inputs/1/voldB "..tostring( vol ) )
	end
end
--snd.mike_vol( 0 )

--PLAY /aaa/kling/voix/play <H/F> /sound <soudfilepath> /voldB <voldB>
function snd.vox_wav_id( snd_id, path, vol, filter )
	if vol == nil then
		vol = 0.0
	end

--	aaa.print( "sound filter "..tostring( filter ) )
	if filter == nil then
		filter = 'H'
	end
	if filter == 'H' then
		vol = vol + 4.0
	end
--	snd.kling_osc_tree = snd.kling_osc_tree or "/aaa/kling/"
	snd.osc_vox_tree = (snd.kling_osc_tree.."voix/")
--	snd.send_osc( snd.osc_vox_tree.."stop", filter )
	--snd.send_diffusion()
	snd.send_osc( snd.osc_vox_tree.."play", filter.." /sound "..path.."/"..snd_id..".wav /voldB "..vol )
--	snd.send_osc( snd.osc_vox_tree.."play", filter.." /sound "..path.."/"..snd_id..".aiff" ) -- /voldB "..vol )
	aaa.print( snd.osc_vox_tree.."play "..filter.." /sound "..path.."/"..snd_id..".wav /voldB "..vol )

end

function snd.spat_wav_id( snd_id, path, spat_type, vol )
	if vol == nil then
		vol = 0.0
	end
	snd.send( "play", "id_fre_"..snd_id.." /sound "..path.."/"..snd_id..".wav /voldB "..vol.." /spat fix "..spat_type )

end

function snd.amb_wav_id( id, snd_id, x, path, vol )
	if vol == nil then
		vol = 0.0
	end
	snd.send( "play", "id_fre_"..snd_id.." /sound "..path.."/"..snd_id..".wav /voldB "..vol.." /spat fix ambiance" )
end

function snd.play_pos_id( snd_id, path, vol, xg, yg, xr, yr )
	if vol == nil then
		vol = 0.0
	end
	snd.send( "play", "id_fre_"..snd_id.." /sound "..path.."/"..snd_id..".wav /voldB "..vol.." /pos "..xg.." "..yg.." "..xr.." "..yr )
end

function snd.play_pos( snd_name, vol, xg, yg, xr, yr )
	if vol == nil then
		vol = 0.0
	end
	snd.send( "play", "id_fre_"..snd_name.." /sound "..snd_name.." /voldB "..vol.." /spat pos "..xg.." "..yg.." "..xr.." "..yr )
end
function snd.loop_amb( snd_name, vol ) --, xg, yg, xr, yr )
	if vol == nil then
		vol = 0.0
	end
--	/spat fix spatconfigname
	snd.send( "loop", "nappe".." /sound "..snd_name.." /voldB "..vol.." /spat fix amb" )
end

function snd.play_wav_id( id, snd_id, x, path, vol )
	if vol == nil then
		vol = 0.0
	end
	snd.send( "play", "id_fre_"..snd_id.." /sound "..path.."/"..snd_id..".wav /voldB "..vol.." /pos "..x.." 4. 0." )
end

--snd.vol = 0.5
function snd.play_aif_id( id, snd_id, x, path, vol )
	if vol == nil then
	vol = 0.0
	end
	snd.send( "play", "id_fre_"..snd_id.." /sound "..path.."/"..snd_id..".aif /voldB "..vol.." /pos "..x.." 4. 0." )
	if vol ~= nil then
		snd.send( "vol", "id_fre_"..snd_id.." "..vol )
	end
end--aaa.print( "in sound ")

function snd.stop_id( snd_id, duration )
	if duration == nil then duration = 0.0 end
	if snd_id ~= nil then
		snd.send( "fadeout", "id_fre_"..snd_id.." "..duration	)
	end
end

function snd.do_it()
--	snd_init()
end

snd.do_it()



