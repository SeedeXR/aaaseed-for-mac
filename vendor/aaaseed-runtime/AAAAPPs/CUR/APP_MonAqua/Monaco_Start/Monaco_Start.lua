if APP.DECLARE( "MONACO_AQUA", APP_GP ) then
	function MONACO_AQUA:set_media_dir()
		--todo why we use other path like ../../monaco later
		MEDIA.set_dir_media( "Monaco" )
	end
end

-- NETWORK UI
--
	--if aaa.net.is_remote() and app:is_master() then
	--if aaa.net.is_remote() then
local function get_meu_bu_name( bu )
	local bus = bu:get_bus_up_no_error()
	if bus then
		local meu = bus.__meu_owner
		if meu then
			--aaa.print_inverse( "meu.__b_meu_send is "..meu.__b_meu_send )
			--aaa.print_inverse( " app:is_bu_send() is ".. app:is_bu_send() )
			if meu.__b_meu_send or app:is_bu_send() then
				local bu_name = bu:get_name_lowercase()
				if bu_name then
					return meu:get_name(), bu_name
				end
			end
		end
	end
end

-- used todo the send in monaco
BUI:set_hook_on_value_change(
	function( bu, balue )
		if not bu:is_send() then return end

		local meu_name, bu_name = get_meu_bu_name( bu )
		if meu_name then
			local str
			if false then
				local val = balue:get_value()
				local id = balue.__id1
				--aaa.print( "id is "..id )
				app:send_method( "change_value_meu", "\""..meu_name.."\",\""..bu_name.."\","..id..","..val )
			else
				local values_str = bu:get_values_as_str_monaco()
				--app:print_inverse( "HOOK values str is "..values_str )
				app:send_method( "change_values_meu", "\""..meu_name.."\",\""..bu_name.."\","..values_str )
			end
		end
	end
)

--this is 2 different fns (value and values)
function MONACO_AQUA:change_values_meu( meu_name, bu_name, values )
--	if self:is_master() then return end

	table.print( values, "change_values_meu", 2 )

	local str = "change_values_meu( \""..meu_name.."\", \""..bu_name.."\", "..values.." )"
	self:print( str )
	local meu = app:get_meu_by_name_no_error( meu_name )
	if meu then
		--self:print( "change_value_meu() : found meu named : "..meu_name )
		if not (meu.__b_meu_send or app:is_bu_send()) then
			self:print( "change_values_meu() found meu :"..meu_name.."\n\twill try to change bu "..bu_name.." now." )
			meu:set_bu_values( bu_name, values )
		end
	end
end
function MONACO_AQUA:change_value_meu( meu_name, bu_name, id, val )
--	if self:is_master() then return end

	--local str = "change_value_meu( \""..meu_name.."\", \""..bu_name.."\", "..id..", "..val.." )"
	--self:print( str )
	local meu = app:get_meu_by_name_no_error( meu_name )
	if meu then
		--self:print( "change_value_meu() : found meu named : "..meu_name )
		if not (meu.__b_meu_send or app:is_bu_send()) then
			self:print( "change_value_meu() found meu :"..meu_name.."\n\twill try to change bu "..bu_name.." now." )
			meu:set_bu_value( bu_name, val, id )
		end
	end

end

function MONACO_AQUA:send_bu_do_method( bu, method_name )
	if not bu:is_send() then return end
	self:print( "send_bu_do_method "..bu.." "..method_name )
	local meu_name, bu_name = get_meu_bu_name( bu )
	if meu_name then
		self:send_method( "receive_bu_do_method", "\""..meu_name.."\",\""..bu_name.."\",\""..method_name.."\"" )
	end
end

function MONACO_AQUA:receive_bu_do_method( meu_name, bu_name, method_name )
--	if self:is_master() then return end
	local meu = app:get_meu_by_name_no_error( meu_name )
	if meu and not (meu.__b_meu_send or app:is_bu_send()) then
		self:print( "receive_bu_do_method() : found meu named : "..meu_name )
		local bu = meu:get_bu_by_key( bu_name )
		if bu then
			self:print( meu.." receive_bu_do_method() : found bu "..bu.." method ".. method_name )
			bu:do_fn( method_name )
		else
			self:print_error( meu.." receive_bu_do_method() : did not found bu named : "..bu_name )
		end
	else
	--	self:print_error( "receive_bu_do_method() : did not found meu named : "..meu_name )
	end
end


-- function MONACO_AQUA:update_before()
-- 	oo.getsuper(MONACO_AQUA).update_before(self)
-- end
-- function APP_GP:draw_after()
-- 	oo.getsuper(MONACO_AQUA).draw_after(self)
-- end

function MONACO_AQUA:init_app( ... )
	self:set_init_monitor_pass( true )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(MONACO_AQUA).init_app( self, ... ) then return end

	self:init_pc_info()
	self:build_fiches()
	ga:define_ui_customer( self )
	ga:set_ui_group_active( false,	"customer" )

	self:lock_onsite()
	self.b_use_dds = true
	self:define_seqs()

	IMGS.set_bind_free( IMGS.get_bind_free_start() + 256 )
--	GA:flip_ui_group_active( "customer" )

	self.b_shutdown_allowed = true
	self.volume_master = 1.0
	self.volume_playlist = 1.0
	self.volume_transition = 1.0
	self.scene_cur = ""
	self.scene_duration = 0.0
	self.scene_time = 0.0
	if self:is_tablet() then
		param.set( aaa.net.ref.obj, "active", 1 )
		ga:set_ui_group_active_all( false )
		ga:set_ui_group_active( true, "customer" )
	end
	return true
end

-- AQUA STUFF
--
function MONACO_AQUA:create_tank( meu )
	--self:box_debug( "meu is "..meu.."dir is\n"..meu:get_dir_absolute() )
	local tank = TANK:create( meu:get_name(), nil, meu, true )
	tank.__force_def_file = app:get_dir_absolute()..MUS:get_dir_MEU().."TankFish_1/DEF/fx.layers_param"
	--self:box_debug( "tank.__force_def_file is\n"..tank.__force_def_file )
	return tank
end

--todoopt master tablet don't needit
function MONACO_AQUA:is_caustic()		return true										end
function MONACO_AQUA:set_caustic_bind( bind )
	local meu = self:get_meu_by_name_cached( "LightPassV1_1" )
	--self:print( meu.." caustic bind is "..bind )
	meu:set_texture_bind_2d( 7, bind )
	--self:print( "Caustic bind : "..bind)
	--todomona not at every frame make it work...
	meu:set_bu_texture_save( 7, false )
end
function MONACO_AQUA:use_compressed_texture()
	return self.b_use_dds and true or false
end



if IS_BUSS_OPEN() then
	APP.CREATE_INST( MONACO_AQUA )
end

