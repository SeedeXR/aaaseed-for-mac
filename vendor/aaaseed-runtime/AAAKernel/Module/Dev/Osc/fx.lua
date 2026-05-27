--[[
function snd.init()
	if b_snd_init == nil then
		b_on_last = false
		vol_last = 0.
		b_snd_init = true
	end
end

function snd.send_test()
	aaa.osc_send( 2, "/aaa_snd/", "hello" )
end

function snd.update()
	b_on = param.get( "Module/Dev/Osc/fx.values", "value_01" )
	if b_on ~=  b_on_last then
		if b_on > .5 then
			aaa.osc_send( 2, "/aaa_snd/sound/loop", "toto /sound hair_sample.wav" )
		else
			aaa.osc_send( 2, "/aaa_snd/sound/stop", "toto" )
		end
		b_on_last = b_on
	end
	vol = param.get( "Module/Dev/Osc/fx.values", "value_02" )
	if vol ~=  vol_last then
		aaa.osc_send( 2, "/aaa_snd/sound/vol", "toto "..vol )
		vol_last = vol
	end
	b_10 = param.get( "Module/Dev/multi_interact/fx_a.bdd_boxes", "Box10_selected" )
	if b_10 > .5 then
		aaa.osc_send( 2, "/aaa_snd/sound/loop", "tutu /sound hair_sample.wav" )
	else
		aaa.osc_send( 2, "/aaa_snd/sound/stop", "tutu" )
	end

	local x = param.get( "Module/Dev/multi_interact/fx_a.bdd_boxes", "Box10_position_x" )
	local y = param.get( "Module/Dev/multi_interact/fx_a.bdd_boxes", "Box10_position_y" )
	aaa.osc_send( 2, "/aaa_snd/sound/pos", "tutu "..x.." "..y.." 0" )
end

snd.init()
snd.update()
--]]
