--[[
APP.DECLARE( "APP_DESK", APP_GP )

function APP_DESK:add_apps_in_dir( dir )
	local dirs = aaa.dir.get_dirs( dir )
	if not dirs then return end

	local gp = self:get_gp()
	local mus = gp:get_mus_down()

	for i, name in ipairs(dirs) do
		local full_name = dir.."/"..name
		local files = aaa.dir.get_files( full_name, "*.dua" )
		if files then
			for i = 1,1 do --deal with several
				aaa.box_warning( #files.." "..files[i] )
				local inst
				if string.find( name, "APP_" ) == 1 then
					inst = string.sub( name, 5 )
				--elseif string.find( name, "Ex_" ) == 1 then
				--	inst = string.sub( name, 4 )
				else
					inst = name
				end
				self:print( i.." - "..full_name )
				APP_FACTORY.add( inst, nil, full_name )
				mus:create_mu_app( inst, full_name )	--tododesk remove full_name here
			end
		end
	end

end

function APP_DESK:init_app( ... )
--	self:load_medias()

	if not oo.getsuper(APP_DESK).init_app( self, 2, ... ) then return end

	self:add_apps_in_dir( "APP" )
	self:add_apps_in_dir( "APP_Examples" )

	-- read saved position
	local gp = GP.cur
	if gp then
		gp:get_bus_mus():load_bu_pos()
	end

	-- local mu = self:get_mu_by_name( "UI_1" )
	-- mu:set_ui_slot( 2 )
	-- --	used
	
	-- self.meu_video_gene	= self:get_meu_by_name(	"video_C"	)


	return true
end
--if zobi then
	--aaa.print( "will trigger strict" )
	--zobi = 1
--end
if IS_BUSS_OPEN() then
	APP.CREATE_INST( APP_DESK )
	--todo maa will solve line below
	--ga:add_ga_and_bu_windows( nil, { "SEQS", "SEQ", "PLAYLIST" } )
end

function APP_DESK:draw_after()
	oo.getsuper(APP_DESK).draw_after(self)
	--self:print( "render" )
end
--]]