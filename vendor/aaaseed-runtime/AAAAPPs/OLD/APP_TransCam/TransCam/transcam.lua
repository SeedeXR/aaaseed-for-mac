if APP.DECLARE( "TRANSCAM", APP_GP ) then
end

local L_APP = TRANSCAM

function L_APP:init_app( ... )
	self:set_init_monitor_m_nb( 0 )
	self:set_init_ui_slot_nb( 6 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end

	local filename = aaa.dir.get_dir_tracker().."/default_a"
	--self:box_debug( "filename "..filename )
	aaa.midi.load_global( filename )

	local set = aaa.midi.add_patch_global

	local off_dst = 104
	local src_ch  = 6
	for i=1,4 do
		local off_src = (i-1)*8
		set( 1, off_dst+i,		1, off_src+src_ch )	-- position
		set( 2, off_dst+i,		9, off_src+src_ch )	-- offset
		set( 1, off_dst+i+4,	2, off_src+src_ch )	-- volume
		set( 2, off_dst+i+4,	1, off_src+5 	  )	-- epaisseur
	end
	return true
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end

