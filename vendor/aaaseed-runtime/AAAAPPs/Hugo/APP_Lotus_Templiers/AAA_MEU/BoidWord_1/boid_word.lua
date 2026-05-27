
function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	local ix = 1
	local iy = 1

	self:add_camera()

	self:add_trig_restart(	{	ix,iy+1})

	ref.boid = aaa.obj.get_branch_by_class( self:get_layer( 1 ), "bdd_boid" )
	aaa.debug.show( ref.boid )
	ref.nb_alloc = param.get_ref( ref.boid, "nb_allocated")
end

function meu:restart()
	param.set( self.ref.boid, "restart_trig", 1  )
end

local words =
{
	"City of London",
	"Investment philosophy",
	"Expertise",
	"People",
	"Performance",
	"Asset manager",
	"International",
	"Real estate and cash",
	"Clients",
	"For over 80 years",
	"Long-term",
	"Conviction-led",
	"Stockmarket",
	"Career",
	"Challenges",
	"Active management",
	"Best returns"
}

function meu:draw()
	oo.getclass(self).draw( self )
	--if true then return end

	local ref = self.ref
	local boid = ref.boid
	local nb = aaa.bdd.get_point_nb( boid )
	local r = .2
	local s = .8
	local os = 1/s
	local words_nb = #words
	gol.set_texture_dim( 0 )

	local ox = -2.
	local oy = -.5
	local	get_point = aaa.bdd.get_point_and_id_local
	for i=1,nb do
		local x,y,z,id = get_point( boid, i )
		gol.translate( x, y, z )
			local s = math.max( (z+1) *.02, .02 )
			gol.scale( s )
				gol.translate( ox, oy )
				local word = words[ id % words_nb + 1]
				gol.color_black()
				aaa.draw_str( word )
				local g = (z+.5)*2
				gol.color( g, (2-z)*5 )
				gol.translate( -s*.1, s*.1, s*.05 )
				aaa.draw_str_xy( word )
				gol.translate( s*.1-ox, -s*.1-oy, -s*.05	 )
			gol.scale( 1/s )

		gol.translate( -x, -y, -z )
	end

end
