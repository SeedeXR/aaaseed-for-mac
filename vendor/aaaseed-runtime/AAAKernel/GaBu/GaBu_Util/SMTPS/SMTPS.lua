aaa.lua.global.declare_table( "SMTPS" )

function SMTPS.reinit()
	SMTPS.smtps = {}
	SMTPS.__by_name = {}
	SMTPS.b_no_dialog_max_yet = true
end

function SMTPS.get_free_index( name )
	for i = 1, SMTPS.nb do
		if SMTPS.__by_name[i] == nil then
			return i
		end
	end
	local str = "While trying to create smtp for :"
						.."\n\t"..name
						.."\n\tAAASeed could not create smtp object"
						.."\n\tbecause this script limit the number of smtp to "..SMTPS.nb
						.."\n\tplease retry with less smtps !"
	if SMTPS.b_no_dialog_max_yet then
		aaa.box_error( str )
		SMTPS.b_no_dialog_max_yet = false
	else
		aaa.print_error( str )
	end
	return nil
end

function SMTPS.__assign_smtp( smtp, name, i )
	SMTPS.smtps[ i ]	= smtp
	SMTPS.__by_name[ i ]	= name
end

function SMTPS.__get_new( name )
	local index = SMTPS.get_free_index( name )
	if not index then return nil end

	local smtp = SMTP:create( index )
	if not smtp then
		aaa.print( "could not get a smtp obj for "..name )
		return nil
	end
	SMTPS.__assign_smtp( smtp, name, index )
	return smtp
end

function SMTPS.get_new( name )
	local smtp = SMTPS.__get_new( name )
	if not smtp then return nil end
	return smtp
end

function SMTPS.__find( name )
	for i = 1, #SMTPS.__by_name do
		if SMTPS.__by_name[i] and SMTPS.__by_name[i] == name then
			local smtp = SMTPS.smtps[ i ]
			return smtp, i
		end
	end
	return nil
end

function SMTPS.find( name )
	local smtp, i = SMTPS.__find( name )
	if smtp then
		aaa.print( "smtp "..name.." we use "..smtp )
	else
		aaa.print_error( "smtp "..name.." is not loaded" )
	end
	return smtp
end

function SMTPS.get( name )
	local smtp, i = SMTPS.__find( name )
	if smtp then
		aaa.print_debug( "smtp "..name.." already exist we reuse "..smtp )
	else
		smtp = SMTPS.get_new( name )
	end
	return smtp
end

function SMTPS.remove( name )
	local smtp, i = SMTPS.__find( name )
	if smtp then
		smtp:free()
		SMTPS.smtps[ i ] = nil
		SMTPS.__by_name[ i ]	= nil
	else
		aaa.print_debug( "smtp "..name.." don't exist, can't remove" )
	end
end

function SMTPS.init()
	if SMTPS.smtps then return end

	aaa.print( "smtps_init()" )
	SMTPS.nb = SMTP.SMTP_NB
	SMTPS.reinit()
end
SMTPS.init()

function SMTPS.free()
	if not SMTPS.smtps then return end
	aaa.print( "smtp_free()" )

	for i = 1 , SMTPS.nb do
		local smtp = SMTPS.smtps[i]
		if smtp then
			smtp:free()
		end
	end
	SMTPS.reinit()
end

