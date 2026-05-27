--UTIL
aaa.show_file_begin( "aaa_layer" )

if not aaa.layer then
	aaa.layer = {}
	aaa.print_error(  "aaa.layer not defined by AAASeed." )
end
aaa.layer.doc = {}	-- .doc not defined in C (yet)
--todo add the get_always fns
--todo deal with this case in doc code or change convention
aaa.layer.doc.doc = "regroup method to ease acces to layer content"


function aaa.layer.get_rendering(		layer_ref )		return aaa.obj.get_branch_by_class( layer_ref, "rendering" )		end
function aaa.layer.get_mapping(			layer_ref )		return aaa.obj.get_branch_by_class( layer_ref, "mapping" )			end
function aaa.layer.get_model(			layer_ref )		return aaa.obj.get_branch_by_class( layer_ref, "model" )			end
function aaa.layer.get_model_no_error(	layer_ref )		return aaa.obj.get_branch_by_class_no_error( layer_ref, "model" )	end

aaa.layer.doc.get_color = "( layer_ref ) return a ref on c_color"
function aaa.layer.get_color(			layer_ref )
	--layer_ref = layer_ref or aaa.layer.get_cur()	--2024 September Maa remove to test
	return aaa.obj.get_branch_by_class( layer_ref, "color" )
end
aaa.layer.doc.create_color = "( layer_ref, name ) return a COLOR_REF encapsulating the c_color"
function aaa.layer.create_color( 		layer_ref, name )
	local ref = aaa.layer.get_color( layer_ref )
	if ref then
		return COLOR_REF:create( name, ref )
	end
end

aaa.layer.doc.get_shading = "( layer_ref ) return a ref on c_shading"
function aaa.layer.get_shading(		layer_ref )
	return aaa.obj.get_branch_by_class_no_error( layer_ref, "shading" )
end
aaa.layer.doc.create_shading = "( layer_ref, name ) return a SHADING encapsulating the c_shading"
function aaa.layer.create_shading( 	layer_ref, name )
	local sha_ref = aaa.layer.get_shading( layer_ref )
	if sha_ref then
		return SHADING:create( name, sha_ref )
	end
end


-- BANK, BIND Mapping
aaa.layer.doc.get_tex_unit = { 	"( layer_ref, id_unit ) return c_obj containing the bank/bind (2d/3d...) params,",
								"	why can be a c_layer or a c_texture_unit)."
							}
									-- " id_unit is 0 when missing"
function aaa.layer.get_tex_unit( layer_ref, id_unit )
	--aaa.print_fn()
	if id_unit==nil or id_unit==0 then
		--aaa.print( "ok return layer_ref because  is "..id_unit  )
		return layer_ref
	else
		if id_unit==1 or id_unit==2 or id_unit==3 then
			local texturing = aaa.obj.get_branch_by_class_no_error( layer_ref, "texturing" )
			if texturing then
				return param.get_obj_attached( texturing, "unit_"..id_unit )
			end
		end
		-- local tab = aaa.obj.get_branchs_by_class( texturing, "texture_unit" )
		-- local str_end = "unit_"..(id_unit-1)
		-- --GABU_OBJ:box_debug( str_end )
		-- for i = 1,#tab do
		-- 	local obj = tab[i]
		-- 	--GABU_OBJ:box_debug( aaa.obj.get_filename(obj).." "..aaa.obj.get_name(obj) )
		-- 	local name = aaa.obj.get_name(obj)
		-- 	if string.sub( name, -string.len(str_end) ) == str_end then
		-- 		--aaa.box_debug( "found "..obj )
		-- 		return obj
		-- 	end
		-- end
	end
end
aaa.layer.doc.get_tex_unit_mapping = "( layer_ref, id_unit ) return c_mapping for tex_unit, id_unit is 0 when missing"
function aaa.layer.get_tex_unit_mapping( layer_ref, id_unit )
	--aaa.print_fn()
	local obj = aaa.layer.get_tex_unit( layer_ref, id_unit )
	if obj then
		--aaa.box_debug( "from "..obj.."\nreturn "..aaa.obj.get_branch_by_class( obj, "mapping" ) )
		return aaa.obj.get_branch_by_class_no_error( obj, "mapping" )
	end
end

aaa.layer.doc.get_bank_bind_2d_ref = "( layer_ref, id_unit ) return bank/bind 2d param ref, id_unit is 0 by_default"
function aaa.layer.get_bank_bind_2d_ref( layer_ref, id_unit )
	local obj = aaa.layer.get_tex_unit( layer_ref, id_unit )
	if obj then
		--GABU_OBJ:box_debug( "found tex_unit for unit "..id_unit )
		return param.get_ref( obj, "bank_2d" ), param.get_ref( obj, "bind_2d" )
	end
end

aaa.layer.doc.build_bank_bind_2d_ref_table = "( layer_ref, id_unit ) return bank/bind 2d param ref, id_unit is 0 by_default"
function aaa.layer.build_bank_bind_2d_ref_table( layer_ref, id_unit_begin, id_unit_end )
	id_unit_begin = id_unit_begin or 0
	id_unit_end = id_unit_end or id_unit_begin
	local map = {}
	for id = id_unit_begin,id_unit_end do
		local bank_ref, bind_ref = aaa.layer.get_bank_bind_2d_ref( layer_ref, id )
		if bank_ref then
			map[id+1] = {	bank_2d = bank_ref,	bind_2d = bind_ref }
		end
	end
	return map
end

function aaa.layer.set_bind_2d(	layer_ref, bind )
	local ba, bi = aaa.img.make_bank_bind_2d( bind )
	param.set( layer_ref, "bank_2d", ba )
	param.set( layer_ref, "bind_2d", bi )
end

aaa.show_file_end( "aaa_layer" )
