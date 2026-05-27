--	APP_FACTORY
--
if CLASS.DECLARE( "APP_FACTORY", GABU_OBJ ) then
	function APP_FACTORY.reset()
		APP_FACTORY.__desc		= {}
	end
	APP_FACTORY.reset()
	APP_FACTORY:set_class_status_doc(	CLASS.STATUS.EXPERIMENTAL,
										"used to try to have several app at the same time" )
end

function APP_FACTORY.get_descriptor( cname )
	cname = string.lower( cname )
	local desc = APP_FACTORY.__desc[cname]
	if not desc then
		aaa.box_error( "APP_FACTORY APP CLASS "..cname.." is not defined" )
	end
	return desc
end

function APP_FACTORY.add( cname, class, module_name_table )
	cname = string.lower( cname )
	if APP.verbose > 0 then
		aaa.debug.print_traceback()
		aaa.box_warning( "APP_FACTORY.add()", "cname = "..cname, "class = "..class, "module_name_table = "..module_name_table )
	end

	if module_name_table then
		local t = type(module_name_table)
		if t ~= "table" then
			if t == "string" then
				module_name_table = { module_name_table }
			else
				aaa.print( "APP_FACTORY.add() module_name_table "..module_name_table.." is not a table or a string as expected " )
				return
			end
		end
	end
	local desc = APP_FACTORY.__desc[cname]
	if desc then
		if class then
			if desc.class then
				aaa.box_error( "APP_FACTORY APP CLASS "..cname.." is already defined" )
			else
				desc.class = class
				class:box_warning( "class "..cname.." now have class object "..class )
			end
		end
	else
		desc = { class=class, module_name_table=module_name_table }
		APP_FACTORY.__desc[cname] = desc
	end
end

function APP_FACTORY:get_desc_field( cname, key )
	local desc = APP_FACTORY.get_descriptor( cname )
	if desc then
		local val = desc[key]
		if not val then
			aaa.box_error( "APP_FACTORY for APP CLASS "..cname.." don't have "..key )
		end
		return val
	end
end
function APP_FACTORY:get_module_name(	cname )	return APP_FACTORY:get_desc_field( cname, "module_name_table" )	end
--todo check if hack need to have table.print work on _G
function APP_FACTORY:get_class(		cname )	return APP_FACTORY:get_desc_field( cname, "class" )				end
--function APP_FACTORY:get_class(			cname )	return self				end
function APP_FACTORY:get_app_class(		cname )
	local capp = APP_FACTORY:get_class(	cname )
	if capp then return capp end

	local module_name =  APP_FACTORY:get_module_name( cname )
	if not module_name then return end

	local desc = APP_FACTORY.get_descriptor( cname )
	local modules = desc.modules_ref
	if not modules then
		modules = aaa.obj.create_by_cid( "modules" )
		param.set( modules, "group_load_only_one_file", true )
		param.set( modules, "group_save_only_one_file", true )
		desc.modules_ref = modules
	end
	if not modules then return end

	local mod_tab = desc.module_table
	if not mod_tab then
		mod_tab = {}
		local pref_start_ref = aaa.obj.get_from_top_by_class( "pref_start" )
		local b = param.get( pref_start_ref, "module_force_load_all" )
		param.set( pref_start_ref, "module_force_load_all", true )
		aaa.obj.set_searchable_by_filename( false )

		for i = 1, #module_name do
			local name = module_name[i]
			aaa.modules.set_module_ui( modules, i )
			param.set( modules, "module_index_end", i )
			local fname = name.."/default"
			local module = aaa.modules.new_module( modules, fname )
			param.set( module, "active", true )
			mod_tab[i] = module
		end

		aaa.obj.set_searchable_by_filename( true )
		param.set( pref_start_ref, "module_force_load_all", b )
		desc.module_table = mod_tab
	end
	if not desc.module_table then return end

	aaa.obj.update_then_draw( modules )
	aaa.obj.set_focus_ui( modules )
	return APP_FACTORY.get_class( cname )
end

function APP_FACTORY.create_app_inst( class_name, name )
	local class = APP_FACTORY:get_app_class( class_name )
	--aaa.box_warning( "APP_FACTORY.create_app_inst()", "is "..capp )
	local app
	if class then
		app = APP.CREATE_INST( class, name )
	end
	return app
end

