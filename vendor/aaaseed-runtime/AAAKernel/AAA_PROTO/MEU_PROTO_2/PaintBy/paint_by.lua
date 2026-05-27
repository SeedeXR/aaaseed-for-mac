function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	ref.pbn_color_bind		= param.get_ref( ref.multiple, "image_color_bind" )
	ref.pbn_gradient_bind	= param.get_ref( ref.multiple, "image_gradient_bind" )
	ref.image_scale_u		= param.get_ref( ref.multiple, "image_scale_u" )
	ref.image_scale_v		= param.get_ref( ref.multiple, "image_scale_v" )

	ref.part			= aaa.obj.get_branch_by_class( ref.__layer_marked, "bdd_particle" )
		ref.part_box_size_u		= param.get_ref( ref.part, "emission_box_size_u" )
		ref.part_box_size_v		= param.get_ref( ref.part, "emission_box_size_v" )

	self:add_camera()
	self:add_bu_texture_target_unit(  {1,1, 4,4} )
	self:add_param(	{9,3,	 4,1},	"brush_u",		param.get_ref( ref.multiple, "size_u" ),		0, 1 )
	self:add_param(	{13,3,	 4,1},	"brush_v",		param.get_ref( ref.multiple, "size_v" ),		0, 1 )
	self:add_param(	{9,4,	 8,1},	"brush_size",	param.get_ref( ref.multiple, "size_factor" ),	0, 4 )

	self.bank_2d = 4	--hack
	self.bind_2d = 17

	self:add_bu_texture_target_unit( {1,5, 4,4}, "TO DO : FIX", 2 )

end

function meu:update()
	local ref = self.ref
	if not ref.multiple then return end
	if self.bank_2d then
		local bind = aaa.img.make_bind_2d( self.bank_2d, self.bind_2d )

		--aaa.print( bind )
		param.set( ref.pbn_color_bind, bind )
		param.set( ref.pbn_gradient_bind, bind )

		local sx,sy = aaa.img.get_size( bind )
		if sx then
			local r = sy/sx
			local s = 4
			param.set( ref.size_u, s )
			param.set( ref.size_v, s*r )

			param.set( ref.part_box_size_u, s*2 )
			param.set( ref.part_box_size_v, s*r*2 )

			param.set( ref.image_scale_u, s*2 )
			param.set( ref.image_scale_v, r*s*2 )
		end
	end
end

--[[
if not AAApbn then
	AAApbn = {}
	AAApbn.ref = {}
	--local obj = aaa.obj.get( "Module/Aaart/Painting_By_Number_v0/fx.layers_param" )
	AAApbn.ref.cpu = param.get_ref( "Module/Aaart/Painting_By_Number_v0/fx.fbo", "tex_1_on_cpu" )
	AAApbn.ref.nb_u = param.get_ref( "Module/Aaart/Painting_By_Number_v0/fx_b.grid", "nb_u" )
	AAApbn.ref.nb_v = param.get_ref( "Module/Aaart/Painting_By_Number_v0/fx_b.grid", "nb_v" )
	AAApbn.cpu_trick = 0
end

function print_pbn()
	local bind = param.get( "Module/Aaart/Painting_By_Number_v0/fx.fbo", "tex_1_bind_2d_asked" )
	local obj = aaa.obj.get_from_top_by_class( "print_master" )
	param.set( obj, "from_texture_bind", bind )
	param.set( obj, "print_trig", 1 )
	AAApbn.cpu_trick = 2
	aaa.mess.show( "Printed\nThanks to Maa\nsignature to add" )
end

--aaa.print( AAApbn.cpu_trick )
param.set( AAApbn.ref.cpu, AAApbn.cpu_trick  )
if AAApbn.cpu_trick > 0 then
	param.set( AAApbn.ref.nb_u, 200  )
	param.set( AAApbn.ref.nb_v, 200  )
else
	param.set( AAApbn.ref.nb_u, 100  )
	param.set( AAApbn.ref.nb_v, 100  )
end

AAApbn.cpu_trick = AAApbn.cpu_trick - 1
AAApbn.cpu_trick = math.max( AAApbn.cpu_trick, 0 )
--]]

