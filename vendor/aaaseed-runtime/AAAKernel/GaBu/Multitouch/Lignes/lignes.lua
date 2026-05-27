--DOIT
function multi_ligne_init()
	aaa.print( "Ligne multi init" )
	b_ligne_init_done = 1
	param.set( "GaBu/Multitouch/Lignes/fx_a.grid", "nb_u", pictos.nb )
	ligne_ref_layers = param.get_ref( "GaBu/Multitouch/Ligne/fx.layers_param", "active" )
	ligne_ref_p_01_u = param.get_ref( "GaBu/Multitouch/Ligne/fx_b.bdd_polygon", "point_01_u" )
	ligne_ref_p_01_v = param.get_ref( "GaBu/Multitouch/Ligne/fx_b.bdd_polygon", "point_01_v" )
	ligne_ref_p_02_u = param.get_ref( "GaBu/Multitouch/Ligne/fx_b.bdd_polygon", "point_02_u" )
	ligne_ref_p_02_v = param.get_ref( "GaBu/Multitouch/Ligne/fx_b.bdd_polygon", "point_02_v" )
	ligne_ref_p_03_u = param.get_ref( "GaBu/Multitouch/Ligne/fx_b.bdd_polygon", "point_03_u" )
	ligne_ref_p_03_v = param.get_ref( "GaBu/Multitouch/Ligne/fx_b.bdd_polygon", "point_03_v" )
end

--b_ligne_init_done = nil
if b_ligne_init_done ~= 1 then
	if pictos ~= nil then
		multi_ligne_init()
	end
end
--rects_init()
if ligne_ref_layers then
	param.set( ligne_ref_layers, b_show_lignes and 1 or 0 )
end

multi_ligne_init()