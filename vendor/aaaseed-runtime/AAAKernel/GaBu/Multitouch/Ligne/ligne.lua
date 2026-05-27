ligne_multiple_index_ref = ligne_multiple_index_ref or param.get_ref( aaa.get_multiple_cur(), "index" )
ligne_index = param.get( ligne_multiple_index_ref )
--multiple = aaa.get_multiple_cur()
--index = param.get( multiple, "index" )

--param.set( ligne_ref_layers, 0 )
--aaa.print( "Ligne index "..ligne_index )
if pictos ~= nil then
	if ligne_index < pictos.nb then
		picto = pictos[ ligne_index + 1 ]
	--		aaa.print( "Index  "..picto_index..", x = "..picto.x..", y = "..picto.y..", img nb = "..picto.img_nb )

		if picto ~= nil then
--			aaa.print( "LIndex  "..ligne_index..", x = "..picto.x..", y = "..picto.y..", par_id = "..picto.id_par )
			local parent_index = picto_find_by_id_node( picto.id_par )
		--	aaa.print( "Parent index = " .. parent_index )
			if parent_index ~= nil then
				local picto_parent = pictos[parent_index]
				if picto_parent ~= nil then
--					aaa.print( "LLIndex  "..ligne_index..", x = "..picto.x..", y = "..picto.y..", par_id = "..picto.id_par )
--					aaa.print( "LLLIndex  "..ligne_index..", x = "..picto_parent.x..", y = "..picto_parent.y..", par_id = "..picto_parent.id_par )
					param.set( ligne_ref_p_01_u, picto_parent.x )
					param.set( ligne_ref_p_01_v, picto_parent.y )
					param.set( ligne_ref_p_02_u, picto_parent.x )
					param.set( ligne_ref_p_02_v, picto.y )
					param.set( ligne_ref_p_03_u, picto.x )
					param.set( ligne_ref_p_03_v, picto.y )
				end
			end
		end
	else
		param.set( ligne_ref_layers, 0 )
	end
else
	param.set( ligne_ref_layers, 0 )
end
