if APP.DECLARE( "APP_GP", APP ) then
	APP_GP:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
									"APP Garden Party mean the app have an interface of MEU and MU ..." )
end

function APP_GP:get_gp()		return	self.__gp			end
function APP_GP:set_gp( gp )	self.__gp = gp	return gp	end
function APP_GP:free()
	local gp = self:get_gp()
	if gp then
		gp:free()
		self:set_gp()
	end
	oo.getsuper(APP_GP).free( self )
end

function APP_GP:save_top_level()
	oo.getsuper(APP_GP).save_top_level( self )
	local gp = self:get_gp()
	if gp then
		gp:save_top_level()
	end
end

APP_GP.doc.get_mu_by_name			= GP.doc.get_mu_by_name
APP_GP.doc.get_mu_by_name_no_error	= GP.doc.get_mu_by_name_no_error
APP_GP.doc.get_meu_by_name			= GP.doc.get_meu_by_name
APP_GP.doc.get_meu_by_name_no_error	= GP.doc.get_meu_by_name_no_error
function APP_GP:get_mu_by_name( name_or_array )				return self:get_gp():get_mu_by_name( name_or_array )			end
function APP_GP:get_mu_by_name_no_error( name_or_array )	return self:get_gp():get_mu_by_name_no_error( name_or_array )	end
function APP_GP:get_meu_by_name( name_or_array )			return self:get_gp():get_meu_by_name( name_or_array )			end
function APP_GP:get_meu_by_name_no_error( name_or_array )	return self:get_gp():get_meu_by_name_no_error( name_or_array )	end
APP_GP.doc.get_meu_by_name_cached	= GP.doc.get_meu_by_name_cached
function APP_GP:get_meu_by_name_cached( name )				return self:get_gp():get_meu_by_name_cached( name )		end

function APP_GP:activate_meu( name, value )
	if value == nil then value = false end
	local m = self:get_meu_by_name_no_error( name )
	if m then	m:set_mu_value( value )
	else		self:print_error( "Couldn't find MEU \""..name.."\" to activate." )
	end
end

function APP_GP:update_before()
	local gp = self:get_gp()
	if gp then
		gp:update_before()
--	else
--		self:print_debug( "in update_before() Have no gp" )
	end
	oo.getsuper(APP_GP).update_before(self)
end
function APP_GP:draw_after()
	oo.getsuper(APP_GP).draw_after(self)
	local gp = self:get_gp()
	if gp then
		--aaa.print_fn()
		gp:render()
--	else
--		self:print_debug( "in draw_after() Have no gp" )
	end
end

function APP_GP:set_init_ui_slot_nb( nb ) 	self.__init_ui_slot_nb = nb 	end
function APP_GP:set_init_monitor_pass( b )	self.__b_init_monitor_pass = b	end
function APP_GP:set_init_monitor_m_nb( nb )	self.__init_monitor_m_nb = nb	end


function APP_GP:set_media_dir()
	self:print_debug( "this APP_GP don't define media dir, we use Garden by default" ) 
	MEDIA.set_dir_media( "Garden" )
	--self:box_error( "this APP_GP don't define media dir" )
end

function APP_GP:init_app( ... )
	if oo.getsuper(APP_GP).init_app( self, ... ) then
		self:set_media_dir()
		local gp = GP:create( "Garden", self:get_dir_absolute() )
		self:set_gp( gp )
		ga:init_dialogs()
		--tododesk find a better way to pass data under
		gp:define_ui_classic( self.__init_ui_slot_nb, self.__b_init_monitor_pass, self.__init_monitor_m_nb )

		self.__mu_out	 = self:get_mu_by_name_no_error( { "Dir_Out", "Out_1" } )
		self:lock_onsite()
		if self.__mu_out and self:is_onsite() then
			self.__mu_out:set_value( 1 )
		end

		ga:set_ui_group_active_all( false )
		return true
	end
end

function APP_GP:mark_by_type( meu_type )
	local mus = GP.cur:get_mus_down()
	mus:mark_by_type( meu_type )
end
function APP_GP:mark_by_match_pattern( value )
	local mus = GP.cur:get_mus_down()
	local status, result = pcall( MUS.mark_by_match_pattern, mus, value )
	if not status then
		self:print_debug( "we probably use a bad pattern : "..value )
		self:print_debug( "    "..result )
	end

	ga:apply_method_to_all_bu( "mark_by_match_pattern", value )
end

function APP_GP:start_dialog_find_mu()
	ga:add_dialog_edit{ title="Find MU and BU containing", param_type="string", b_simple=true, obj=self }
end

function APP_GP:do_dialog_hook( what, value, dialog_table )
	--self:print( "APP_GP:do_dialog_hook()" )
	aaa.print_method()
	if what == "change_value" then
		if value then
			value = string.lower(value)
			self:mark_by_match_pattern( value )
		end
	elseif what == "close" then
	end
	return true
end


function APP_GP:do_key( key )
	self:print_do_key( "APP_GP", key )

	local b_ctrl = aaa.keyboard.is_ctrl()
	local b_shift = aaa.keyboard.is_shift()
	if key == 6 then	-- CTRL f
		self:print( "do CTRL f" )
		self:start_dialog_find_mu()
		return true
	elseif	key == 19 and b_ctrl and b_shift then -- SHIFT CTRL s
		self:print( "do SHIFT CTRL s" )
		MEU:save_all()
		-- the CTRL S goe also to AAASeed so the env is saved too
		return true
	end
	
	return oo.getsuper(APP_GP).do_key( self, key )
end
