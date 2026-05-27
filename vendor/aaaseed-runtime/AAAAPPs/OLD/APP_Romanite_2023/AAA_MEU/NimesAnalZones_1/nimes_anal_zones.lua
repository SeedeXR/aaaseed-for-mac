
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix,iy = 1,1
	local SY = 1
	local DY = .2

	local SXS = 4
	local SYS = SY * 2


	for i=1,self.zone_nb do
		local zone = {}
		zone.bu = self:add_slider(		{ix+i,iy,	1,SXS},		"Coverage "..i,		zone, "coverage",		0,	0,1 	)
		self.zones[i] = zone
	end
	--	:set_draw_min(false)	--todo works but not the right way ? solve later
	iy = iy + SXS + DY

	self:add_button(     	{ix,iy,			4,SY},		"OSC Send", 	self,	"b_send",	false		)
end


function meu:init()
	local ref = self.ref

	self.zones = {}
	self.zone_nb =  15

	local bdd = self:get_layer_bdd(1)
	ref.bdd		=	bdd 
	ref.cov_out =	param.get_ref( bdd, "out_coverage" )
	ref.ub		=	param.get_ref( bdd, "begin_u" )
	ref.vb		=	param.get_ref( bdd, "begin_v" )
	ref.ue		=	param.get_ref( bdd, "end_u" )
	ref.ve		=	param.get_ref( bdd, "end_v" )
end

function meu:update_ui()
	local ui = self.ui

end

function meu:draw()
	local ref = self.ref

	self:draw_layers_begin()

		local nb = self.zone_nb
		local f = 1/nb

		for i=1,self.zone_nb do
			local b = (i-1) * f
			param.set( ref.ub, b )
			param.set( ref.ue, b + f )

			aaa.obj.update_then_draw( ref.bdd )
			--self:draw_layer( 1 ) 
			local cov = param.get( ref.cov_out )
			self.zones[i].coverage = cov
		end

	self:draw_layers_end()
end

function meu:draw()
	local ref = self.ref

	self:draw_layers_begin()

		local nb = self.zone_nb
		local f = 1/nb

		local tab = {}
		for i=1,self.zone_nb do
			local b = (i-1) * f
			param.set( ref.ub, b )
			param.set( ref.ue, b + f )

			aaa.obj.update_then_draw( ref.bdd )
			--self:draw_layer( 1 ) 
			local cov = param.get( ref.cov_out )
			self.zones[i].coverage = cov
			tab[i] = cov
		end

		if self.b_send then
			app:send_osc( "romanite/presence/", unpack(tab) )
		end

	self:draw_layers_end()
end

