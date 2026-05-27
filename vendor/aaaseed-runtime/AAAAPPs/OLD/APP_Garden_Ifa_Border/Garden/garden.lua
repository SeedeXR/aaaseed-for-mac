APP.DECLARE( "GARDEN_IFA_BORDER", APP_GP )

local L_APP = GARDEN_IFA_BORDER

function L_APP:init_app( ... )
	self:set_init_monitor_m_nb( 0 )
	if not oo.getsuper(L_APP).init_app( self, ... ) then return end

	self.boid_multi = self:get_meu_by_name( "BoidMulti_1" )
	self.binds = {}
	self:load_dir( "Latin_DEF", 1 )
	self:load_dir( "Arab_DEF", 2 )
	self:load_dir( "Heb_DEF", 3 )
	self.boid_multi:set_tex_anim( self.binds  )

	ga:set_onsite_by_machine( { "MAAHEX", "IFA-PC03-CT1" } )
	local b_onsite = 	self:is_onsite()
	ga:set_ui_group_active( not b_onsite )
	if b_onsite then
		ga:set_lock_app( true )
	end

	return true
end

function L_APP:set_media_dir()
end

function L_APP:load_dir( dir,id )
	local base_dir = app.media_dir_rel.."Floating_Border/2014/Letter/"
	dir = base_dir..dir.."/"
	local t_file = aaa.dir.get_files( dir, "*.tga" )
	if not t_file then
		self:box_error( "No file in "..dir )
	else
		self.binds[id] = {}
		local bind
		for i= 1, #t_file do
			MEDIA.set_dir( dir )
			self:print( "Garden will load letter image "..t_file[i] )
			bind = IMGS.get_bind( t_file[i] )
			if i==1 then self.binds[id][1] = bind end
		end
		self.binds[id][2] = bind
	end
end

if IS_BUSS_OPEN() then
	APP.CREATE_INST( L_APP )
end
