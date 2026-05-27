-- FBO fn for MEU
--
function MEU:get_meu_fbo_used()
	-- --todofbo
	-- local fi = self.__fbo_internal
	-- if fi then
	-- 	return fi.meu_fbo
	-- end
	return self.__meu_fbo_used
end
function MEU:__open_meu_fbo()
	local slot_id = self:get_ui_slot()
	local mfbo = self:get_meu_fbo_used()
	if mfbo and mfbo~=self then
		mfbo:set_ui_slot( slot_id-1 )
	end
end

-- FBO lower level
--
MEU.doc.__build_ref_fbo = "() called at init\n"..
						" if self.ref.fbo don't exist yet find fbo and reference it using a FBO encapsulating it"
function MEU:__build_ref_fbo()
	--self:print( "__build_ref_fbo()" )
	local ref = self.ref
	local fbo = ref.fbo
	if not fbo then
		fbo = aaa.obj.get_branch_by_class_no_error( self:__get_obj_main(), "fbo" )
		--fbo = aaa.obj.get_branch_by_class_no_error(	self:get_layers(), "fbo" )
		if fbo then
			--self:box_debug( self.."\ncreate a FBO GABU_OBJ" )
			fbo = FBO:create( self:get_inst_key(), fbo )
			ref.fbo = fbo
		end
	end
	return fbo
end

function MEU:get_meu_fbo_by_name( inst_key )
	--	aaa.print_fn()

	local meu_fbo = self:get_meu_by_name_no_error( inst_key )
	if not meu_fbo then
		meu_fbo = self:get_meu_by_name_no_error( "fbo_"..inst_key )
	end
	if not meu_fbo then
		self:show_error( "Can't find meu fbo \""..inst_key )
		--self:print_error( "Can't find mu fbo \""..instance_name )
		--local bu = self.ui.bu_ndc_monitor
		--if meu_fbo and bu then
			--local bind = meu_fbo:get_texture_bind_2d()
			--meu:box_debug( "bind is "..bind )
			--bu:set_texture_bind_2d( bind )
		--end
	end
	return meu_fbo
end

function MEU:set_meu_fbo( inst_key )
	local fi = self:get_table_always( "__fbo_internal" )
	fi.inst_key_asked = inst_key
end
function MEU:get_meu_fbo_name_ask()
	local fi =  self.__fbo_internal
	return fi and fi.inst_key_asked
end

function MEU:get_meu_fbo()
	local fi = self:get_table( "__fbo_internal" )
	if fi then
		local meu_fbo = fi.meu_fbo
		if meu_fbo then
			return meu_fbo
		end
		meu_fbo = self:get_meu_fbo_by_name( fi.inst_key_asked )
		fi.meu_fbo = meu_fbo
		return meu_fbo
	end
end
function MEU:use_meu_fbo_flipflop( fbo_nb, tex_unit_index_dst, fbo_attachment_id_src, attachment_table )
	self:set_meu_fbo( self.__fbo_internal.inst_key_asked )
	local meu_fbo = self:get_meu_fbo()
	if meu_fbo then
		meu_fbo:__set_flipflop_fbo( fbo_nb, tex_unit_index_dst, fbo_attachment_id_src, attachment_table )
		local bind = TEXS:get_bind_by_name( meu_fbo:get_inst_key() )
		if bind then
			self:update_ui_ndc_bind( bind )
		else
			self:print_error( "No bind here" )
		end
		return meu_fbo
	end
	self:show_error( "No fbo named "..self:get_meu_fbo_name_ask() )
	--self:print_debug( "use_meu_fbo_flipflop() no fbo named "..fbo_name )
end

function MEU:__update_fbo_internal( inst_key )
	inst_key = inst_key or self:get_inst_key()
	--todo deal with rename too or put fbo choose in MEU
	local fi = self.__fbo_internal -- fbo stuff
--	table.print( fi, "fi" ) 
	
	local meu_fbo
	if not fi then
		fi = {}
		self.__fbo_internal = fi
	end
	if fi.inst_key ~= inst_key then
		fi.meu_fbo = nil
	else
		meu_fbo = fi.meu_fbo
	end
	if not meu_fbo then
		fi.inst_key_asked = inst_key
		meu_fbo = self:define_fbo_internal( inst_key )
		if meu_fbo then
			fi.meu_fbo = meu_fbo
			fi.inst_key = inst_key
		end
	end
	if not meu_fbo then
		self:show_error( "No meu_Fbo internal found" )
		return
	end
	return meu_fbo
end

-- careful if you redefine
function MEU:get_fbo()
	local fbo = self.ref.fbo
	if fbo then
-- RETURN its own fbo
		return fbo	
	end
	
	local meu_fbo = self:get_meu_fbo_used()
	if meu_fbo then
		fbo = meu_fbo.ref.fbo
		if fbo then
-- RETURN the fbo used by the meu_fbo which handle it
			return fbo
		else
			self:box_debug( "MEU:get_fbo() Houston Houston we got a problem" )
		end
	else
		self:print_error( "MEU:get_fbo()self:get_meu_fbo_used() return nil" )
	end
end

function MEU:do_fbo( ... )
	--	aaa.print_fn()
	local meu_fbo = self:get_meu_fbo()
	if meu_fbo then
		--meu_fbo:print( "FBO use in ndc" )
		meu_fbo:do_fbo( ... )
	else
		self:print_error( "MEU:do_fbo() No meu_fbo" )
	end
end

function MEU:do_fbo_flipflop( b_verbose )
	--	aaa.print_fn()
	local meu_fbo = self:get_meu_fbo()	
	if meu_fbo then
		--meu_fbo:print( "FBO use in ndc" )
		meu_fbo:do_fbo_flipflop( b_verbose )
	else
		self:box_debug( "MEU:do_fbo_flipflop() No meu_fbo" )
	end
end

-- not used (2025 Feb)
function MEU:do_fbo_update()
	--	aaa.print_fn()
	local meu_fbo = self:get_meu_fbo()
	if meu_fbo then
		meu_fbo:update()
--	else
--		self:box_debug( "MEU:do_fbo_before() No meu_fbo" )
	end
end
--[[
function MEU:do_fbo_before()
	--	aaa.print_fn()
	local meu_fbo = self:get_meu_fbo()
	if meu_fbo then
		meu_fbo:do_fbo_before()
--	else
--		self:box_debug( "MEU:do_fbo_before() No meu_fbo" )
	end
end
function MEU:do_fbo_after()
	--	aaa.print_fn()
	local meu_fbo = self:get_meu_fbo()
	if meu_fbo then
		meu_fbo:do_fbo_after()
--	else
--		self:box_debug( "MEU:do_fbo_before() No meu_fbo" )
	end
end
--]]

--todo make sure it is dynamic

function MEU:__add_debug_fbo( ix,iy )
	--we don't have fbo yet
	if true then
		return
	end

	ix = ix or 9
	iy = iy or 1
	local SY = 1
	self:set_tab_key( "FboD" )

	local meu_fbo = self:get_meu_fbo()	
	if meu_fbo then
		local fbos = meu_fbo.__fbos
		for key,elt in PAIRS( fbos ) do
			self:print( "in __add_debug_fbo() FBO ".. elt )
			self:add_trig_method( {ix,iy,	4, SY},	"Fbo"..key, elt, "set_focus" )
			iy = iy + SY
		end
	else
		self:print_error( "__add_debug_fbo() No meu_fbo" )
	end
end
