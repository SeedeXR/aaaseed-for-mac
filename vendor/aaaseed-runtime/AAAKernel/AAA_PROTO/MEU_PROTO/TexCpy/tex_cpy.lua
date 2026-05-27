function meu:define_meu_infos( )
	return { author = "Mâa",
				tags={  "2d", "Texture", "Core", "CoreGraphic", "unfinished" },
				help = "Low level copy of texture."
			}
end
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local ix,iy = 1,1
	local SY = 1
	local DY = .5
	local SDY = SY + DY
	bu = self:add_trig_method(	{	ix,	iy,			4, SY},			"Focus" ):set_function_on_click( aaa.obj.set_focus_ui, ref.bdd )
	iy = iy + SDY

	self:add_button( {ix,iy, SY,SY }, "On GPU",	ref.bdd, "grab_to_gpu" )
	iy = iy + SY
	self:add_button( {ix,iy, SY,SY }, "On CPU",	ref.bdd, "grab_to_cpu" )
	iy = iy + SDY

--	self:add_param_obj_name(	{1,2},		"grab_alpha",		ref, "grab_alpha"		)
--	self:add_param_obj_name(	{1,3},		"mipmap_generate",	ref, "mipmap_generate"	)
	self:add_param_obj_name(	{ix,iy},	"smooth",			ref.bdd, "smooth"			)

	iy = 2
	local SYM = 6
	self:add_bu_texture( { 9,iy, 8,SYM },	"Src",  1, false )
	iy = iy + SYM
	self:add_bu_texture( { 9,iy, 8,SYM },	"Dst",  2, false )
end

function meu:init()
	local ref = self.ref
	local bdd = self:get_layer_bdd(1)
	ref.bdd = bdd
	ref.bind_src = param.get_ref( bdd, "src_bind_2d" )
		param.set_save( ref.bind_src, false )
	ref.bind_dst = param.get_ref( bdd, "dst_bind_2d" )
		param.set_save( ref.bind_dst, false )
end

function meu:update()
	local ref = self.ref
	local bind
	bind = self:get_texture_bind_2d( 1 );
	param.set( ref.bind_src, bind )
	bind = self:get_texture_bind_2d( 2 );
	param.set( ref.bind_dst, bind )
end