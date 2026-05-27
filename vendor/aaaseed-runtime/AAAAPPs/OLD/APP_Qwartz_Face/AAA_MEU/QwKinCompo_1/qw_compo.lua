function meu:init()
	local ref = self.ref
	local sha = aaa.obj.get_down_by_class( ref.obj, "shading" )
		ref.compo = param.get_ref( sha, "fu_float_05" )
end

function meu:update()
	local compo = app.compo
	local f
	--self:print( cov[1]+cov[2] )
	if app.b_coverage then	f = 1
	else					f = -4
	end
	--self:print( compo )
	compo = clamp_01( compo + f * aaa.time.dt )
	--compo = compo + f * aaa.time.dt
	app.compo = compo
	--self:print( "       "..compo )
	param.set( self.ref.compo, compo )
end

