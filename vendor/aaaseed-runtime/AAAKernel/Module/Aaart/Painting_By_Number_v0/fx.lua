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

