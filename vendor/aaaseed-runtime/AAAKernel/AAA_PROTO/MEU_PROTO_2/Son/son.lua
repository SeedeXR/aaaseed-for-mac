function meu:define_meu_infos( )
	return { 	name_long = "Sound visualisation",
				author = "Mâa",
				tags = { "3d", "Vj", "Core", "CoreGraphic", "Draw", "procedural", "Sound","unfinished" },
				help =	{	"represent the Sound input",
							"using c_bdd_sound so have access to Sound wave and spectrum, midi, csv file",
							"It have a lot of problems and the sound input function only in certain case",
							"Unusable for Now (2024 Sep Mâa)"
						}
			}
end

local 	b_meu_sound_input_dialog = false

function meu:define_ui()
	local ref = self.ref

	self:add_rgb(	{9,3 } )
	self:add_camera( nil, 16 )
	self:add_mapping_and_blending()
	self:add_rendering( {1,9} )
	self:add_bu_texture_target_unit()
	self:add_mapping_hexa()

	local ix = 9
	local iy = 2.2

	-- snd_input is attached
	-- local input = aaa.obj.get_down_by_class_no_error( self:__get_obj_main(), "snd_input" )
	local input = aaa.obj.get_by_class_no_error( self:__get_obj_main(), "snd_input" )
	if not input then
		--self:box_error( "No sound input.\nif hardware available change pref at start and restart." )
		local str = "Can't access sound input attached.\n\tMaa have to solve this."
		if b_meu_sound_input_dialog then
			self:box_error( str )
			b_meu_sound_input_dialog = false
		else
			self:print_error(str)
		end
	else
--		self:add_button_param( ix,		iy,		"Calib_auto",	param.get_ref( input, "calibration_auto"  ) )
--		self:add_button_param( ix+1,	iy+1,	"Left",			param.get_ref( input, "calibration_left"  ) )
--		self:add_button_param( ix+4,	iy+1,	"Right",		param.get_ref( input, "calibration_right" ) )
	end
	ref.snd_input = input

	iy = iy + 3.2
	local SY = .8
	local bdd = self:get_layer_bdd( 1 )
	local function add_buts( iy, name, what, min, max )
		local bu
		bu = self:add_param(	{ix,iy,	 		8,SY},	name.."u",		param.get_ref( bdd, what.."u"	),	min, max )
		bu = self:add_param(	{ix,iy+SY,	 	8,SY},	name.."v",		param.get_ref( bdd, what.."v"	),	min, max )
		bu = self:add_param(	{ix,iy+2*SY,	8,SY},	name.."axe",	param.get_ref( bdd, what.."axe"	),	min, max )
			bu:set_meter( true )
		return iy + 3*SY + .2
	end

	iy = add_buts( iy, "offset_",	"offset_",		0,	8	)
	iy = add_buts( iy, "size_",		"size_",		0,	1	)
	iy = add_buts( iy, "dir_",		"direction_",	-4,	4	)

end
