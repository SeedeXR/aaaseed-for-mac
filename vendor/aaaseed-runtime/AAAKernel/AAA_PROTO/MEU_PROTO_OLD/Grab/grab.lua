function meu:define_meu_infos( )
	return { author = "Mâa",
			name_long = "Floating Point",
			tags = { "2d", "texture", "Core", "CoreGraphic", "VJ", "depreciated", "unfinished" },
			help = "Old way to do feedback (using c_bdd_feedback)\n"..
				"Feedback is what you do when you filming with a camera teh monitor where you display it\n"..
				"Of you did not too bad\n"..
				"Now you should use MEU TexCopy"

		}
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	ref = self:get_obj_down( "feedback" )

	self:add_param_obj_name(	{1,2},		"grab_alpha",		ref, "grab_alpha"		)
--	self:add_param_obj_name(	{1,3},		"mipmap_generate",	ref, "mipmap_generate"	)
	self:add_param_obj_name(	{1,4},		"smooth",			ref, "smooth"			)

	self:add_bu_texture( 		{9,3, 8,6},	"Dst", 1, false )
end

function meu:init()
	local ref = self.ref
	local bdd = self:get_layer_bdd(1)
	ref.bind_dst = param.get_ref( bdd, "bind_2d_dst" )
end

function meu:update()
	local ref = self.ref
	local bind = self:get_texture_bind_2d( 1 );
--	self:print( bind )

	param.set( ref.bind_dst, bind )
end