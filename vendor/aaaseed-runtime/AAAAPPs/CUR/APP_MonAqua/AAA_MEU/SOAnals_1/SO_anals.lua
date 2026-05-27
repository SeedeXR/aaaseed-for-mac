function meu:draw_icon()
	gol.set_line_width( 2 )
	aaa.draw_str_xy( "Anal", -.45, -.3, .15, .8 )
end

		--todo deal with color
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local ix, iy = 1, 2
	local SY = .8
	local DY = .5

	self:add_camera()

	self:add_slider(	{ix,iy,	8,SY},	"Coverage_min", 	self,	"coverage_min", 		.004, 	0, 1 )
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},	"Coverage_max", 	self,	"coverage_max",	  	1, 		0, 1 )
	iy = iy + SY + DY

	self:add_slider_two(	{ix,iy,	8,SY},	"Speed_threshold", 	self,	"speed_threshold_min",
																		"speed_threshold_max",	.2, .5,	0, 1 )
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},	"time Down", 		self,	"time_down",		.5, 	0.001, 1 )
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},	"time Up", 			self,	"time_up",			.5, 	0.001, 1 )
	iy = iy + SY
end

function meu:init()
	local ref = self.ref
	local function prepare_anal( layer_id )
		local anal  = { ref={}, x_last=0, z_last=0, cov_factor=1 }
		local bdd = self:get_layer_bdd( layer_id )
		local t = anal.ref
		t.cov_min	= param.get_ref( bdd, "coverage_min" )
		t.cov_max	= param.get_ref( bdd, "coverage_max" )
		t.cov		= param.get_ref( bdd, "out_coverage" )
		t.u			= param.get_ref( bdd, "out_center_u" )
		t.v			= param.get_ref( bdd, "out_center_v" )
		t.ub		= param.get_ref( bdd, "begin_u"		)
		t.ue		= param.get_ref( bdd, "end_u"		)
		t.vb		= param.get_ref( bdd, "begin_v"		)
		t.ve		= param.get_ref( bdd, "end_v"		)
		return anal
	end

	self.anals_kake = {}
	for i=1,8 do	self.anals_kake[i] = prepare_anal( 5+i )	end

	self.anals_clam = {}
	for i=1,7 do	self.anals_clam[i] = prepare_anal( 14+i )	end

	self.anals_nemo = {}
	for i=1,3 do	self.anals_nemo[i] = prepare_anal( 22+i )	end

end
function meu:__get_cov_xz_uv( anal, b_fix_removed, pre )
	local t = anal.ref
	param.set( t.cov_min, self.coverage_min )
	param.set( t.cov_max, self.coverage_max )
	local cov	= param.get( t.cov	)
	local	u	= param.get( t.u	)
	local	v	= param.get( t.v	)
	local	ub	= param.get( t.ub	)
	local	ue	= param.get( t.ue	)
	local	vb	= param.get( t.vb	)
	local	ve	= param.get( t.ve	)


	local x = ( ub + u * (ue-ub) ) * 20.75
	local z = ( vb + v * (ve-vb) ) * -8. + 4.

	if b_fix_removed and cov > 0 then
		local dx = x - anal.x_last
		local dz = z - anal.z_last
		anal.x_last = x
		anal.z_last = z
		local speed = math.sqrt( dx*dx + dz*dz ) / aaa.time.dt
		if speed < self.speed_threshold_min then
			anal.cov_factor = clamp_01( anal.cov_factor - aaa.time.dt / self.time_down )
		elseif self.speed_threshold_max < speed then
			anal.cov_factor = clamp_01( anal.cov_factor + aaa.time.dt / self.time_up )
		end

		if pre then
			self:print( pre.." speed "..speed.."\t"..anal.cov_factor    )
		end
		cov = cov * anal.cov_factor
	end

	return cov, x,z, u,v
end

function meu:do_anals_kake()
--todo optimize on change only
	--self:print( "do_anals_kake() " )
	local anals = self.anals_kake
	local str =	"local t = aaa.lua.global.get(\"app\")\n"..
				"local fn = t and t.update_presence_SO "..
				"if fn then "
	for i=1,#anals do
		local cov, x,z, u,v = self:__get_cov_xz_uv( anals[i], true )	--, "kake_"..i )
		if cov~=0 then aaa.show( cov.." with "..x.." , "..z, "Kake_Cov_"..i ) end
		--if cov > 0 then	self:print( i.." cov at "..cov.." at "..x.." z "..z ) end
		app:update_presence_SO( i, cov, x,z, u,v )
		str = str.."fn(app,"..i..","..cov..","..x..","..z..","..u..","..v..") "
	end
	str = str.."end "
	--self:print( str )
	aaa.net.lua_send( 0, 0, str )
end

function meu:do_anals_clam()
--todo optimize on change only
	--self:print( "do_anals_clam() " )
	local anals = self.anals_clam
	for i=1,#anals do
		local cov, x, z = self:__get_cov_xz_uv( anals[i] )
		--if cov~=0 then aaa.show( cov.." with "..x.." , "..z, "CLam_"..i ) end
		--if cov > 0 then self:print( i.." cov at "..cov.." at "..x.." z "..z ) end
		-- clams are only on SO, no need to send it to network
		app:update_presence_clam_SO( i, cov, x,z	)
	end
end

function meu:do_anals_nemo()
--todo optimize on change only
	local anals = self.anals_nemo
	local str =	"local t = aaa.lua.global.get(\"app\")\n"..
				"local fn = t and t.update_presence_nemo_SO "..
				"if fn then "
	for i=1,#anals do
		local cov, x, z = self:__get_cov_xz_uv( anals[i] )
		--if cov~=0 then aaa.show( cov.." with "..x.." , "..z, "Nemo_"..i ) end
		--if cov > 0 then self:print( i.." cov at "..cov.." at "..x.." z "..z ) end
		--app:update_presence_nemo_SO( i, cov, x,z	)
		str = str.."fn(app,"..i..","..cov..","..x..","..z..") "
	end
	str = str.."end "
	aaa.net.lua_send( 0, 0, str )
end

function meu:is_update_and_draw()
	return app:is_so() or app:is_pc_dev()
end

function meu:update()
	if self:is_update_and_draw() then
		self:do_anals_kake()
		self:do_anals_clam()
		self:do_anals_nemo()
	end
end

function meu:draw()
	if self:is_update_and_draw() then
		MEU.draw(self)
	end
end	